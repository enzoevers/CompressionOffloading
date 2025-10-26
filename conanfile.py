from conan import ConanFile
from conan.tools.cmake import cmake_layout


class ExampleRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        self.requires("zlib/1.3.1")

    def configure(self):
        self.options["zlib/*"].shared = False

    def layout(self):
        cmake_layout(self)
