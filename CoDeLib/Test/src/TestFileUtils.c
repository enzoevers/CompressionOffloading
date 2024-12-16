#include "TestFileUtils.h"
#include "unity_fixture.h"
#include <CoDeLib/FileUtils/FileUtils.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

static char *g_pFullPathToBenchmarkTestFiles = NULL;
static char *g_pCurrentWorkingDirectory = NULL;

void SetupTestFileUtils(char *pFullPathToBenchmarkTestFiles,
                        char *pCurrentWorkingDirectory) {
    g_pFullPathToBenchmarkTestFiles = pFullPathToBenchmarkTestFiles;
    g_pCurrentWorkingDirectory = pCurrentWorkingDirectory;
}

TEST_GROUP(TestFileUtils);

TEST_SETUP(TestFileUtils) {}

TEST_TEAR_DOWN(TestFileUtils) {
    // TODO: Remove all created directories during tests
}

//==============================
// IsAbsolutePath(...)
//==============================

TEST(TestFileUtils, test_IsAbsolutePath_ReturnsFalseIfPathIsNull) {
    bool isAbsolutePath = IsAbsolutePath(NULL);
    TEST_ASSERT_FALSE(isAbsolutePath);
}

TEST(TestFileUtils, test_IsAbsolutePath_ReturnsFalseIfPathIsEmpty) {
    bool isAbsolutePath = IsAbsolutePath("");
    TEST_ASSERT_FALSE(isAbsolutePath);
}

TEST(TestFileUtils, test_IsAbsolutePath_ReturnsFalseIfPathIsRelative_Linux) {
    TEST_ASSERT_FALSE(IsAbsolutePath("tmp/tmp2/"));
    TEST_ASSERT_FALSE(IsAbsolutePath("tmp/tmp2"));
    TEST_ASSERT_FALSE(IsAbsolutePath("./tmp/tmp2/"));
    TEST_ASSERT_FALSE(IsAbsolutePath("./tmp/tmp2"));
}

TEST(TestFileUtils, test_IsAbsolutePath_ReturnsFalseIfPathIsRelative_Windows) {
    TEST_ASSERT_FALSE(IsAbsolutePath("tmp\\tmp2\\"));
    TEST_ASSERT_FALSE(IsAbsolutePath("tmp\\tmp2"));
    TEST_ASSERT_FALSE(IsAbsolutePath(".\\tmp\\tmp2\\"));
    TEST_ASSERT_FALSE(IsAbsolutePath(".\\tmp\\tmp2"));
}

TEST(TestFileUtils,
     test_IsAbsolutePath_ReturnsFalseIfPathStartIsInvalid_Windows) {
    TEST_ASSERT_FALSE(IsAbsolutePath("\\tmp\\tmp2"));
}

TEST(
    TestFileUtils,
    test_IsAbsolutePath_ReturnsFalseIfPathIsNotValidPath_Windows_MultiCharDriveLetter) {
    // Note that officialy Windows only supports single character drive letters.
    TEST_ASSERT_FALSE(IsAbsolutePath("DE:\\tmp\\tmp2\\"));
    TEST_ASSERT_FALSE(IsAbsolutePath("DE:\\tmp\\tmp2"));
}

TEST(TestFileUtils, test_IsAbsolutePath_ReturnsTrueIfPathIsAbsolute_Linux) {
    TEST_ASSERT_TRUE(IsAbsolutePath("/tmp/tmp2/"));
    TEST_ASSERT_TRUE(IsAbsolutePath("/tmp/tmp2"));
}

TEST(TestFileUtils, test_IsAbsolutePath_ReturnsTrueIfPathIsAbsolute_Windows) {
    TEST_ASSERT_TRUE(IsAbsolutePath("C:\\tmp\\tmp2\\"));
    TEST_ASSERT_TRUE(IsAbsolutePath("C:\\tmp\\tmp2"));
}

TEST(TestFileUtils,
     test_IsAbsolutePath_ReturnsTrueIfPathIsAbsolute_Windows_MixedSeparators) {
    TEST_ASSERT_TRUE(IsAbsolutePath("C:\\tmp/tmp2\\"));
    TEST_ASSERT_TRUE(IsAbsolutePath("C:\\tmp/tmp2"));
    TEST_ASSERT_TRUE(IsAbsolutePath("C:/tmp\\tmp2\\"));
    TEST_ASSERT_TRUE(IsAbsolutePath("C:/tmp\\tmp2"));
}

