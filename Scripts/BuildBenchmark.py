import os
from pathlib import Path
import subprocess

CurrentScriptPath = Path(os.path.dirname(os.path.abspath(__file__)))
RepositoryRootPath = Path(CurrentScriptPath.parent)
BenchmarkRootPath = Path(RepositoryRootPath / "Benchmark")
BuildDirectory = Path(BenchmarkRootPath / "Build")
CoDeLibInstallDirectory = Path(RepositoryRootPath / "CoDeLib" / "Install")
ExternalLibPath = Path(RepositoryRootPath / "External")
ExternalLibInstallPath = Path(ExternalLibPath / "Install")

if not BuildDirectory.exists():
    BuildDirectory.mkdir(parents=True)

os.chdir(RepositoryRootPath)


def GetGCCPath() -> str:
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

print("Configuring CMake")
configureCommand = 'cmake -G "{0}" -DCMAKE_C_COMPILER="{1}" -DCMAKE_PREFIX_PATH="{2}" -DCMAKE_BUILD_TYPE=Debug -S {3} -B {4} -DZLIB_ROOT="{5}"'.format(
    CmakeGenerator,
    GCCPath,
    CoDeLibInstallDirectory,
    BenchmarkRootPath,
    BuildDirectory,
    ExternalLibInstallPath,
)
subprocess.run(
    configureCommand,
    shell=True,
    check=True,
)

print("Building Benchmark")
buildCommand = "cmake --build {0} --config Debug".format(BuildDirectory)
subprocess.run(
    buildCommand,
    shell=True,
    check=True,
)
