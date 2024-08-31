import os
from pathlib import Path
import subprocess

CurrentScriptPath = Path(os.path.dirname(os.path.abspath(__file__)))
ProjectRootPath = Path(CurrentScriptPath.parent)
TopLevelCMakeListsDirectory = Path(ProjectRootPath / "Benchmark")
BuildDirectory = Path(ProjectRootPath / "Build")

os.chdir(ProjectRootPath)

BenchmarkExecutable = Path(BuildDirectory / "Benchmark")

print()
print('Running "{}"'.format(BenchmarkExecutable))
print()

subprocess.run(str(BenchmarkExecutable), shell=True, check=True)
