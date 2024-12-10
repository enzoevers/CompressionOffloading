#include "unity_fixture.h"
#include <CoDeLib/FileUtils/FileUtils.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/stat.h>
#endif

static char *g_pFullPathToBenchmarkTestFiles = NULL;

void SetupTestFileUtils(char *pFullPathToBenchmarkTestFiles) {
    g_pFullPathToBenchmarkTestFiles = pFullPathToBenchmarkTestFiles;
}

TEST_GROUP(TestFileUtils);

TEST_SETUP(TestFileUtils) {}

TEST_TEAR_DOWN(TestFileUtils) {}

//==============================
// RecursiveMkdir(...)
//==============================

TEST(TestFileUtils, test_RecursiveMkdir_ReturnsFalseIfDirnameIsNull) {
    bool success = RecursiveMkdir(NULL);
    TEST_ASSERT_FALSE(success);
}

TEST(TestFileUtils, test_RecursiveMkdir_CanCreateAbsolutePath) {
    RAII_STRING tmpPath =
        RaiiStringCreateFromCString(g_pFullPathToBenchmarkTestFiles);
    RaiiStringAppend_cString(&tmpPath, "/tmp/");

    bool success = RecursiveMkdir(tmpPath.pString);
    TEST_ASSERT_TRUE(success);

// TODO: Create a `FileOrDirectoryExists(...)` function
#ifdef _WIN32
    const DWORD attributes = GetFileAttributes(tmpPath.pString);
    TEST_ASSERT_NOT_EQUAL(INVALID_FILE_ATTRIBUTES, attributes);
#else
    struct stat statbuf;
    const int statResult = stat(tmpPath.pString, &statbuf);
    TEST_ASSERT_EQUAL(0, statResult);
#endif
}

TEST(TestFileUtils, test_RecursiveMkdir_CanCreateRelativePath) {
    TEST_IGNORE_MESSAGE("In progress");

    const char *pDirname = "./tmp/";
    bool success = RecursiveMkdir(pDirname);
    TEST_ASSERT_TRUE(success);

// TODO: Create a `FileOrDirectoryExists(...)` function
#ifdef _WIN32
    const DWORD attributes = GetFileAttributes(pDirname);
    TEST_ASSERT_NOT_EQUAL(INVALID_FILE_ATTRIBUTES, attributes);
#else
    struct stat statbuf;
    const int statResult = stat(pDirname, &statbuf);
    TEST_ASSERT_EQUAL(0, statResult);
#endif
}

//==============================
// TEST_GROUP_RUNNER
//==============================

TEST_GROUP_RUNNER(TestFileUtils) {
    RUN_TEST_CASE(TestFileUtils,
                  test_RecursiveMkdir_ReturnsFalseIfDirnameIsNull);
    RUN_TEST_CASE(TestFileUtils, test_RecursiveMkdir_CanCreateAbsolutePath);
    RUN_TEST_CASE(TestFileUtils, test_RecursiveMkdir_CanCreateRelativePath);
}
