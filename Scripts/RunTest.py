import os
from pathlib import Path
import subprocess

CurrentScriptPath = Path(os.path.dirname(os.path.abspath(__file__)))
RepositoryRootPath = Path(CurrentScriptPath.parent)
BenchmarkRootPath = Path(RepositoryRootPath / "Benchmark")
BenchmarkTestFilesPath = Path(BenchmarkRootPath / "Files")
CoDeLibRootPath = Path(RepositoryRootPath / "CoDeLib")
BuildDirectory = Path(CoDeLibRootPath / "Build")
TestName = "CoDeLib_Test"

os.chdir(RepositoryRootPath)

TestResultsFileName = "TestResults.txt"

if os.path.exists(TestResultsFileName):
    os.remove(TestResultsFileName)

TestHeader = (
    "\n" + "Running tests\n" + "-----------------------------------------" + "\n"
)
print(TestHeader, end="")

TestExecutablePath = Path(BuildDirectory / "Test" / TestName)
TestOptions = '"' + str(BenchmarkTestFilesPath) + '/"'
TestCommand = str(TestExecutablePath) + " " + TestOptions

print("Command: {}".format(TestCommand))

TestResultsFileRawName = "Test_" + TestName + "_Results.txt" + ".raw"
TestResultsFileRaw = open(TestResultsFileRawName, "w+")
subprocess.run(
    TestCommand,
    shell=True,
    check=True,
    stdout=TestResultsFileRaw,
)


TestResultsFile = open(TestResultsFileName, "w+")
TestResultsFile.write(TestHeader)

TestResultsFileRaw.seek(0)
for line in TestResultsFileRaw:
    print(line, end="")
    TestResultsFile.write(line)


TestResultsFileRaw.close()
TestResultsFile.close()
