import os
from pathlib import Path
import subprocess
import shutil
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


##############################
# zlib
##############################
def BuildAndInstallZlib(
    buildEnv: EnvironmentConfig.EnvironmentConfiguration,
    buildConfig: EnvironmentConfig.BuildConfig = EnvironmentConfig.BuildConfig.DEBUG,
):
    ProjectName = "zlib"

    BuildTypeString = EnvironmentConfig.BuildConfig.ToCMakeBuildType(buildConfig)
    targetPlatformString = EnvironmentConfig.Platform.PlatformToOsName(
        buildEnv.GetTargetPlatform()
    )

    TopLevelCMakeListsDirectory = Path(ExternalLibPath / ProjectName)
    BuildDirectory = Path(
        ExternalLibPath / ProjectName / "Build" / targetPlatformString / BuildTypeString
    )
    InstallDirectory = Path(
        ExternalLibPath
        / ProjectName
        / "Install"
        / targetPlatformString
        / BuildTypeString
    )

    if not BuildDirectory.exists():
        BuildDirectory.mkdir(parents=True)

    if InstallDirectory.exists():
        shutil.rmtree(InstallDirectory)
        InstallDirectory.mkdir(parents=True)

    os.chdir(RepositoryRootPath)

    print("==============================")
    print(ProjectName + ": Configuring ({})".format(BuildTypeString))
    print("==============================")
    configureCommand = 'cmake -G "{0}" -DCMAKE_TOOLCHAIN_FILE="{1}" -S {2} -B {3} -DCMAKE_INSTALL_PREFIX="{4}" -DZLIB_BUILD_EXAMPLES=OFF -DCMAKE_BUILD_TYPE={5}'.format(
        buildEnv.GetCmakeGenerator(),
        buildEnv.GetCustomToolChainPath(),
        TopLevelCMakeListsDirectory,
        BuildDirectory,
        InstallDirectory,
        BuildTypeString,
    )
    print(configureCommand)
    subprocess.run(
        configureCommand,
        shell=True,
        check=True,
    )

    print("==============================")
    print(ProjectName + ": Building ({})".format(BuildTypeString))
    print("==============================")
    buildCommand = "cmake --build {0} -- -j 4".format(BuildDirectory)
    print(buildCommand)
    subprocess.run(
        buildCommand,
        shell=True,
        check=True,
    )

    print("==============================")
    print(ProjectName + ": Installing ({})".format(BuildTypeString))
    print("==============================")
    installCommand = "cmake --install {0}".format(BuildDirectory)
    print(installCommand)
    subprocess.run(
        installCommand,
        shell=True,
        check=True,
    )


##############################
# minizip-ng
##############################
def BuildAndInstallMinizipNg(
    buildEnv: EnvironmentConfig.EnvironmentConfiguration,
    buildConfig: EnvironmentConfig.BuildConfig = EnvironmentConfig.BuildConfig.DEBUG,
):
    ProjectName = "minizip-ng"

    BuildTypeString = EnvironmentConfig.BuildConfig.ToCMakeBuildType(buildConfig)
    targetPlatformString = EnvironmentConfig.Platform.PlatformToOsName(
        buildEnv.GetTargetPlatform()
    )

    TopLevelCMakeListsDirectory = Path(ExternalLibPath / ProjectName)
    BuildDirectory = Path(
        ExternalLibPath / ProjectName / "Build" / targetPlatformString / BuildTypeString
    )
    InstallDirectory = Path(
        ExternalLibPath
        / ProjectName
        / "Install"
        / targetPlatformString
        / BuildTypeString
    )

    if not BuildDirectory.exists():
        BuildDirectory.mkdir(parents=True)

    if InstallDirectory.exists():
        shutil.rmtree(InstallDirectory)
        InstallDirectory.mkdir(parents=True)

    os.chdir(RepositoryRootPath)

    # All options for minizip-ng that are not needed are turned off (all off them) except for MZ_COMPAT
    # In this project minizing-ng is used only for zip file creation and extraction
    print("==============================")
    print(ProjectName + ": Configuring ({})".format(BuildTypeString))
    print("==============================")
    configureCommand = 'cmake -G "{0}" -DCMAKE_TOOLCHAIN_FILE="{1}" -S {2} -B {3} -DCMAKE_INSTALL_PREFIX="{4}" -DCMAKE_BUILD_TYPE={5} -DBUILD_SHARED_LIBS=OFF -DMZ_COMPAT=ON -DMZ_ZLIB=OFF -DMZ_BZIP2=OFF -DMZ_LZMA=OFF -DMZ_ZSTD=OFF -DMZ_LIBCOMP=OFF -DMZ_FETCH_LIBS=OFF -DMZ_PKCRYPT=OFF -DMZ_WZAES=OFF -DMZ_OPENSSL=OFF -DMZ_LIBBSD=OFF -DMZ_ICONV=OFF -DUSE_FILE_OFFSET_BITS64=ON'.format(
        buildEnv.GetCmakeGenerator(),
        buildEnv.GetCustomToolChainPath(),
        TopLevelCMakeListsDirectory,
        BuildDirectory,
        InstallDirectory,
        BuildTypeString,
    )
    print(configureCommand)
    subprocess.run(
        configureCommand,
        shell=True,
        check=True,
    )

    print("==============================")
    print(ProjectName + ": Building ({})".format(BuildTypeString))
    print("==============================")
    buildCommand = "cmake --build {0}".format(BuildDirectory)
    print(buildCommand)
    subprocess.run(
        buildCommand,
        shell=True,
        check=True,
    )

    print("==============================")
    print(ProjectName + ": Installing ({})".format(BuildTypeString))
    print("==============================")
    installCommand = "cmake --install {0}".format(BuildDirectory)
    print(installCommand)
    subprocess.run(
        installCommand,
        shell=True,
        check=True,
    )


BuildEnv = EnvironmentConfig.EnvironmentConfiguration(
    RepositoryRootPath, targetPlatform
)

# zlib
BuildAndInstallZlib(BuildEnv, EnvironmentConfig.BuildConfig.DEBUG)
BuildAndInstallZlib(BuildEnv, EnvironmentConfig.BuildConfig.RELEASE)

# minizip-ng
BuildAndInstallMinizipNg(BuildEnv, EnvironmentConfig.BuildConfig.DEBUG)
BuildAndInstallMinizipNg(BuildEnv, EnvironmentConfig.BuildConfig.RELEASE)
