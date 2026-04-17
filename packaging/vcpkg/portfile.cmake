# SPDX-License-Identifier: MIT
# Header-only port — no libs are built, only headers + CMake config are installed.
set(VCPKG_BUILD_TYPE release)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO        stescobedo92/stellar
    REF         "v${VERSION}"
    SHA512      0  # filled in by publish-vcpkg workflow before submission
    HEAD_REF    main
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
