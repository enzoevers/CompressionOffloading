import os
from pathlib import Path
import subprocess
import glob

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

ClangFormatCommand = "clang-format --dry-run -Werror -style=file {}".format(
    " ".join([str(file) for file in FilesToFormat])
)
subprocess.run(
    ClangFormatCommand,
    shell=True,
    check=True,
)

print("All files are formatted correctly")
