cmake_minimum_required(VERSION 3.25)
include(FetchContent)

project(COMPRESSION_DECOMPRESSION_BENCHMARK C)

# b63
FetchContent_Declare(
  b63
  GIT_REPOSITORY https://github.com/enzoevers/b63.git
  GIT_TAG fbe75ced2ef6474b3088191ce80ac60cc9115a73
)
FetchContent_MakeAvailable(b63)

# Custom compressions libraries
find_package(CoDeLib REQUIRED)

add_executable(BenchmarkDeflateInflateTextFile BenchmarkDeflateInflateTextFile.c)

target_link_libraries(BenchmarkDeflateInflateTextFile PRIVATE m) # Math library required by b63
target_include_directories(BenchmarkDeflateInflateTextFile PRIVATE ${b63_SOURCE_DIR}/include)
target_include_directories(BenchmarkDeflateInflateTextFile PRIVATE ${Unity_SOURCE_DIR}/src)

target_link_libraries(BenchmarkDeflateInflateTextFile PRIVATE CoDeLib::CoDeLib)
target_link_libraries(BenchmarkDeflateInflateTextFile PRIVATE CoDeLib::Deflate_zlib)
target_link_libraries(BenchmarkDeflateInflateTextFile PRIVATE CoDeLib::Inflate_zlib)
target_link_libraries(BenchmarkDeflateInflateTextFile PRIVATE CoDeLib::RaiiString)
target_link_libraries(BenchmarkDeflateInflateTextFile PRIVATE CoDeLib::Utility)
