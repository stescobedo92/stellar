# SPDX-License-Identifier: MIT
# Header-only port — no libs are built, only headers + CMake config are installed.
set(VCPKG_BUILD_TYPE release)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO        stescobedo92/stellar
    REF         "v${VERSION}"
    SHA512      556623b4cd6206efa548d268a2d3b2638e88ff12db9c4eb375482ee2a0eaf1bd50e429bb5639b7228b549eb1b32f0199f99d20cf56d291e8ef792b8093a0249d  # filled in by publish-vcpkg workflow before submission
    HEAD_REF    master
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        -DSTELLAR_BUILD_TESTS=OFF
        -DSTELLAR_BUILD_BENCHMARKS=OFF
        -DSTELLAR_BUILD_EXAMPLES=OFF
)

vcpkg_cmake_install()
vcpkg_cmake_config_fixup(PACKAGE_NAME stellar CONFIG_PATH lib/cmake/stellar)

file(REMOVE_RECURSE
    "${CURRENT_PACKAGES_DIR}/debug"
    "${CURRENT_PACKAGES_DIR}/lib"
)

file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/usage"
     DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
