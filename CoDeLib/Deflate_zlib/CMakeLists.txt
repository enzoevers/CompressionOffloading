add_library(Deflate_zlib STATIC
    src/Deflate_zlib.c)

target_include_directories(Deflate_zlib PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set(ZLIB_USE_STATIC_LIBS "ON")
find_package(ZLIB REQUIRED)
target_link_libraries(Deflate_zlib PRIVATE ZLIB::ZLIB)

message(STATUS "CoDeLib_PUBLIC_INCLUDE_PATH: ${CoDeLib_PUBLIC_INCLUDE_PATH}")
set(Deflate_zlib_PUBLIC_INCLUDE_PATH ${CoDeLib_PUBLIC_INCLUDE_PATH}/Deflate_zlib)
set(Deflate_zlib_PUBLIC_HEADERS
    ${Deflate_zlib_PUBLIC_INCLUDE_PATH}/Deflate_zlib.h
)
install(FILES ${Deflate_zlib_PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/CoDeLib/Deflate_zlib)
