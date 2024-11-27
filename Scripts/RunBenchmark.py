import os
import stat
from pathlib import Path
import subprocess
import re
import EnvironmentConfig
from typing import List
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
BenchmarkRootPath = Path(RepositoryRootPath / "Benchmark")
BenchmarkTestFilesPath = Path(BenchmarkRootPath / "Files")

os.chdir(RepositoryRootPath)


def escape_ansi(line):
    ansi_escape = re.compile(r"\x1b[^m]*m")
    return ansi_escape.sub("", line)


def AppendBenchmarkResultToTotalResultFile(
    BenchmarkResultsFileName: str, BenchmarkResultsFileRawNames: List[str]
):
    BenchmarkResultsFile = open(BenchmarkResultsFileName, "a")

    for BenchmarkResultsFileRawName in BenchmarkResultsFileRawNames:
        BenchmarkResultsFileRaw = open(BenchmarkResultsFileRawName, "r")
        BenchmarkResultsFileRaw.seek(0)

        for line in BenchmarkResultsFileRaw:
            FormattedLine = escape_ansi(line)
            BenchmarkResultsFile.write(FormattedLine)

        BenchmarkResultsFile.write("\n")
        BenchmarkResultsFileRaw.close()

    BenchmarkResultsFile.close()


def GetBenchmarkResultsFileRawName(
    buildConfig: EnvironmentConfig.BuildConfig, BenchmarkName: str
) -> str:
    return (
        BenchmarkName
        + "_"
        + EnvironmentConfig.BuildConfig.ToCMakeBuildType(buildConfig)
        + "_Results.txt"
        + ".raw"
    )


def SortRawResultsFile(rawResultsFileName: str):
    with open(rawResultsFileName, "r") as original:
        data = original.readlines()
    data.sort()
    with open(rawResultsFileName, "w") as modified:
        modified.write("".join(data))


def PrependBenchmarkHeaderInRawResultsFile(header: str, rawResultsFileName: str):
    with open(rawResultsFileName, "r") as original:
        data = original.read()
    with open(rawResultsFileName, "w") as modified:
        modified.write(header + data)


def RunBenchmark(
    buildEnv: EnvironmentConfig.EnvironmentConfiguration,
    buildConfig: EnvironmentConfig.BuildConfig,
    BenchmarkList: List[str],
) -> List[str]:
    RawBenchmarkResultsFileNames = []
    BuildTypeString = EnvironmentConfig.BuildConfig.ToCMakeBuildType(buildConfig)
    targetPlatformString = EnvironmentConfig.Platform.PlatformToOsName(
        buildEnv.GetTargetPlatform()
    )

    BuildDirectory = Path(
        BenchmarkRootPath / "Build" / targetPlatformString / BuildTypeString
    )

    # Note that one epoch has a maximum of 32 iterations
    BenchmarkOptions = '"' + str(BenchmarkTestFilesPath) + '/"' + " " + "-e 2"

    for BenchmarkName in BenchmarkList:
        BenchmarkHeader = (
            "Running: benchmark {} with {} build on {}\n".format(
                BenchmarkName, BuildTypeString, targetPlatformString
            )
            + "-----------------------------------------"
            + "\n"
        )
        print("\n")
        print(BenchmarkHeader, end="")

        BenchmarkExecutablePath = Path(BuildDirectory / BenchmarkName)
        if targetPlatform == EnvironmentConfig.Platform.WINDOWS:
            BenchmarkExecutablePath = BenchmarkExecutablePath.with_suffix(".exe")

        BenchmarkExecutablePath.chmod(
            BenchmarkExecutablePath.stat().st_mode | stat.S_IEXEC
        )

        BenchmarkCommand = str(BenchmarkExecutablePath) + " " + BenchmarkOptions
        print("Command: {}".format(BenchmarkCommand))

        RawBenchmarkResultsFileNames.append(
            GetBenchmarkResultsFileRawName(buildConfig, BenchmarkName)
        )
        BenchmarkResultsFileRaw = open(RawBenchmarkResultsFileNames[-1], "w")

        subprocess.run(
            BenchmarkCommand,
            shell=True,
            check=True,
            stdout=BenchmarkResultsFileRaw,
        )

        SortRawResultsFile(RawBenchmarkResultsFileNames[-1])
        PrependBenchmarkHeaderInRawResultsFile(
            BenchmarkHeader, RawBenchmarkResultsFileNames[-1]
        )

        BenchmarkResultsFileRaw.close()

    return RawBenchmarkResultsFileNames


# =====================================================================================

BenchmarkList = [
    "BenchmarkDeflateInflateTextFile",
]

BenchmarkResultFileName = "BenchmarkResults.txt"

if os.path.exists(BenchmarkResultFileName):
    os.remove(BenchmarkResultFileName)

BuildEnv = EnvironmentConfig.EnvironmentConfiguration(
    RepositoryRootPath, targetPlatform
)

RawResultFiles = RunBenchmark(
    BuildEnv, EnvironmentConfig.BuildConfig.DEBUG, BenchmarkList
)
AppendBenchmarkResultToTotalResultFile(BenchmarkResultFileName, RawResultFiles)

RawResultFiles = RunBenchmark(
    BuildEnv, EnvironmentConfig.BuildConfig.RELEASE, BenchmarkList
)
AppendBenchmarkResultToTotalResultFile(BenchmarkResultFileName, RawResultFiles)
