add_executable(CoDeLib_Test
    src/TestDeflateInflateZlib.c)

target_include_directories(CoDeLib_Test PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

target_link_libraries(CoDeLib_Test PRIVATE Deflate_zlib)
target_link_libraries(CoDeLib_Test PRIVATE Inflate_zlib)
target_link_libraries(CoDeLib_Test PRIVATE RaiiString)

FetchContent_Declare(
    Unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG 860062d51b2e8a75d150337b63ca2a472840d13c # v2.6.0
    EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(Unity)
target_link_libraries(CoDeLib_Test PRIVATE unity)

add_subdirectory(Utility)
target_link_libraries(CoDeLib_Test PUBLIC Utility)