TEST(TestFileUtils,
     test_IsAbsolutePath_ReturnsTrueIfPathIsAbsolute_Linux_MixedSeparators) {
    TEST_ASSERT_TRUE(IsAbsolutePath("/tmp\\tmp2/"));
    TEST_ASSERT_TRUE(IsAbsolutePath("/tmp\\tmp2"));
}

//==============================
// GetCurrentWorkingDirectory(...)
//==============================

TEST(TestFileUtils, test_GetCurrentWorkingDirectory_ReturnsNullIfBufferIsNull) {
    char *pCurrentWorkingDirectory = GetCurrentWorkingDirectory(NULL, 0);
    TEST_ASSERT_NULL(pCurrentWorkingDirectory);
}

TEST(TestFileUtils, test_GetCurrentWorkingDirectory_ReturnsNullIfBufferIsZero) {
    char localBuffer[10];
    char *pCurrentWorkingDirectory =
        GetCurrentWorkingDirectory(&localBuffer[0], 0);
    TEST_ASSERT_NULL(pCurrentWorkingDirectory);
}

TEST(TestFileUtils,
     test_GetCurrentWorkingDirectory_ReturnsNullIfBufferIsTooSmall) {

    // This length does not include the null terminator. It therefore is too
    // small.
    const size_t cwdLength = strlen(g_pCurrentWorkingDirectory);
    char *pLocalBuffer = calloc(cwdLength, sizeof(char));

    char *pCurrentWorkingDirectory =
        GetCurrentWorkingDirectory(pLocalBuffer, cwdLength);

    TEST_ASSERT_NULL(pCurrentWorkingDirectory);

    free(pLocalBuffer);
}

TEST(TestFileUtils,
     test_GetCurrentWorkingDirectory_ReturnsCurrentPathIfBufferCorrectSize) {

    const size_t cwdLength = strlen(g_pCurrentWorkingDirectory);
    const size_t allocationLength = cwdLength + 1;
    char *pLocalBuffer = calloc(allocationLength, sizeof(char));

    char *pCurrentWorkingDirectory =
        GetCurrentWorkingDirectory(pLocalBuffer, allocationLength);
    TEST_ASSERT_EQUAL(pLocalBuffer, pCurrentWorkingDirectory);
    TEST_ASSERT_EQUAL_STRING(g_pCurrentWorkingDirectory,
                             pCurrentWorkingDirectory);
    free(pLocalBuffer);
}

TEST(
    TestFileUtils,
    test_GetCurrentWorkingDirectory_ReturnsCurrentPathIfBufferIsBiggerThanNeeded) {

    const size_t cwdLength = strlen(g_pCurrentWorkingDirectory);
    const size_t allocationLength = cwdLength + 1 + 10;
    char *pLocalBuffer = calloc(allocationLength, sizeof(char));

    char *pCurrentWorkingDirectory =
        GetCurrentWorkingDirectory(pLocalBuffer, allocationLength);
    TEST_ASSERT_EQUAL(pLocalBuffer, pCurrentWorkingDirectory);
    TEST_ASSERT_EQUAL_STRING(g_pCurrentWorkingDirectory,
                             pCurrentWorkingDirectory);
    free(pLocalBuffer);
}

TEST(TestFileUtils,
     test_GetCurrentWorkingDirectory_HasSeparatorAtEndIfNotAlreadyPresent) {

    char localBuffer[MAX_PATH_LENGTH_SIZE];
    GetCurrentWorkingDirectory(localBuffer, MAX_PATH_LENGTH_SIZE);

    const size_t cwdLength = strlen(localBuffer);
    TEST_ASSERT_TRUE(localBuffer[cwdLength - 1] == '/');
}

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
    if (statResult != 0) {
        printf("\npath: %s\n", tmpPath.pString);
        printf("error: %s\n", strerror(errno));
    }
    TEST_ASSERT_EQUAL(0, statResult);
#endif
}

