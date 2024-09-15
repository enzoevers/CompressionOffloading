cmake_minimum_required(VERSION 3.25)
project(COMPRESSION_DECOMPRESSION_LIB VERSION 0.0.1 LANGUAGES C)

include(GNUInstallDirs)

# CoDeLib stands for CompressionDecompressionLib
file(WRITE ${CMAKE_BINARY_DIR}/empty.c "")
add_library(CoDeLib STATIC ${CMAKE_BINARY_DIR}/empty.c)

include_directories(
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

add_subdirectory(Deflate_zlib)
#add_subdirectory(INFLATE_zlib)

set(CoDeLib_PUBLIC_INCLUDE_PATH include/CoDeLib)
set(COMMON_HEADERS
    ${CoDeLib_PUBLIC_INCLUDE_PATH}/IDeflate.h
)
install(FILES ${COMMON_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/CoDeLib)

set(Deflate_zlib_PUBLIC_INCLUDE_PATH ${CoDeLib_PUBLIC_INCLUDE_PATH}/Deflate_zlib)
set(Deflate_zlib_PUBLIC_HEADERS
    ${Deflate_zlib_PUBLIC_INCLUDE_PATH}/Deflate_zlib.h
)
install(FILES ${Deflate_zlib_PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/CoDeLib/Deflate_zlib)

install(
    TARGETS CoDeLib Deflate_zlib
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
