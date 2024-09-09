import os
from pathlib import Path
import subprocess
import shutil

CurrentScriptPath = Path(os.path.dirname(os.path.abspath(__file__)))
RepositoryRootPath = Path(CurrentScriptPath.parent)
ExternalLibPath = Path(RepositoryRootPath / "External")
ExternalLibInstallPath = Path(ExternalLibPath / "Install")

ProjectName = "CoDeLib"
CoDeLibRootPath = Path(RepositoryRootPath / ProjectName)
BuildDirectory = Path(CoDeLibRootPath / "Build")
InstallDirectory = Path(CoDeLibRootPath / "Install")

if not BuildDirectory.exists():
    BuildDirectory.mkdir(parents=True)

if InstallDirectory.exists():
    shutil.rmtree(InstallDirectory)
    InstallDirectory.mkdir(parents=True)

os.chdir(RepositoryRootPath)


def GetGCCPath() -> Path:
    # Check if Windows, Linux or MacOS and return the path to the GCC compiler
    # using a build in command
    platform = os.name
    if platform == "nt":
        # Windows
        return (
            subprocess.check_output("where gcc", shell=True)
            .decode("utf-8")
            .split("\n")[0]
        )
    elif platform == "posix":
        # Linux or MacOS
        return (
            subprocess.check_output("which gcc", shell=True)
            .decode("utf-8")
            .split("\n")[0]
        )
    else:
        raise Exception("Unknown platform: {}".format(platform))


def GetCmakeGenerator() -> str:
    platform = os.name
    if platform == "nt":
        # Windows
        return "MinGW Makefiles"
    elif platform == "posix":
        # Linux or MacOS
        return "Unix Makefiles"
    else:
        raise Exception("Unknown platform: {}".format(platform))


GCCPath = GetGCCPath()
CmakeGenerator = GetCmakeGenerator()
print("GCCPath: {}".format(GCCPath))
print("CmakeGenerator: {}".format(CmakeGenerator))

print("==============================")
print(ProjectName + ": Configuring")
print("==============================")
configureCommand = 'cmake -G "{0}" -DCMAKE_C_COMPILER="{1}" -S "{2}" -B "{3}" -DCMAKE_INSTALL_PREFIX="{4}" -DZLIB_ROOT="{5}" -DCMAKE_BUILD_TYPE=Debug'.format(
    CmakeGenerator,
    GCCPath,
    CoDeLibRootPath,
    BuildDirectory,
    InstallDirectory,
    ExternalLibInstallPath,
)
print(configureCommand)
subprocess.run(
    configureCommand,
    shell=True,
    check=True,
)

print("==============================")
print(ProjectName + ": Building")
print("==============================")
buildCommand = "cmake --build {0} --config Debug".format(BuildDirectory)
print(buildCommand)
subprocess.run(
    buildCommand,
    shell=True,
    check=True,
)

print("==============================")
print(ProjectName + ": Installing")
print("==============================")
installCommand = "cmake --install {0}".format(BuildDirectory)
print(installCommand)
subprocess.run(
    installCommand,
    shell=True,
    check=True,
)
