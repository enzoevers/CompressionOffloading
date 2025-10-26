import os
from pathlib import Path
import subprocess
import shutil
import EnvironmentConfig
import argparse

parser = argparse.ArgumentParser(description="Build and install external libraries")

targetPlatformOptions = ["host", "zynq"]

parser.add_argument(
    "--TargetPlatform",
    action="store",
    dest="targetPlatform",
    choices=targetPlatformOptions,
    default="host",
    help="Target platform (default: %(default)s)",
)

args = parser.parse_args()

targetPlatform: EnvironmentConfig.Platform
match args.targetPlatform:
    case "host":
        targetPlatform = EnvironmentConfig.Platform.OsNameToPlatform(os.name)
    case "zynq":
        targetPlatform = EnvironmentConfig.Platform.ZYNQ
    case _:
        raise Exception("Unsupported target platform: {}".format(args.targetPlatform))


CurrentScriptPath = Path(os.path.dirname(os.path.abspath(__file__)))
RepositoryRootPath = Path(CurrentScriptPath.parent)
ExternalLibPath = Path(RepositoryRootPath / "External")


def BuildAndInstallCoDeLib(
    buildEnv: EnvironmentConfig.EnvironmentConfiguration,
    buildConfig: EnvironmentConfig.BuildConfig = EnvironmentConfig.BuildConfig.DEBUG,
):
    ProjectName = "CoDeLib"

    BuildTypeString = EnvironmentConfig.BuildConfig.ToCMakeBuildType(buildConfig)
    targetPlatformString = EnvironmentConfig.Platform.PlatformToOsName(
        buildEnv.GetTargetPlatform()
    )

    ExternalMinizipNgLibInstallPath = Path(
        ExternalLibPath / "minizip-ng/Install" / targetPlatformString / BuildTypeString
    )

    CoDeLibRootPath = Path(RepositoryRootPath / ProjectName)
    BuildDirectory = Path(
        CoDeLibRootPath / "Build" / targetPlatformString / BuildTypeString
    )
    InstallDirectory = Path(
        CoDeLibRootPath / "Install" / targetPlatformString / BuildTypeString
    )

    if not BuildDirectory.exists():
        BuildDirectory.mkdir(parents=True)

    if InstallDirectory.exists():
        shutil.rmtree(InstallDirectory)
        InstallDirectory.mkdir(parents=True)

    os.chdir(RepositoryRootPath)

    print("==============================")
    print(ProjectName + ": Configuring ({})".format(BuildTypeString))
    print("==============================")
    configureCommand = 'cmake -G "{0}" -DCMAKE_TOOLCHAIN_FILE="{1}" -S "{2}" -B "{3}" -DCMAKE_INSTALL_PREFIX="{4}" -DCMAKE_BUILD_TYPE={5} -DMINIZIP_INSTALL_PATH="{6}"'.format(
        buildEnv.GetCmakeGenerator(),
        Path(
            RepositoryRootPath
            / "build/conan/build"
            / BuildTypeString
            / "generators"
            / "conan_toolchain.cmake"
        ),
        CoDeLibRootPath,
        BuildDirectory,
        InstallDirectory,
        BuildTypeString,
        ExternalMinizipNgLibInstallPath,
    )
    print(configureCommand)
    subprocess.run(
        configureCommand,
        shell=True,
        check=True,
    )

    print("==============================")
    print(ProjectName + ": Building ({})".format(BuildTypeString))
    print("==============================")
    buildCommand = "cmake --build {0} --config {1} -- -j 4".format(
        BuildDirectory, BuildTypeString
    )
    print(buildCommand)
    subprocess.run(
        buildCommand,
        shell=True,
        check=True,
    )

    print("==============================")
    print(ProjectName + ": Installing ({})".format(BuildTypeString))
    print("==============================")
    installCommand = "cmake --install {0}".format(BuildDirectory)
    print(installCommand)
    subprocess.run(
        installCommand,
        shell=True,
        check=True,
    )


BuildEnv = EnvironmentConfig.EnvironmentConfiguration(
    RepositoryRootPath, targetPlatform
)
BuildAndInstallCoDeLib(BuildEnv, EnvironmentConfig.BuildConfig.DEBUG)
BuildAndInstallCoDeLib(BuildEnv, EnvironmentConfig.BuildConfig.RELEASE)