TEST(TestFileUtils, test_RecursiveMkdir_CanCreateRecursiveAbsolutePath) {
    RAII_STRING tmpPath =
        RaiiStringCreateFromCString(g_pFullPathToBenchmarkTestFiles);
    RaiiStringAppend_cString(&tmpPath, "/tmp2/Other/");

    bool success = RecursiveMkdir(tmpPath.pString);
    TEST_ASSERT_TRUE(success);

// TODO: Create a `FileOrDirectoryExists(...)` function
#ifdef _WIN32
    const DWORD attributes = GetFileAttributes(tmpPath.pString);
    TEST_ASSERT_NOT_EQUAL(INVALID_FILE_ATTRIBUTES, attributes);
#else
    struct stat statbuf;
    const int statResult = stat(tmpPath.pString, &statbuf);
    if (statResult != 0) {
        printf("\npath: %s\n", tmpPath.pString);
        printf("error: %s\n", strerror(errno));
    }
    TEST_ASSERT_EQUAL(0, statResult);
#endif
}

TEST(TestFileUtils, test_RecursiveMkdir_CanCreateRelativePath) {
    const char *pDirname = "./tmp/";
    bool success = RecursiveMkdir(pDirname);
    TEST_ASSERT_TRUE(success);

// TODO: Create a `FileOrDirectoryExists(...)` function
#ifdef _WIN32
    char targetPath[MAX_PATH_LENGTH_SIZE];
    char *currentPath =
        GetCurrentWorkingDirectory(&targetPath[0], MAX_PATH_LENGTH_SIZE);
    if (currentPath == NULL) {
        TEST_FAIL_MESSAGE("Failed to get current working directory.");
    }
    const size_t currentPathLength = strlen(targetPath);
    const size_t dirnameLength = strlen(pDirname);
    // (dirnameLength + 1) to include the null terminator
    memcpy(&targetPath[currentPathLength], pDirname,
           sizeof(char) * (dirnameLength + 1));

    const DWORD attributes = GetFileAttributes(&targetPath[0]);
    TEST_ASSERT_NOT_EQUAL(INVALID_FILE_ATTRIBUTES, attributes);
#else
    struct stat statbuf;
    const int statResult = stat(pDirname, &statbuf);
    if (statResult != 0) {
        printf("\npath: %s \n", pDirname);
        printf("error: %s\n", strerror(errno));
    }
    TEST_ASSERT_EQUAL(0, statResult);
#endif
}

TEST(TestFileUtils, test_RecursiveMkdir_CanCreateRecursiveRelativePath) {
    const char *pDirname = "./tmp2/Other/";
    bool success = RecursiveMkdir(pDirname);
    TEST_ASSERT_TRUE(success);

// TODO: Create a `FileOrDirectoryExists(...)` function
#ifdef _WIN32
    char targetPath[MAX_PATH_LENGTH_SIZE];
    char *currentPath =
        GetCurrentWorkingDirectory(&targetPath[0], MAX_PATH_LENGTH_SIZE);
    if (currentPath == NULL) {
        TEST_FAIL_MESSAGE("Failed to get current working directory.");
    }
    const size_t currentPathLength = strlen(targetPath);
    const size_t dirnameLength = strlen(pDirname);
    // (dirnameLength + 1) to include the null terminator
    memcpy(&targetPath[currentPathLength], pDirname,
           sizeof(char) * (dirnameLength + 1));

    const DWORD attributes = GetFileAttributes(&targetPath[0]);
    TEST_ASSERT_NOT_EQUAL(INVALID_FILE_ATTRIBUTES, attributes);
#else
    struct stat statbuf;
    const int statResult = stat(pDirname, &statbuf);
    if (statResult != 0) {
        printf("\npath: %s \n", pDirname);
        printf("error: %s\n", strerror(errno));
    }
    TEST_ASSERT_EQUAL(0, statResult);
#endif
}

//==============================
// RecursiveRmdir(...)
//==============================

TEST(TestFileUtils, test_RecursiveRmdir_ReturnsFalseIfDirnameIsNull) {
    bool success = RecursiveRmdir(NULL);
    TEST_ASSERT_FALSE(success);
}

