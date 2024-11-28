cmake_minimum_required(VERSION 3.28)
project(COMPRESSION_DECOMPRESSION_LIB VERSION 0.0.1 LANGUAGES C)

include(FetchContent)
include(GNUInstallDirs)

# CoDeLib stands for CompressionDecompressionLib
file(WRITE ${CMAKE_BINARY_DIR}/empty.c "")
add_library(CoDeLib STATIC ${CMAKE_BINARY_DIR}/empty.c)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

include_directories(
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set(CoDeLib_PUBLIC_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include/CoDeLib)
set(COMMON_HEADERS
    ${CoDeLib_PUBLIC_INCLUDE_PATH}/IDeflate.h
    ${CoDeLib_PUBLIC_INCLUDE_PATH}/IInflate.h
    ${CoDeLib_PUBLIC_INCLUDE_PATH}/IUnZip.h
)
install(FILES ${COMMON_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/CoDeLib)

add_subdirectory(Deflate_zlib)
add_subdirectory(Inflate_zlib)
add_subdirectory(FileUtils)
add_subdirectory(UnZip_minizip)
add_subdirectory(RaiiString)
add_subdirectory(ZipContentInfo)
add_subdirectory(Test)

install(
    TARGETS CoDeLib Deflate_zlib Inflate_zlib UnZip_minizip RaiiString ZipContentInfo FileUtils
    EXPORT CoDeLibTargets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
    EXPORT CoDeLibTargets
    FILE CoDeLibTargets.cmake
    NAMESPACE CoDeLib::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/CoDeLib
)

include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/CoDeLibConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/CoDeLib
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/CoDeLibConfig.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/CoDeLib
)
