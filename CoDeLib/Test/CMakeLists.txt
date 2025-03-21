add_executable(CoDeLib_Test
    src/main.c
    src/TestRaiiString.c
    src/TestDeflateInflateZlib.c
    src/TestFileUtils.c
    src/TestUnZipMinizip.c
    src/TestZipMinizip.c
    src/TestUnZipMinizipInflateZlib.c
    src/TestZipMinizipUnZipMinizip.c
    src/TestZipContentInfo.c
)

target_include_directories(CoDeLib_Test PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

FetchContent_Declare(
    Unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG 860062d51b2e8a75d150337b63ca2a472840d13c # v2.6.0
    EXCLUDE_FROM_ALL
)
# Solution to use `CACHE INTERNAL` found here: https://discourse.cmake.org/t/what-is-the-correct-way-to-set-options-of-a-project-before-fetch-content/268/4
# If cache is not used, you will get `Policy CMP0077 is not set: option() honors normal variables.` 
set(UNITY_EXTENSION_FIXTURE ON CACHE INTERNAL "")
FetchContent_MakeAvailable(Unity)

target_link_libraries(CoDeLib_Test PRIVATE Deflate_zlib)
target_link_libraries(CoDeLib_Test PRIVATE Inflate_zlib)
target_link_libraries(CoDeLib_Test PRIVATE FileUtils)
target_link_libraries(CoDeLib_Test PRIVATE UnZip_minizip)
target_link_libraries(CoDeLib_Test PRIVATE Zip_minizip)
target_link_libraries(CoDeLib_Test PRIVATE RaiiString)
target_link_libraries(CoDeLib_Test PRIVATE ZipContentInfo)

target_link_libraries(CoDeLib_Test PRIVATE unity)