TEST(TestFileUtils, test_RecursiveRmdir_CanDeleteAbsolutePath) {
    RAII_STRING tmpPath =
        RaiiStringCreateFromCString(g_pFullPathToBenchmarkTestFiles);
    RaiiStringAppend_cString(&tmpPath, "/tmp/");

    bool success = RecursiveMkdir(tmpPath.pString);
    if (!success) {
        TEST_FAIL_MESSAGE("Failed to create directory.");
    }
    success = RecursiveRmdir(tmpPath.pString);
    TEST_ASSERT_TRUE(success);

#ifdef _WIN32
    const DWORD attributes = GetFileAttributes(tmpPath.pString);
    TEST_ASSERT_EQUAL(INVALID_FILE_ATTRIBUTES, attributes);
#else
    struct stat statbuf;
    const int statResult = stat(tmpPath.pString, &statbuf);
    TEST_ASSERT_EQUAL(-1, statResult);
#endif
}

TEST(TestFileUtils, test_RecursiveRmdir_CanDeleteRecursiveAbsolutePath) {
    RAII_STRING tmpPath =
        RaiiStringCreateFromCString(g_pFullPathToBenchmarkTestFiles);
    RaiiStringAppend_cString(&tmpPath, "/tmp2/");
    RAII_STRING tmpSubPath = RaiiStringCreateFromCString(tmpPath.pString);
    RaiiStringAppend_cString(&tmpSubPath, "Other/");

    bool success = RecursiveMkdir(tmpSubPath.pString);
    if (!success) {
        TEST_FAIL_MESSAGE("Failed to create directory.");
    }
    success = RecursiveRmdir(tmpPath.pString);
    TEST_ASSERT_TRUE(success);

#ifdef _WIN32
    const DWORD attributes = GetFileAttributes(tmpPath.pString);
    TEST_ASSERT_EQUAL(INVALID_FILE_ATTRIBUTES, attributes);
#else
    struct stat statbuf;
    const int statResult = stat(tmpPath.pString, &statbuf);
    TEST_ASSERT_EQUAL(-1, statResult);
#endif
}

TEST(TestFileUtils,
     test_RecursiveRmdir_CanDeleteRecursiveAbsolutePathWithFile) {
    RAII_STRING tmpPath =
        RaiiStringCreateFromCString(g_pFullPathToBenchmarkTestFiles);
    RaiiStringAppend_cString(&tmpPath, "/tmp2/");
    RAII_STRING tmpSubPath = RaiiStringCreateFromCString(tmpPath.pString);
    RaiiStringAppend_cString(&tmpSubPath, "Other/");

    bool success = RecursiveMkdir(tmpSubPath.pString);
    if (!success) {
        TEST_FAIL_MESSAGE("Failed to create directory.");
    }

    // Make a file in the directory
    RaiiStringAppend_cString(&tmpSubPath, "file.txt");
    FILE *pFile = fopen(tmpSubPath.pString, "w");
    if (pFile == NULL) {
        TEST_FAIL_MESSAGE("Failed to create file.");
    }
    fclose(pFile);

    success = RecursiveRmdir(tmpPath.pString);
    TEST_ASSERT_TRUE(success);

#ifdef _WIN32
    const DWORD attributes = GetFileAttributes(tmpPath.pString);
    TEST_ASSERT_EQUAL(INVALID_FILE_ATTRIBUTES, attributes);
#else
    struct stat statbuf;
    const int statResult = stat(tmpPath.pString, &statbuf);
    TEST_ASSERT_EQUAL(-1, statResult);
#endif
}

TEST(TestFileUtils, test_RecursiveRmdir_CanDeleteRelativePath) {
    const char *pDirname = "./tmp/";
    bool success = RecursiveMkdir(pDirname);
    if (!success) {
        TEST_FAIL_MESSAGE("Failed to create directory.");
    }
    success = RecursiveRmdir(pDirname);
    TEST_ASSERT_TRUE(success);

    // TODO: Create a `FileOrDirectoryExists(...)` function
#ifdef _WIN32
    char targetPath[MAX_PATH_LENGTH_SIZE];
    char *currentPath =
        GetCurrentWorkingDirectory(&targetPath[0], MAX_PATH_LENGTH_SIZE);
    if (currentPath == NULL) {
        TEST_FAIL_MESSAGE("Failed to get current working directory.");
    }
    const size_t currentPathLength = strlen(targetPath);
    const size_t dirnameLength = strlen(pDirname);
    // (dirnameLength + 1) to include the null terminator
    memcpy(&targetPath[currentPathLength], pDirname,
           sizeof(char) * (dirnameLength + 1));

    const DWORD attributes = GetFileAttributes(&targetPath[0]);
    TEST_ASSERT_EQUAL(INVALID_FILE_ATTRIBUTES, attributes);
#else
    struct stat statbuf;
    const int statResult = stat(pDirname, &statbuf);
    TEST_ASSERT_EQUAL(-1, statResult);
#endif
}

