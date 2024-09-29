
add_library(RaiiString STATIC
    src/RaiiString.c)

target_include_directories(RaiiString PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set(RaiiString_PUBLIC_INCLUDE_PATH ${CoDeLib_PUBLIC_INCLUDE_PATH}/RaiiString)
set(RaiiString_PUBLIC_HEADERS
    ${RaiiString_PUBLIC_INCLUDE_PATH}/RaiiString.h
)
install(FILES ${RaiiString_PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/CoDeLib/RaiiString)
