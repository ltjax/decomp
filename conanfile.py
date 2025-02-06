from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout
from conan.tools.microsoft.visual import is_msvc

class DecompConan(ConanFile):
    name = "decomp"
    version = "1.2"
    license = "MIT"
    author = "Marius Elvert marius.elvert@googlemail.com"
    url = "https://github.com/ltjax/decomp"
    description = "Triangulation and convex decomposition of polygonal meshes"
    topics = ("polygon", "decomposition")
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False]}
    default_options = {
        "shared": False,
        "fPIC": True}
    exports_sources = "source/*", "test/*", "demo/*", "include/*", "CMakeLists.txt",
    test_requires = "catch2/2.13.10",

    def config_options(self):
        if is_msvc(self):
            del self.options.fPIC

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()

        toolchain = CMakeToolchain(self)
        toolchain.variables['decomp_USE_CONAN'] = True
        if not is_msvc(self):
           toolchain.variables['decomp_PIC'] = self.options.fPIC
        toolchain.generate()

    def _configured_cmake(self):
        cmake.configure(source_folder=".", defs={
            'decomp_USE_CONAN': True,
        })
        return cmake

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()
    
    def layout(self):
        cmake_layout(self)

    def package_info(self):
        self.cpp_info.libs = ["decomp"]
