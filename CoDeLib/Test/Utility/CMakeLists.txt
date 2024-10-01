add_library(Utility STATIC
    FileUtils.c
)

target_include_directories(Utility PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(Utility PUBLIC RaiiString)

set(Utility_PUBLIC_INCLUDE_PATH ${CoDeLib_PUBLIC_INCLUDE_PATH}/Test/Utility)
set(Utility_PUBLIC_HEADERS
    ${Utility_PUBLIC_INCLUDE_PATH}/FileUtils.h
)
install(FILES ${Utility_PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/CoDeLib/Test/Utility)

