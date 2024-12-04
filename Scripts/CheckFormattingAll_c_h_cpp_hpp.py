import os
from pathlib import Path
import subprocess
import glob
import argparse


parser = argparse.ArgumentParser(
    description="Check and format all c, h, cpp, hpp files"
)

formatInPlaceOptions = ["true", "false"]

parser.add_argument(
    "--FormatInPlace",
    action="store",
    dest="formatInPlace",
    choices=formatInPlaceOptions,
    default="false",
    help="Selects if the files should be formatted in place or if the formatting should be checked only",
)

# TODO: This method doesn't seem very safe due to the option to execute any executable that starts with 'clang-format'.
#       Better argument checking should be implemented.
parser.add_argument(
    "--ClangFormatVersion",
    action="store",
    dest="clangFormatVersion",
    default="",
    help="Clang format version to use. The provided version is appended after the clang-format command with and must start with a '-'. For example: --ClangFormatVersion=-18",
)

args = parser.parse_args()


CurrentScriptPath = Path(os.path.dirname(os.path.abspath(__file__)))
RepositoryRootPath = Path(CurrentScriptPath.parent)
BenchmarkRootPath = Path(RepositoryRootPath / "Benchmark")
CoDeLibRootPath = Path(RepositoryRootPath / "CoDeLib")

ProjectPathsToCheck = [BenchmarkRootPath, CoDeLibRootPath]
FilesToFormat = list()

for ProjectPath in ProjectPathsToCheck:
    patterns = ["**/*.c", "**/*.h", "**/*.cpp", "**/*.hpp"]

    for pattern in patterns:
        for file in ProjectPath.glob(pattern):
            if "Build" not in file.parts and "Install" not in file.parts:
                FilesToFormat.append(file)

print()
print("Checking if formatted:")
for file in FilesToFormat:
    print("\t{}".format(file))
print()

clangFormatFormattingOptions: str = ""
if args.formatInPlace == "true":
    clangFormatFormattingOptions = "-i"
    print("Formatting files in place")
else:
    clangFormatFormattingOptions = "--dry-run -Werror"
    print("Checking formatting only")

ClangFormatCommand = "clang-format{0} {1} -style=file {2}".format(
    args.clangFormatVersion,
    clangFormatFormattingOptions,
    " ".join([str(file) for file in FilesToFormat]),
)
subprocess.run(
    ClangFormatCommand,
    shell=True,
    check=True,
)

print("All files are formatted correctly")
