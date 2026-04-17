add_executable(stellar_bench
    bench_strings.cpp
    bench_collections.cpp
)

target_link_libraries(stellar_bench PRIVATE
    stellar::stellar
    benchmark::benchmark
    benchmark::benchmark_main
)

if(MSVC)
    target_compile_options(stellar_bench PRIVATE /O2 /bigobj)
else()
    target_compile_options(stellar_bench PRIVATE -O3)
endif()