TEST(TestFileUtils, test_RecursiveRmdir_CanDeleteRecursiveRelativePath) {
    RAII_STRING tmpPath = RaiiStringCreateFromCString("./tmp/");
    RAII_STRING tmpSubPath = RaiiStringCreateFromCString(tmpPath.pString);
    RaiiStringAppend_cString(&tmpSubPath, "Other/");

    bool success = RecursiveMkdir(tmpSubPath.pString);
    if (!success) {
        TEST_FAIL_MESSAGE("Failed to create directory.");
    }

    success = RecursiveRmdir(tmpPath.pString);
    TEST_ASSERT_TRUE(success);

    // TODO: Create a `FileOrDirectoryExists(...)` function
#ifdef _WIN32
    char targetPath[MAX_PATH_LENGTH_SIZE];
    char *currentPath =
        GetCurrentWorkingDirectory(&targetPath[0], MAX_PATH_LENGTH_SIZE);
    if (currentPath == NULL) {
        TEST_FAIL_MESSAGE("Failed to get current working directory.");
    }
    const size_t currentPathLength = strlen(targetPath);
    const size_t dirnameLength = strlen(tmpPath.pString);
    // (dirnameLength + 1) to include the null terminator
    memcpy(&targetPath[currentPathLength], tmpPath.pString,
           sizeof(char) * (dirnameLength + 1));

    const DWORD attributes = GetFileAttributes(&targetPath[0]);
    TEST_ASSERT_EQUAL(INVALID_FILE_ATTRIBUTES, attributes);
#else
    struct stat statbuf;
    const int statResult = stat(tmpPath.pString, &statbuf);
    TEST_ASSERT_EQUAL(-1, statResult);
#endif
}

TEST(TestFileUtils,
     test_RecursiveRmdir_CanDeleteRecursiveRelativePathWithFile) {
    RAII_STRING tmpPath = RaiiStringCreateFromCString("./tmp/");
    RAII_STRING tmpSubPath = RaiiStringCreateFromCString(tmpPath.pString);
    RaiiStringAppend_cString(&tmpSubPath, "Other/");

    bool success = RecursiveMkdir(tmpSubPath.pString);
    if (!success) {
        TEST_FAIL_MESSAGE("Failed to create directory.");
    }

    // Make a file in the directory
    RaiiStringAppend_cString(&tmpSubPath, "file.txt");
    FILE *pFile = fopen(tmpSubPath.pString, "w");
    if (pFile == NULL) {
        TEST_FAIL_MESSAGE("Failed to create file.");
    }
    fclose(pFile);

    success = RecursiveRmdir(tmpPath.pString);
    TEST_ASSERT_TRUE(success);

    // TODO: Create a `FileOrDirectoryExists(...)` function
#ifdef _WIN32
    char targetPath[MAX_PATH_LENGTH_SIZE];
    char *currentPath =
        GetCurrentWorkingDirectory(&targetPath[0], MAX_PATH_LENGTH_SIZE);
    if (currentPath == NULL) {
        TEST_FAIL_MESSAGE("Failed to get current working directory.");
    }
    const size_t currentPathLength = strlen(targetPath);
    const size_t dirnameLength = strlen(tmpPath.pString);
    // (dirnameLength + 1) to include the null terminator
    memcpy(&targetPath[currentPathLength], tmpPath.pString,
           sizeof(char) * (dirnameLength + 1));

    const DWORD attributes = GetFileAttributes(&targetPath[0]);
    TEST_ASSERT_EQUAL(INVALID_FILE_ATTRIBUTES, attributes);
#else
    struct stat statbuf;
    const int statResult = stat(tmpPath.pString, &statbuf);
    TEST_ASSERT_EQUAL(-1, statResult);
#endif
}

//==============================
// TEST_GROUP_RUNNER
//==============================

