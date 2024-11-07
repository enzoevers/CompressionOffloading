import os
from pathlib import Path
import subprocess
from enum import Enum
import zipfile
import shutil
import stat


class BuildConfig(Enum):
    DEBUG = 1
    RELEASE = 2

    @classmethod
    def ToCMakeBuildType(cls, buildConfig: "BuildConfig") -> str:
        if buildConfig == cls.DEBUG:
            return "Debug"
        elif buildConfig == cls.RELEASE:
            return "Release"
        else:
            raise Exception("Unknown build config: {}".format(buildConfig))


class Platform(Enum):
    WINDOWS = 1
    LINUX = 2
    ZYNQ = 3

    @classmethod
    def OsNameToPlatform(cls, osName: str) -> "Platform":
        if osName == "nt":
            return cls.WINDOWS
        elif osName == "posix":
            return cls.LINUX
        elif osName == "posix-zynq":
            return cls.ZYNQ
        else:
            raise Exception("Unknown platform: {}".format(osName))

    @classmethod
    def PlatformToOsName(cls, platform: "Platform") -> str:
        if platform == cls.WINDOWS:
            return "nt"
        elif platform == cls.LINUX:
            return "posix"
        elif platform == cls.ZYNQ:
            return "posix-zynq"
        else:
            raise Exception("Unknown platform: {}".format(platform))


def on_rm_error(func, path, exc_info):
    # path contains the path of the file that couldn't be removed
    # let's just assume that it's read-only and unlink it.
    os.chmod(path, stat.S_IWRITE)
    os.unlink(path)


