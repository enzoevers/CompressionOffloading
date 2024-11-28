import os
from pathlib import Path
import subprocess
import EnvironmentConfig
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
ExternalLibPath = Path(RepositoryRootPath / "External")
CoDeLibPath = Path(RepositoryRootPath / "CoDeLib")


os.chdir(RepositoryRootPath)


def BuildBenchmark(
    buildEnv: EnvironmentConfig.EnvironmentConfiguration,
    buildConfig: EnvironmentConfig.BuildConfig = EnvironmentConfig.BuildConfig.DEBUG,
):
    ProjectName = "Benchmark"

    BuildTypeString = EnvironmentConfig.BuildConfig.ToCMakeBuildType(buildConfig)
    targetPlatformString = EnvironmentConfig.Platform.PlatformToOsName(
        buildEnv.GetTargetPlatform()
    )

    CoDeLibInstallDirectory = Path(
        CoDeLibPath / "Install" / targetPlatformString / BuildTypeString
    )
    ExternalZlibLibInstallPath = Path(
        ExternalLibPath / "zlib/Install" / targetPlatformString / BuildTypeString
    )
    ExternalMinizipNgLibInstallPath = Path(
        ExternalLibPath / "minizip-ng/Install" / targetPlatformString / BuildTypeString
    )

    BenchmarkRootPath = Path(RepositoryRootPath / ProjectName)
    BuildDirectory = Path(
        BenchmarkRootPath / "Build" / targetPlatformString / BuildTypeString
    )

    if not BuildDirectory.exists():
        BuildDirectory.mkdir(parents=True)

    os.chdir(RepositoryRootPath)

    print("==============================")
    print(ProjectName + ": Configuring ({})".format(BuildTypeString))
    print("==============================")
    configureCommand = 'cmake -G "{0}" -DCMAKE_TOOLCHAIN_FILE="{1}" -S {2} -B {3} -DCMAKE_BUILD_TYPE={4} -DZLIB_ROOT="{5}" -DCMAKE_PREFIX_PATH="{6};{7}"'.format(
        buildEnv.GetCmakeGenerator(),
        buildEnv.GetCustomToolChainPath(),
        BenchmarkRootPath,
        BuildDirectory,
        BuildTypeString,
        ExternalZlibLibInstallPath,
        CoDeLibInstallDirectory,
        ExternalMinizipNgLibInstallPath,
    )
    subprocess.run(
        configureCommand,
        shell=True,
        check=True,
    )

    print("==============================")
    print(ProjectName + ": Building ({})".format(BuildTypeString))
    print("==============================")
    buildCommand = "cmake --build {0} -- -j 4".format(BuildDirectory)
    subprocess.run(
        buildCommand,
        shell=True,
        check=True,
    )


BuildEnv = EnvironmentConfig.EnvironmentConfiguration(
    RepositoryRootPath, targetPlatform
)
BuildBenchmark(BuildEnv, EnvironmentConfig.BuildConfig.DEBUG)
BuildBenchmark(BuildEnv, EnvironmentConfig.BuildConfig.RELEASE)
