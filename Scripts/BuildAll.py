import os
from pathlib import Path
import subprocess

CurrentScriptPath = Path(os.path.dirname(os.path.abspath(__file__)))
ProjectRootPath = Path(CurrentScriptPath.parent)
TopLevelCMakeListsDirectory = Path(ProjectRootPath / "Benchmark")
BuildDirectory = Path(ProjectRootPath / "Build")

if not BuildDirectory.exists():
    BuildDirectory.mkdir()

os.chdir(ProjectRootPath)


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

configureCommand = 'cmake -G "{0}" -DCMAKE_C_COMPILER="{1}" -S {2} -B {3}'.format(
    CmakeGenerator, GCCPath, TopLevelCMakeListsDirectory, BuildDirectory
)
subprocess.run(
    configureCommand,
    shell=True,
    check=True,
)

buildCommand = "cmake --build {0}".format(BuildDirectory)
subprocess.run(
    buildCommand,
    shell=True,
    check=True,
)