class EnvironmentConfiguration:
    def __init__(
        self,
        repositoryRootPath: Path,
        targetPlatform: Platform = Platform.OsNameToPlatform(os.name),
        hostPlatform: Platform = Platform.OsNameToPlatform(os.name),
    ):
        self.__targetPlatform = targetPlatform
        self.__hostPlatform = hostPlatform
        self.__repositoryRootPath: Path = repositoryRootPath
        self.__gccPath: Path
        self.__cmakeGenerator: str = ""
        self.__gccTargetSpecificCompilerOptionsString: str = ""
        self.__customToolChainPath: Path = (
            repositoryRootPath / "customToolchain.toolchain"
        )
        self.__ZyboSdkSysrootPath: Path = (
            repositoryRootPath / "ZyboEmbeddedLinux/Petalinux/ZyboSdkSysroot"
        )
        self.__ZyboCrossCompilerBasePath: Path = (
            repositoryRootPath / "ZyboEmbeddedLinux/CrossCompilers/aach32/"
        )

        self.__SetupEnvironment(targetPlatform, hostPlatform)

    def __del__(self):
        self.__RemoveCustomToolChainFile()

    def __GetProgramPath(self, programName: str, hostPlatform: Platform) -> Path:
        match hostPlatform:
            case Platform.WINDOWS:
                return (
                    subprocess.check_output("where {}".format(programName), shell=True)
                    .decode("utf-8")
                    .split("\n")[0]
                )
            case Platform.LINUX:
                # Linux or MacOS
                return (
                    subprocess.check_output("which {}".format(programName), shell=True)
                    .decode("utf-8")
                    .split("\n")[0]
                )
            case _:
                raise Exception(
                    "Unsupported host platform: {}".format(
                        Platform.OsNameToPlatform(hostPlatform)
                    )
                )

    def __GetGccPath(self, targetPlatform: Platform, hostPlatform: Platform) -> Path:
        if targetPlatform == hostPlatform:
            return self.__GetProgramPath("gcc", hostPlatform)
        elif targetPlatform == Platform.ZYNQ:
            gccName = "arm-xilinx-linux-gnueabi-gcc"
            if hostPlatform == Platform.WINDOWS:
                gccName += ".exe"

            fullPath = self.__ZyboCrossCompilerBasePath
            match hostPlatform:
                case Platform.WINDOWS:
                    fullPath = fullPath / "Windows"
                case Platform.LINUX:
                    fullPath = fullPath / "Linux"
                case _:
                    raise Exception(
                        "Unsupported host platform: {}".format(
                            Platform.OsNameToPlatform(hostPlatform)
                        )
                    )
            fullPath = fullPath / "compiler/bin/arm-xilinx-linux-gnueabi/" / gccName
            return fullPath
        else:
            raise Exception(
                "This class does not support cross-compilation: {}".format(
                    targetPlatform
                )
            )

    def __GetCmakeGenerator(self, hostPlatform: Platform) -> str:
        match hostPlatform:
            case Platform.WINDOWS:
                return "MinGW Makefiles"
            case Platform.LINUX:
                return "Unix Makefiles"
            case _:
                raise Exception(
                    "Unsupported host platform: {}".format(
                        Platform.OsNameToPlatform(hostPlatform)
                    )
                )

    def __GetTargetSpecificCompilerOptionsString(
        self, targetPlatform: Platform
    ) -> Path:
        match targetPlatform:
            case Platform.WINDOWS:
                return ""
            case Platform.LINUX:
                return ""
            case Platform.ZYNQ:
                # Based on ~/petalinux/BasicLinuxZybo/images/linux/sdk/environment-setup-cortexa9t2hf-neon-xilinx-linux-gnueabi
                return "-mthumb -mfpu=neon -mfloat-abi=hard -mcpu=cortex-a9 -fstack-protector-strong  -O2 -D_FORTIFY_SOURCE=2 -Wformat -Wformat-security -Werror=format-security"
            case _:
                raise Exception("Unknown target platform: {}".format(targetPlatform))

    def __CreateCustomToolChainFile(self, targetPlatform: Platform):
        content: str = ""

        content = content + "set(CMAKE_C_COMPILER {} {})\n".format(
            Path(self.__gccPath).as_posix(),
            self.__gccTargetSpecificCompilerOptionsString,
        )

        match targetPlatform:
            case Platform.ZYNQ:
                content = content + 'set(CMAKE_SYSROOT "{}")\n'.format(
                    self.__ZyboSdkSysrootPath.as_posix()
                )
                content = content + "set(CMAKE_SYSTEM_PROCESSOR arm)\n"
                content = content + "set(CMAKE_SYSTEM_NAME Generic)\n"
                content = (
                    content + "set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)\n"
                )
            case _:
                print(
                    "No platform specific toolchain required for platform: {}".format(
                        Platform.PlatformToOsName(targetPlatform)
                    )
                )

        with open(self.__customToolChainPath, "w") as file:
            file.write(content)

    def __RemoveCustomToolChainFile(self):
        if self.__customToolChainPath.exists():
            self.__customToolChainPath.unlink()

    def __SetCrossCompilerPermissions(self, targetPlatform: Platform):
        match targetPlatform:
            case Platform.ZYNQ:
                for root, _, files in os.walk(self.__ZyboCrossCompilerBasePath):
                    for file in files:
                        path = os.path.join(root, file)
                        os.chmod(path, 0o777)
                return

            case _:
                return

    def __SetupEnvironment(
        self,
        targetPlatform: Platform,
        hostPlatform: Platform,
    ):
        self.__gccPath = self.__GetGccPath(targetPlatform, hostPlatform)
        self.__cmakeGenerator = self.__GetCmakeGenerator(hostPlatform)
        self.__gccTargetSpecificCompilerOptionsString = (
            self.__GetTargetSpecificCompilerOptionsString(targetPlatform)
        )
        self.__CreateCustomToolChainFile(targetPlatform)
        self.__SetCrossCompilerPermissions(targetPlatform)

        print("Host platform: {}".format(hostPlatform))
        print("Target platform: {}".format(targetPlatform))
        print("gcc path: {}".format(self.__gccPath))
        print("CMake cenerator: {}".format(self.__cmakeGenerator))
        print(
            "gcc target specific options: {}".format(
                self.__gccTargetSpecificCompilerOptionsString
            )
        )
        print("Custom toolchain cmake path: {}".format(self.__customToolChainPath))

    def GetGccPath(self) -> Path:
        return self.__gccPath

    def GetCmakeGenerator(self) -> str:
        return self.__cmakeGenerator

    def GetGccTargetSpecificCompilerOptionsString(self) -> str:
        return self.__gccTargetSpecificCompilerOptionsString

    def GetCustomToolChainPath(self) -> Path:
        return self.__customToolChainPath

    def GetTargetPlatform(self) -> Platform:
        return self.__targetPlatform
