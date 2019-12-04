from conans import ConanFile, CMake, tools


class DecompConan(ConanFile):
    name = "decomp"
    version = "1.0"
    license = "MIT"
    author = "Marius Elvert marius.elvert@googlemail.com"
    url = "https://github.com/ltjax/decomp"
    description = "Triangulation and convex decomposition of polygonal meshes"
    topics = ("polygon", "decomposition")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = "shared=False"
    generators = "cmake"
    exports_sources = "source/*", "test/*", "include/*", "CMakeLists.txt",
    build_requires = "Catch2/2.7.2@catchorg/stable",

    def _configured_cmake(self):
        cmake = CMake(self)
        cmake.configure(source_folder=".", defs={
            'decomp_USE_CONAN': True
        })
        return cmake

    def build(self):
        cmake = self._configured_cmake()
        cmake.build()

    def package(self):
        self._configured_cmake().install()

    def package_info(self):
        self.cpp_info.libs = ["decomp"]
