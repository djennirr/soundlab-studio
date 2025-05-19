from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout


class MainProjectConan(ConanFile):
    name = "main_project"
    version = "0.1"

    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    requires = [
        "sdl/2.28.3",
        "glfw/3.4",
        "opengl/system"
    ]

    default_options = {
    "sdl/*:shared": True,
    }

    def layout(self):
        cmake_layout(self)

    def generate(self):
        pass  

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()