TEST_GROUP_RUNNER(TestFileUtils) {
    // IsAbsolutePath(...)
    RUN_TEST_CASE(TestFileUtils, test_IsAbsolutePath_ReturnsFalseIfPathIsNull);
    RUN_TEST_CASE(TestFileUtils, test_IsAbsolutePath_ReturnsFalseIfPathIsEmpty);
    RUN_TEST_CASE(TestFileUtils,
                  test_IsAbsolutePath_ReturnsFalseIfPathIsRelative_Linux);
    RUN_TEST_CASE(TestFileUtils,
                  test_IsAbsolutePath_ReturnsFalseIfPathIsRelative_Windows);
    RUN_TEST_CASE(TestFileUtils,
                  test_IsAbsolutePath_ReturnsFalseIfPathStartIsInvalid_Windows);
    RUN_TEST_CASE(
        TestFileUtils,
        test_IsAbsolutePath_ReturnsFalseIfPathIsNotValidPath_Windows_MultiCharDriveLetter);
    RUN_TEST_CASE(TestFileUtils,
                  test_IsAbsolutePath_ReturnsTrueIfPathIsAbsolute_Linux);
    RUN_TEST_CASE(TestFileUtils,
                  test_IsAbsolutePath_ReturnsTrueIfPathIsAbsolute_Windows);
    RUN_TEST_CASE(
        TestFileUtils,
        test_IsAbsolutePath_ReturnsTrueIfPathIsAbsolute_Windows_MixedSeparators);
    RUN_TEST_CASE(
        TestFileUtils,
        test_IsAbsolutePath_ReturnsTrueIfPathIsAbsolute_Linux_MixedSeparators);

    // GetCurrentWorkingDirectory(...)
    RUN_TEST_CASE(TestFileUtils,
                  test_GetCurrentWorkingDirectory_ReturnsNullIfBufferIsNull);
    RUN_TEST_CASE(TestFileUtils,
                  test_GetCurrentWorkingDirectory_ReturnsNullIfBufferIsZero);
    RUN_TEST_CASE(
        TestFileUtils,
        test_GetCurrentWorkingDirectory_ReturnsNullIfBufferIsTooSmall);
    RUN_TEST_CASE(
        TestFileUtils,
        test_GetCurrentWorkingDirectory_ReturnsCurrentPathIfBufferCorrectSize);
    RUN_TEST_CASE(
        TestFileUtils,
        test_GetCurrentWorkingDirectory_ReturnsCurrentPathIfBufferIsBiggerThanNeeded);
    RUN_TEST_CASE(
        TestFileUtils,
        test_GetCurrentWorkingDirectory_HasSeparatorAtEndIfNotAlreadyPresent);

    // RecursiveMkdir(...)
    RUN_TEST_CASE(TestFileUtils,
                  test_RecursiveMkdir_ReturnsFalseIfDirnameIsNull);
    RUN_TEST_CASE(TestFileUtils, test_RecursiveMkdir_CanCreateAbsolutePath);
    RUN_TEST_CASE(TestFileUtils,
                  test_RecursiveMkdir_CanCreateRecursiveAbsolutePath);
    RUN_TEST_CASE(TestFileUtils, test_RecursiveMkdir_CanCreateRelativePath);
    RUN_TEST_CASE(TestFileUtils,
                  test_RecursiveMkdir_CanCreateRecursiveRelativePath);

    // RecursiveRmdir(...)
    // RUN_TEST_CASE(TestFileUtils,
    //               test_RecursiveRmdir_ReturnsFalseIfDirnameIsNull);
    // RUN_TEST_CASE(TestFileUtils, test_RecursiveRmdir_CanDeleteAbsolutePath);
    // RUN_TEST_CASE(TestFileUtils,
    //               test_RecursiveRmdir_CanDeleteRecursiveAbsolutePath);
    // RUN_TEST_CASE(TestFileUtils,
    //               test_RecursiveRmdir_CanDeleteRecursiveAbsolutePathWithFile);
    // RUN_TEST_CASE(TestFileUtils, test_RecursiveRmdir_CanDeleteRelativePath);
    // RUN_TEST_CASE(TestFileUtils,
    //               test_RecursiveRmdir_CanDeleteRecursiveRelativePath);
    // RUN_TEST_CASE(TestFileUtils,
    //               test_RecursiveRmdir_CanDeleteRecursiveRelativePathWithFile);
}
