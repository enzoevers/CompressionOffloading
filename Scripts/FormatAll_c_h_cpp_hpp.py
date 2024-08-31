import os
from pathlib import Path
import subprocess
import glob

CurrentScriptPath = Path(os.path.dirname(os.path.abspath(__file__)))
ProjectRootPath = Path(CurrentScriptPath.parent)
TopLevelCMakeListsDirectory = Path(ProjectRootPath / "Benchmark")

FilesToFormat = list(TopLevelCMakeListsDirectory.glob("**/*.c"))
FilesToFormat += list(TopLevelCMakeListsDirectory.glob("**/*.h"))
FilesToFormat += list(TopLevelCMakeListsDirectory.glob("**/*.cpp"))
FilesToFormat += list(TopLevelCMakeListsDirectory.glob("**/*.hpp"))

print()
print("Formatting:")
for file in FilesToFormat:
    print("\t{}".format(file))
print()

ClangFormatCommand = "clang-format -i -style=file {}".format(
    " ".join([str(file) for file in FilesToFormat])
)
subprocess.run(
    ClangFormatCommand,
    shell=True,
    check=True,
)
