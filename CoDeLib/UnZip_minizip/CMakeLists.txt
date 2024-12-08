add_library(UnZip_minizip STATIC
    src/UnZip_minizip.c)

target_include_directories(UnZip_minizip PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

find_package(MINIZIP REQUIRED)
target_link_libraries(UnZip_minizip PRIVATE MINIZIP::minizip)

set(UnZip_minizip_PUBLIC_INCLUDE_PATH ${CoDeLib_PUBLIC_INCLUDE_PATH}/UnZip_minizip)
set(UnZip_minizip_PUBLIC_HEADERS
    ${UnZip_minizip_PUBLIC_INCLUDE_PATH}/UnZip_minizip.h
)
install(FILES ${UnZip_minizip_PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/CoDeLib/UnZip_minizip)
