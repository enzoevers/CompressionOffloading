add_library(Inflate_zlib STATIC
    src/Inflate_zlib.c)

target_include_directories(Inflate_zlib PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set(ZLIB_USE_STATIC_LIBS "ON")
find_package(ZLIB REQUIRED)
target_link_libraries(Inflate_zlib PRIVATE ZLIB::ZLIB)

set(Inflate_zlib_PUBLIC_INCLUDE_PATH ${CoDeLib_PUBLIC_INCLUDE_PATH}/Inflate_zlib)
set(Inflate_zlib_PUBLIC_HEADERS
    ${Inflate_zlib_PUBLIC_INCLUDE_PATH}/Inflate_zlib.h
)
install(FILES ${Inflate_zlib_PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/CoDeLib/Inflate_zlib)
