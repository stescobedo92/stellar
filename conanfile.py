# SPDX-License-Identifier: MIT
"""Conan 2.x recipe for stellar — header-only C++23 library."""

import os

from conan import ConanFile
from conan.errors import ConanInvalidConfiguration
from conan.tools.build import check_min_cppstd
from conan.tools.cmake import CMake, CMakeToolchain, cmake_layout
from conan.tools.files import copy, get
from conan.tools.scm import Version

required_conan_version = ">=2.0.0"


class StellarConan(ConanFile):
    name = "stellar"
    version = "0.1.0"
    description = (
        "Header-only C++23 toolkit extensions (ste::) — string, collection, "
        "async and StringBuilder utilities."
    )
    license = "MIT"
    url = "https://github.com/stescobedo92/stellar"
    homepage = "https://github.com/stescobedo92/stellar"
    topics = ("cpp23", "cxx23", "string", "utilities", "header-only",
              "coroutines", "ranges")

    package_type = "header-library"
    settings = "os", "arch", "compiler", "build_type"
    no_copy_source = True

    # Files that are packaged into the exported recipe. When published to
    # conan-center-index, only the recipe is committed; the source is fetched
    # via conan.tools.files.get() at build time. For local `conan create`,
    # exports_sources is used instead.
    exports_sources = (
        "CMakeLists.txt",
        "cmake/*",
        "include/*",
        "LICENSE",
        "README.md",
    )

    @property
    def _min_cppstd(self):
        return 23

    @property
    def _compilers_minimum_version(self):
        return {
            "gcc":            "13",
            "clang":          "17",
            "apple-clang":    "15",
            "Visual Studio":  "17",
            "msvc":           "193",
        }

    def validate(self):
        if self.settings.compiler.get_safe("cppstd"):
            check_min_cppstd(self, self._min_cppstd)
        minimum_version = self._compilers_minimum_version.get(str(self.settings.compiler))
        if minimum_version and Version(self.settings.compiler.version) < minimum_version:
            raise ConanInvalidConfiguration(
                f"{self.ref} requires C++{self._min_cppstd}. "
                f"{self.settings.compiler} {self.settings.compiler.version} is not supported."
            )

    def package_id(self):
        # Header-only package: binaries are identical across all settings.
        self.info.clear()

    def layout(self):
        cmake_layout(self, src_folder=".")

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["STELLAR_BUILD_TESTS"]      = False
        tc.variables["STELLAR_BUILD_BENCHMARKS"] = False
        tc.variables["STELLAR_BUILD_EXAMPLES"]   = False
        tc.generate()

    def build(self):
        # header-only: nothing to compile
        pass

    def package(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.install()
        copy(self, "LICENSE", self.source_folder,
             os.path.join(self.package_folder, "licenses"))

    def package_info(self):
        self.cpp_info.set_property("cmake_file_name",   "stellar")
        self.cpp_info.set_property("cmake_target_name", "stellar::stellar")
        self.cpp_info.set_property("pkg_config_name",   "stellar")

        # Header-only: no libs, no lib dirs, no bin dirs.
        self.cpp_info.bindirs = []
        self.cpp_info.libdirs = []

        # C++23 requirement propagated to consumers.
        self.cpp_info.cppstd = "23"
