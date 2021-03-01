from conans import ConanFile, CMake, tools


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
    generators = "cmake"
    exports_sources = "source/*", "test/*", "demo/*", "include/*", "CMakeLists.txt",
    build_requires = "catch2/2.13.4",

    def configure(self):
        if self.settings.compiler == 'Visual Studio':
            del self.options.fPIC

    def _configured_cmake(self):
        cmake = CMake(self)
        cmake.configure(source_folder=".", defs={
            'decomp_USE_CONAN': True,
        })
        if self.settings.compiler != 'Visual Studio':
            cmake.definitions['decomp_PIC'] = self.options.fPIC
        return cmake

    def build(self):
        cmake = self._configured_cmake()
        cmake.build()

    def package(self):
        self._configured_cmake().install()

    def package_info(self):
        self.cpp_info.libs = ["decomp"]
