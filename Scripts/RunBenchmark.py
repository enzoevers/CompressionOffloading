import os
from pathlib import Path
import subprocess
import re
import fileinput

CurrentScriptPath = Path(os.path.dirname(os.path.abspath(__file__)))
RepositoryRootPath = Path(CurrentScriptPath.parent)
BenchmarkRootPath = Path(RepositoryRootPath / "Benchmark")
BenchmarkTestFilesPath = Path(BenchmarkRootPath / "Files")
BuildDirectory = Path(BenchmarkRootPath / "Build")

os.chdir(RepositoryRootPath)


def escape_ansi(line):
    ansi_escape = re.compile(r"\x1b[^m]*m")
    return ansi_escape.sub("", line)


BenchmarkList = [
    "BenchmarkCompressTextFile",
]

BenchmarkResultsFileName = "BenchmarkResults.txt"

if os.path.exists(BenchmarkResultsFileName):
    os.remove(BenchmarkResultsFileName)

BenchmarkResultsFile = open(BenchmarkResultsFileName, "a")

for BenchmarkName in BenchmarkList:
    BenchmarkHeader = (
        "\n"
        + "Running: {}\n".format(BenchmarkName)
        + "-----------------------------------------"
        + "\n"
    )
    print(BenchmarkHeader, end="")

    BenchmarkExecutablePath = Path(BuildDirectory / BenchmarkName)
    # Note that one epoch has a maximum of 32 iterations
    BenchmarkOptions = '"' + str(BenchmarkTestFilesPath) + '/"' + " -i -e 2"
    BenchmarkCommand = str(BenchmarkExecutablePath) + " " + BenchmarkOptions

    print("Command: {}".format(BenchmarkCommand))

    BenchmarkResultsFileRawName = BenchmarkName + "_Results.txt" + ".raw"

    if os.path.exists(BenchmarkResultsFileRawName):
        os.remove(BenchmarkResultsFileRawName)

    BenchmarkResultsFileRaw = open(BenchmarkResultsFileRawName, "w+")
    subprocess.run(
        BenchmarkCommand,
        shell=True,
        check=True,
        stdout=BenchmarkResultsFileRaw,
    )

    BenchmarkResultsFile.write(BenchmarkHeader)
    BenchmarkResultsFileRaw.seek(0)
    for line in BenchmarkResultsFileRaw:
        FormattedLine = escape_ansi(line)
        print(FormattedLine, end="")
        BenchmarkResultsFile.write(FormattedLine)

    BenchmarkResultsFileRaw.close()

BenchmarkResultsFile.close()
