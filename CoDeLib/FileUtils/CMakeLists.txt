add_library(FileUtils STATIC
    src/FileUtils.c
)

target_include_directories(FileUtils PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(FileUtils PUBLIC RaiiString)

set(FileUtils_PUBLIC_INCLUDE_PATH ${CoDeLib_PUBLIC_INCLUDE_PATH}/FileUtils)
set(FileUtils_PUBLIC_HEADERS
    ${FileUtils_PUBLIC_INCLUDE_PATH}/FileUtils.h
)
install(FILES ${FileUtils_PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/CoDeLib/FileUtils)

