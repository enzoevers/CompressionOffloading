import os
from pathlib import Path
import EnvironmentConfig
import subprocess

hostPlatform = EnvironmentConfig.Platform.OsNameToPlatform(os.name)

currentScriptPath = Path(os.path.dirname(os.path.abspath(__file__)))
repositoryRootPath = Path(currentScriptPath.parent)

venvPath = Path(repositoryRootPath / ".venv")
if not venvPath.exists():
    print("Creating virtual environment at {}".format(venvPath))
    subprocess.run(
        "python -m venv {}".format(venvPath),
        shell=True,
        check=True,
    )
else:
    print("Virtual environment already exists at {}".format(venvPath))

print("Activating virtual environment...")

VirtualEnvPythonPath = Path(
    venvPath
    / ("Scripts" if hostPlatform == EnvironmentConfig.Platform.WINDOWS else "bin")
    / "python"
)


print("Upgrading pip...")
pipUpgradeCommand = "{} -m pip install --upgrade pip".format(VirtualEnvPythonPath)
print(pipUpgradeCommand)
subprocess.run(
    pipUpgradeCommand,
    shell=True,
    check=True,
)

requirementsFilePath = Path(repositoryRootPath / "requirements.txt")
if requirementsFilePath.exists():
    print("Installing required packages from {}".format(requirementsFilePath))
    installCommand = "{} -m pip install -r {}".format(
        VirtualEnvPythonPath, requirementsFilePath
    )
    print(installCommand)
    subprocess.run(
        installCommand,
        shell=True,
        check=True,
    )
else:
    print(
        "No requirements.txt file found at {}, skipping package installation.".format(
            requirementsFilePath
        )
    )

print("Virtual environment setup complete.")

VirtualEnvConanPath = Path(
    venvPath
    / ("Scripts" if hostPlatform == EnvironmentConfig.Platform.WINDOWS else "bin")
    / "conan"
)

print("Getting conan profile...")
conanProfileCommand = "{} profile detect --force".format(VirtualEnvConanPath)
print(conanProfileCommand)
subprocess.run(
    conanProfileCommand,
    shell=True,
    check=True,
)

print("Getting Conan dependencies...")
buildTypes = ["Debug", "Release"]
profiles = ["Windows_x86_64", "Linux_x86_64"]
for profile in profiles:
    if (
        hostPlatform == EnvironmentConfig.Platform.WINDOWS
        and profile.startswith("Linux")
    ) or (
        hostPlatform == EnvironmentConfig.Platform.LINUX
        and profile.startswith("Windows")
    ):
        continue
    for buildType in buildTypes:
        print(
            "Installing Conan dependencies for build type '{}' and profile '{}'...".format(
                buildType, profile
            )
        )
        conanInstallCommand = '{0} install "{1}" --profile:host={2} --profile:build={3} --output-folder="{4}" --build=missing --settings=build_type={5}'.format(
            VirtualEnvConanPath,
            Path(repositoryRootPath / "conanfile.py"),
            Path(repositoryRootPath / "ConanProfiles" / profile),
            Path(repositoryRootPath / "ConanProfiles" / profile),
            Path(repositoryRootPath / "build" / "conan"),
            buildType,
        )
        print(conanInstallCommand)
        subprocess.run(
            conanInstallCommand,
            shell=True,
            check=True,
        )
