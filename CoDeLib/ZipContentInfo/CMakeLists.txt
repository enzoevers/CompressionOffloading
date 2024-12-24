
add_library(ZipContentInfo STATIC
    src/ZipContentInfo.c)

target_include_directories(ZipContentInfo PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)