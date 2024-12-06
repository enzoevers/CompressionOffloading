import os
import stat
from pathlib import Path
import subprocess
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
BenchmarkTestFilesPath = Path(RepositoryRootPath / "Benchmark" / "Files")

os.chdir(RepositoryRootPath)


def AppendTestResultToTotalResultFile(
    testResultsFileName: str, testResultsFileRawNames: List[str]
):
    testResultsFile = open(testResultsFileName, "a")

    for testResultsFileRawName in testResultsFileRawNames:
        testResultsFileRaw = open(testResultsFileRawName, "r")
        testResultsFileRaw.seek(0)

        for line in testResultsFileRaw:
            testResultsFile.write(line)

        testResultsFile.write("\n")
        testResultsFileRaw.close()

    testResultsFile.close()


def GetTestResultsFileRawName(
    buildConfig: EnvironmentConfig.BuildConfig, testName: str
) -> str:
    return (
        "Test_"
        + testName
        + "_"
        + EnvironmentConfig.BuildConfig.ToCMakeBuildType(buildConfig)
        + "_Results.txt"
        + ".raw"
    )


def PrependTestHeaderInRawResultsFile(header: str, rawResultsFileName: str):
    with open(rawResultsFileName, "r") as original:
        data = original.read()
    with open(rawResultsFileName, "w") as modified:
        modified.write(header + data)


def RunTests(
    buildEnv: EnvironmentConfig.EnvironmentConfiguration,
    buildConfig: EnvironmentConfig.BuildConfig,
    testList: List[str],
) -> tuple[bool, List[str]]:
    RawTestResultsFileNames = []
    success = True

    BuildTypeString = EnvironmentConfig.BuildConfig.ToCMakeBuildType(buildConfig)
    targetPlatformString = EnvironmentConfig.Platform.PlatformToOsName(
        buildEnv.GetTargetPlatform()
    )

    CoDeLibBuildPath = Path(
        RepositoryRootPath
        / "CoDeLib"
        / "Build"
        / targetPlatformString
        / BuildTypeString
    )

    for testName in testList:
        testHeader = (
            "Running: test {} with {} build on {}\n".format(
                testName, BuildTypeString, targetPlatformString
            )
            + "-----------------------------------------"
            + "\n"
        )

        TestExecutablePath = Path(CoDeLibBuildPath / "Test" / testName)
        if targetPlatform == EnvironmentConfig.Platform.WINDOWS:
            TestExecutablePath = TestExecutablePath.with_suffix(".exe")

        TestExecutablePath.chmod(TestExecutablePath.stat().st_mode | stat.S_IEXEC)

        TestOptions = '"' + str(BenchmarkTestFilesPath) + '/"' + " -v"
        TestCommand = str(TestExecutablePath) + " " + TestOptions

        RawTestResultsFileNames.append(GetTestResultsFileRawName(buildConfig, testName))

        TestResultsFileRaw = open(RawTestResultsFileNames[-1], "w")

        result = subprocess.run(
            TestCommand,
            shell=True,
            check=False,
            stdout=TestResultsFileRaw,
            stderr=subprocess.STDOUT,
        )
        PrependTestHeaderInRawResultsFile(testHeader, RawTestResultsFileNames[-1])
        TestResultsFileRaw.close()

        if result.returncode != 0:
            print(testHeader)
            print("Failed\n")
            print("Called command:\n")
            print(TestCommand)
            print("\n")
            success = False

    return (success, RawTestResultsFileNames)


# =====================================================================================

TestList = [
    "CoDeLib_Test",
]

TestResultsFileName = "TestResults.txt"

if os.path.exists(TestResultsFileName):
    os.remove(TestResultsFileName)

BuildEnv = EnvironmentConfig.EnvironmentConfiguration(
    RepositoryRootPath, targetPlatform
)

[successDebug, RawResultFilesDebug] = RunTests(
    BuildEnv, EnvironmentConfig.BuildConfig.DEBUG, TestList
)
AppendTestResultToTotalResultFile(TestResultsFileName, RawResultFilesDebug)

[successRelease, RawResultFilesRelease] = RunTests(
    BuildEnv, EnvironmentConfig.BuildConfig.RELEASE, TestList
)
AppendTestResultToTotalResultFile(TestResultsFileName, RawResultFilesRelease)

# Print the content of BenchmarkResultFileName to the console
print("\n")
with open(TestResultsFileName, "r") as file:
    print(file.read())

if not successDebug or not successRelease:
    print("Test failed")
    exit(1)
