# SPDX-License-Identifier: MIT
# Self-hosted vcpkg registry port for Stellar. The SHA512 feb7fddfb84e4da8cc1e68a879e197b836e04daf466c00c42d96e73d4bce76015bbecc6c7205f3f85c6e0af2b9a324f5e9ddd57410b5fdf8eca8edf833a37b19nd version are
# maintained automatically by .github/workflows/publish-vcpkg.yml.
set(VCPKG_BUILD_TYPE release)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO        stescobedo92/stellar
    REF         "v${VERSION}"
    SHA512      feb7fddfb84e4da8cc1e68a879e197b836e04daf466c00c42d96e73d4bce76015bbecc6c7205f3f85c6e0af2b9a324f5e9ddd57410b5fdf8eca8edf833a37b19
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

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
