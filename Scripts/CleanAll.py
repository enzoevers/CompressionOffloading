import os
from pathlib import Path
import shutil
import stat

CurrentScriptPath = Path(os.path.dirname(os.path.abspath(__file__)))
ProjectRootPath = Path(CurrentScriptPath.parent)

BenchmarkRootPath = Path(ProjectRootPath / "Benchmark")
BenchmarkBuildPath = Path(BenchmarkRootPath / "Build")

CoDeLibPath = Path(ProjectRootPath / "CoDeLib")
CoDeLibBuildPath = Path(CoDeLibPath / "Build")
CoDeLibInstallPath = Path(CoDeLibPath / "Install")

ExternalLibPath = Path(ProjectRootPath / "External")
ExternalZlibLibBuildPath = Path(ExternalLibPath / "zlib/Build")
ExternalZlibLibInstallPath = Path(ExternalLibPath / "zlib/Install")

DirectoriesToRemove = [
    BenchmarkBuildPath,
    CoDeLibBuildPath,
    CoDeLibInstallPath,
    ExternalZlibLibBuildPath,
    ExternalZlibLibInstallPath,
]


def on_rm_error(func, path, exc_info):
    # path contains the path of the file that couldn't be removed
    # let's just assume that it's read-only and unlink it.
    os.chmod(path, stat.S_IWRITE)
    os.unlink(path)


for directory in DirectoriesToRemove:
    if directory.exists():
        print(f"Removing directory: {directory}")
        shutil.rmtree(directory, onerror=on_rm_error)
