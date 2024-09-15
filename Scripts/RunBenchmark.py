import os
from pathlib import Path
import subprocess
import re
import fileinput

CurrentScriptPath = Path(os.path.dirname(os.path.abspath(__file__)))
RepositoryRootPath = Path(CurrentScriptPath.parent)
BenchmarkRootPath = Path(RepositoryRootPath / "Benchmark")
BuildDirectory = Path(BenchmarkRootPath / "Build")

os.chdir(RepositoryRootPath)

BenchmarkExecutable = Path(BuildDirectory / "Benchmark")
BenchmarkOptions = "-i -e 5"
BenchmarkCommand = str(BenchmarkExecutable) + " " + BenchmarkOptions

print()
print('Running "{}"'.format(BenchmarkCommand))
print()

BenchmarkResultsFileRaw = open("BenchmarkResults.txt.raw", "w+")
subprocess.run(BenchmarkCommand, shell=True, check=True, stdout=BenchmarkResultsFileRaw)


def escape_ansi(line):
    ansi_escape = re.compile(r"\x1b[^m]*m")
    return ansi_escape.sub("", line)


BenchmarkResultsFile = open("BenchmarkResults.txt", "w")
BenchmarkResultsFileRaw.seek(0)
for line in BenchmarkResultsFileRaw:
    FormattedLine = escape_ansi(line)
    BenchmarkResultsFile.write(FormattedLine)

BenchmarkResultsFileRaw.close()
BenchmarkResultsFile.close()
