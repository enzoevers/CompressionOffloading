cmake_minimum_required(VERSION 3.25)
include(FetchContent)

project(COMPRESSION_DECOMPRESSION_BENCHMARK C)
add_executable(Benchmark main.c)

# b63
FetchContent_Declare(
  b63
  GIT_REPOSITORY https://github.com/enzoevers/b63.git
  GIT_TAG fbe75ced2ef6474b3088191ce80ac60cc9115a73
)
FetchContent_MakeAvailable(b63)

target_link_libraries(Benchmark PRIVATE m) # Math library required by b63
target_include_directories(Benchmark PRIVATE ${b63_SOURCE_DIR}/include)

# Custom compressions libraries
find_package(CoDeLib REQUIRED)
target_link_libraries(Benchmark PRIVATE CoDeLib::CoDeLib)
target_link_libraries(Benchmark PRIVATE CoDeLib::Deflate_zlib)
