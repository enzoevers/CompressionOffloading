add_library(Zip_minizip STATIC
    src/Zip_minizip.c)

target_include_directories(Zip_minizip PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

find_package(MINIZIP REQUIRED)
target_link_libraries(Zip_minizip PRIVATE MINIZIP::minizip)

set(Zip_minizip_PUBLIC_INCLUDE_PATH ${CoDeLib_PUBLIC_INCLUDE_PATH}/Zip_minizip)
set(Zip_minizip_PUBLIC_HEADERS
    ${Zip_minizip_PUBLIC_INCLUDE_PATH}/Zip_minizip.h
)
install(FILES ${Zip_minizip_PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/CoDeLib/Zip_minizip)
