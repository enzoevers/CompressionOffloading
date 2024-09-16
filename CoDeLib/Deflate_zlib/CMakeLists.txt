add_library(Deflate_zlib STATIC
    src/Deflate_zlib.c)

target_include_directories(Deflate_zlib PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_include_directories(Deflate_zlib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(ZLIB_USE_STATIC_LIBS "ON")
find_package(ZLIB REQUIRED)
target_link_libraries(Deflate_zlib PRIVATE ZLIB::ZLIB)
