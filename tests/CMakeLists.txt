include(GoogleTest)

add_executable(stellar_tests
    test_strings.cpp
    test_collections.cpp
    test_string_builder.cpp
    test_async.cpp
)

target_link_libraries(stellar_tests PRIVATE
    stellar::stellar
    GTest::gtest_main
)

if(MSVC)
    target_compile_options(stellar_tests PRIVATE /bigobj)
endif()

gtest_discover_tests(stellar_tests DISCOVERY_TIMEOUT 30)
