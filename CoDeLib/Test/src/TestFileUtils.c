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

    char localBuffer[MAX_PATH_LENGTH_WTH_TERMINATOR];
    GetCurrentWorkingDirectory(localBuffer, MAX_PATH_LENGTH_WTH_TERMINATOR);

    const size_t cwdLength = strlen(localBuffer);
    TEST_ASSERT_TRUE(localBuffer[cwdLength - 1] == '/');
}

//==============================
// GetAbsolutePath(...)
//==============================

TEST(TestFileUtils, test_GetAbsolutePath_ReturnsFalseIfPathIsNull) {
    char localBuffer[MAX_PATH_LENGTH_WTH_TERMINATOR];
    bool success =
        GetAbsolutePath(NULL, localBuffer, MAX_PATH_LENGTH_WTH_TERMINATOR);
    TEST_ASSERT_FALSE(success);
}

TEST(TestFileUtils, test_GetAbsolutePath_ReturnsFalseIfBufferIsNull) {
    bool success = GetAbsolutePath("tmp/tmp2/", NULL, 10);
    TEST_ASSERT_FALSE(success);
}

TEST(TestFileUtils, test_GetAbsolutePath_ReturnsFalseIfBufferSizeIsZero) {
    char localBuffer[MAX_PATH_LENGTH_WTH_TERMINATOR];
    bool success = GetAbsolutePath("tmp/tmp2/", localBuffer, 0);
    TEST_ASSERT_FALSE(success);
}

TEST(TestFileUtils,
     test_GetAbsolutePath_ReturnsFalseIfBufferSizeIsLessThenPathSize) {
    char *pPath = "tmp/tmp2/";
    const size_t pathLength = strlen(pPath);
    char localBuffer[pathLength];
    bool success = GetAbsolutePath(pPath, localBuffer, pathLength);
    TEST_ASSERT_FALSE(success);
}

TEST(TestFileUtils,
     test_GetAbsolutePath_AllowsBuffersToBeBiggerThanMaxPathLength) {
    char localBuffer[MAX_PATH_LENGTH_WTH_TERMINATOR + 1];
    bool success = GetAbsolutePath("tmp/tmp2/", localBuffer,
                                   MAX_PATH_LENGTH_WTH_TERMINATOR + 1);
    TEST_ASSERT_TRUE(success);
}

TEST(
    TestFileUtils,
    test_GetAbsolutePath_ReturnsTrueIfPathIsAlreadyAbsoluteAndCopiesItToBuffer_TrailingSlash_WindowsStyle) {
    char localBuffer[MAX_PATH_LENGTH_WTH_TERMINATOR];
    char *pPath = "C:\\a\\b\\";

    bool success =
        GetAbsolutePath(pPath, localBuffer, MAX_PATH_LENGTH_WTH_TERMINATOR);

    TEST_ASSERT_TRUE(success);
    TEST_ASSERT_NOT_EQUAL(pPath, &localBuffer[0]);
    TEST_ASSERT_EQUAL_STRING(pPath, localBuffer);
}

TEST(
    TestFileUtils,
    test_GetAbsolutePath_ReturnsTrueIfPathIsAlreadyAbsoluteAndCopiesItToBuffer_NoTrailingSlash_WindowsStyle) {
    char localBuffer[MAX_PATH_LENGTH_WTH_TERMINATOR];
    char *pPath = "C:\\a\\b";

    bool success =
        GetAbsolutePath(pPath, localBuffer, MAX_PATH_LENGTH_WTH_TERMINATOR);

    TEST_ASSERT_TRUE(success);
    TEST_ASSERT_NOT_EQUAL(pPath, &localBuffer[0]);
    TEST_ASSERT_EQUAL_STRING(pPath, localBuffer);
}

TEST(
    TestFileUtils,
    test_GetAbsolutePath_ReturnsTrueIfPathIsAlreadyAbsoluteAndCopiesItToBuffer_FileExtention_WindowsStyle) {
    char localBuffer[MAX_PATH_LENGTH_WTH_TERMINATOR];
    char *pPath = "C:\\a\\b.txt";

    bool success =
        GetAbsolutePath(pPath, localBuffer, MAX_PATH_LENGTH_WTH_TERMINATOR);

    TEST_ASSERT_TRUE(success);
    TEST_ASSERT_NOT_EQUAL(pPath, &localBuffer[0]);
    TEST_ASSERT_EQUAL_STRING(pPath, localBuffer);
}

TEST(
    TestFileUtils,
    test_GetAbsolutePath_ReturnsTrueIfPathIsAlreadyAbsoluteAndCopiesItToBuffer_TrailingSlash_PosixStyle) {
    char localBuffer[MAX_PATH_LENGTH_WTH_TERMINATOR];
    char *pPath = "/a/b/";

    bool success =
        GetAbsolutePath(pPath, localBuffer, MAX_PATH_LENGTH_WTH_TERMINATOR);

    TEST_ASSERT_TRUE(success);
    TEST_ASSERT_NOT_EQUAL(pPath, &localBuffer[0]);
    TEST_ASSERT_EQUAL_STRING(pPath, localBuffer);
}

TEST(
    TestFileUtils,
    test_GetAbsolutePath_ReturnsTrueIfPathIsAlreadyAbsoluteAndCopiesItToBuffer_NoTrailingSlash_PosixStyle) {
    char localBuffer[MAX_PATH_LENGTH_WTH_TERMINATOR];
    char *pPath = "/a/b";

    bool success =
        GetAbsolutePath(pPath, localBuffer, MAX_PATH_LENGTH_WTH_TERMINATOR);

    TEST_ASSERT_TRUE(success);
    TEST_ASSERT_NOT_EQUAL(pPath, &localBuffer[0]);
    TEST_ASSERT_EQUAL_STRING(pPath, localBuffer);
}

TEST(
    TestFileUtils,
    test_GetAbsolutePath_ReturnsTrueIfPathIsAlreadyAbsoluteAndCopiesItToBuffer_FileExtentions_PosixStyle) {
    char localBuffer[MAX_PATH_LENGTH_WTH_TERMINATOR];
    char *pPath = "/a/b.txt";

    bool success =
        GetAbsolutePath(pPath, localBuffer, MAX_PATH_LENGTH_WTH_TERMINATOR);

    TEST_ASSERT_TRUE(success);
    TEST_ASSERT_NOT_EQUAL(pPath, &localBuffer[0]);
    TEST_ASSERT_EQUAL_STRING(pPath, localBuffer);
}

TEST(
    TestFileUtils,
    test_GetAbsolutePath_ReturnsTrueIfPathIsRelativeAndCopiesItToBuffer_TrailingSlash_WindowsStyle) {
    char expectedAbsolutePathBuffer[MAX_PATH_LENGTH_WTH_TERMINATOR];
    GetCurrentWorkingDirectory(expectedAbsolutePathBuffer,
                               MAX_PATH_LENGTH_WTH_TERMINATOR);
    RAII_STRING expectedAbsolutePath =
        RaiiStringCreateFromCString(expectedAbsolutePathBuffer);

    char localBuffer[MAX_PATH_LENGTH_WTH_TERMINATOR];
    char *pPath = "b\\";

    RaiiStringAppend_cString(&expectedAbsolutePath, pPath);

    bool success =
        GetAbsolutePath(pPath, localBuffer, MAX_PATH_LENGTH_WTH_TERMINATOR);

    TEST_ASSERT_TRUE(success);
    TEST_ASSERT_GREATER_THAN(strlen(pPath), strlen(&localBuffer[0]));
    TEST_ASSERT_EQUAL_STRING(expectedAbsolutePath.pString, localBuffer);
}

TEST(
    TestFileUtils,
    test_GetAbsolutePath_ReturnsTrueIfPathIsRelativeAndCopiesItToBuffer_TrailingSlash_PosixStyle) {
    char expectedAbsolutePathBuffer[MAX_PATH_LENGTH_WTH_TERMINATOR];
    GetCurrentWorkingDirectory(expectedAbsolutePathBuffer,
                               MAX_PATH_LENGTH_WTH_TERMINATOR);
    RAII_STRING expectedAbsolutePath =
        RaiiStringCreateFromCString(expectedAbsolutePathBuffer);

    char localBuffer[MAX_PATH_LENGTH_WTH_TERMINATOR];
    char *pPath = "b/";

    RaiiStringAppend_cString(&expectedAbsolutePath, pPath);

    bool success =
        GetAbsolutePath(pPath, localBuffer, MAX_PATH_LENGTH_WTH_TERMINATOR);

    TEST_ASSERT_TRUE(success);
    TEST_ASSERT_GREATER_THAN(strlen(pPath), strlen(&localBuffer[0]));
    TEST_ASSERT_EQUAL_STRING(expectedAbsolutePath.pString, localBuffer);
}

//==============================
// PathExists(...)
//==============================

TEST(TestFileUtils, test_PathExists_ReturnsFalseIfPathIsNull) {
    bool exists = PathExists(NULL);
    TEST_ASSERT_FALSE(exists);
}

TEST(TestFileUtils, test_PathExists_ReturnsFalseIfPathIsEmpty) {
    bool exists = PathExists("");
    TEST_ASSERT_FALSE(exists);
}

TEST(TestFileUtils,
     test_PathExists_ReturnsFalseIfAbsolutePathDoesNotExist_WindowsStyle) {
    bool exists = PathExists("C:\\NonExistentPath\\");
    TEST_ASSERT_FALSE(exists);
}

TEST(TestFileUtils,
     test_PathExists_ReturnsFalseIfAbsolutePathDoesNotExist_PosixStyle) {
    bool exists = PathExists("/NonExistentPath/");
    TEST_ASSERT_FALSE(exists);
}

TEST(TestFileUtils,
     test_PathExists_ReturnsTrueIfAbsolutePathToDirectoryExists) {
    bool exists = PathExists(g_pCurrentWorkingDirectory);
    TEST_ASSERT_TRUE(exists);
}

TEST(TestFileUtils,
     test_PathExists_ReturnsFalseIfAbsolutePathToDirectoryDoesNotExist) {
    RAII_STRING tmpPath =
        RaiiStringCreateFromCString(g_pFullPathToBenchmarkTestFiles);
    RaiiStringAppend_cString(&tmpPath, "/NonExistentPath/");

    bool exists = PathExists(tmpPath.pString);
    TEST_ASSERT_FALSE(exists);
}

TEST(TestFileUtils,
     test_PathExists_ReturnsFalseIfAbsolutePathToFileDoesNotExist) {
    RAII_STRING tmpPath =
        RaiiStringCreateFromCString(g_pFullPathToBenchmarkTestFiles);
    RaiiStringAppend_cString(&tmpPath, "/NonExistentFile.txt");

    bool exists = PathExists(tmpPath.pString);
    TEST_ASSERT_FALSE(exists);
}

TEST(TestFileUtils,
     test_PathExists_ReturnsTrueIfRelativePathToDirectoryExists) {
    bool exists = PathExists("./");
    TEST_ASSERT_TRUE(exists);
}

TEST(TestFileUtils, test_PathExists_ReturnsTrueIfAbsolutePathToFileExists) {
    RAII_STRING tmpPath =
        RaiiStringCreateFromCString(g_pFullPathToBenchmarkTestFiles);
    RaiiStringAppend_cString(&tmpPath, "SmallBasicTextFile.txt");

    bool exists = PathExists(tmpPath.pString);
    TEST_ASSERT_TRUE(exists);
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

    TEST_ASSERT_TRUE(PathExists(tmpPath.pString));

    TEST_ASSERT_TRUE(RecursiveRmdir(tmpPath.pString));
}

TEST(TestFileUtils, test_RecursiveMkdir_CanCreateRecursiveAbsolutePath) {
    RAII_STRING tmpPath =
        RaiiStringCreateFromCString(g_pFullPathToBenchmarkTestFiles);
    RaiiStringAppend_cString(&tmpPath, "/tmp2/");
    RAII_STRING tmpSubPath = RaiiStringCreateFromCString(tmpPath.pString);
    RaiiStringAppend_cString(&tmpSubPath, "Other/");

    bool success = RecursiveMkdir(tmpSubPath.pString);
    TEST_ASSERT_TRUE(success);

    TEST_ASSERT_TRUE(PathExists(tmpSubPath.pString));

    TEST_ASSERT_TRUE(RecursiveRmdir(tmpPath.pString));
}

TEST(TestFileUtils, test_RecursiveMkdir_CanCreateRelativePath) {
    const char *pDirname = "./tmp/";
    bool success = RecursiveMkdir(pDirname);
    TEST_ASSERT_TRUE(success);

    TEST_ASSERT_TRUE(PathExists(pDirname));

    TEST_ASSERT_TRUE(RecursiveRmdir(pDirname));
}

TEST(TestFileUtils, test_RecursiveMkdir_CanCreateRecursiveRelativePath) {
    const char *pDirname = "./tmp2/";
    RAII_STRING tmpPath = RaiiStringCreateFromCString(pDirname);
    RAII_STRING tmpSubPath = RaiiStringCreateFromCString(tmpPath.pString);
    RaiiStringAppend_cString(&tmpSubPath, "Other/");

    bool success = RecursiveMkdir(tmpSubPath.pString);
    TEST_ASSERT_TRUE(success);

    TEST_ASSERT_TRUE(PathExists(tmpSubPath.pString));

    TEST_ASSERT_TRUE(RecursiveRmdir(tmpPath.pString));
}

//==============================
// RecursiveRmdir(...)
//==============================

TEST(TestFileUtils, test_RecursiveRmdir_ReturnsFalseIfDirnameIsNull) {
    bool success = RecursiveRmdir(NULL);
    TEST_ASSERT_FALSE(success);
}

TEST(TestFileUtils, test_RecursiveRmdir_ReturnsFalseIfDirnameIsEmpty) {
    bool success = RecursiveRmdir("");
    TEST_ASSERT_FALSE(success);
}

TEST(TestFileUtils,
     test_RecursiveRmdir_ReturnsFalseIfDirnameIsLargerThanMaxPath) {
    char localBuffer[MAX_PATH_LENGTH_WTH_TERMINATOR + 1];
    memset(localBuffer, 'a', MAX_PATH_LENGTH_WTH_TERMINATOR);
    localBuffer[MAX_PATH_LENGTH_WTH_TERMINATOR] = '\0';

    bool success = RecursiveRmdir(localBuffer);
    TEST_ASSERT_FALSE(success);
}

TEST(
    TestFileUtils,
    test_RecursiveRmdir_ReturnsFalseIfRelativePathDoesNotEndInSlash_WindowsStyle) {
    bool success = RecursiveRmdir(".\\NonExisting\\other");
    TEST_ASSERT_FALSE(success);
}

TEST(
    TestFileUtils,
    test_RecursiveRmdir_ReturnsFalseIfRelativePathDoesNotEndInSlash_PosixStyle) {
    bool success = RecursiveRmdir("./NonExisting/other");
    TEST_ASSERT_FALSE(success);
}

TEST(
    TestFileUtils,
    test_RecursiveRmdir_ReturnsFalseIfAbsolutePathDoesNotEndInSlash_WindowsStyle) {
    bool success = RecursiveRmdir("C:\\NonExisting\\other");
    TEST_ASSERT_FALSE(success);
}

TEST(
    TestFileUtils,
    test_RecursiveRmdir_ReturnsFalseIfAbsolutePathDoesNotEndInSlash_PosixStyle) {
    bool success = RecursiveRmdir("/NonExisting/other");
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

    TEST_ASSERT_FALSE(PathExists(tmpPath.pString));
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

    TEST_ASSERT_FALSE(PathExists(tmpPath.pString));
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

    TEST_ASSERT_FALSE(PathExists(tmpPath.pString));
}

TEST(TestFileUtils, test_RecursiveRmdir_CanDeleteRelativePath) {
    const char *pDirname = "./tmp/";
    bool success = RecursiveMkdir(pDirname);
    if (!success) {
        TEST_FAIL_MESSAGE("Failed to create directory.");
    }
    success = RecursiveRmdir(pDirname);
    TEST_ASSERT_TRUE(success);

    TEST_ASSERT_FALSE(PathExists(pDirname));
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

    TEST_ASSERT_FALSE(PathExists(tmpPath.pString));
}

TEST(
    TestFileUtils,
    test_RecursiveRmdir_CanDeleteRecursiveRelativePathWithMultipleDirectories) {
    RAII_STRING tmpPath = RaiiStringCreateFromCString("./tmp/");
    RAII_STRING tmpSubPath1 = RaiiStringCreateFromCString(tmpPath.pString);
    RaiiStringAppend_cString(&tmpSubPath1, "Other1/");
    RAII_STRING tmpSubPath2 = RaiiStringCreateFromCString(tmpPath.pString);
    RaiiStringAppend_cString(&tmpSubPath2, "Other2/");

    bool success = RecursiveMkdir(tmpSubPath1.pString);
    if (!success) {
        TEST_FAIL_MESSAGE("Failed to create directory 1.");
    }
    success = RecursiveMkdir(tmpSubPath2.pString);
    if (!success) {
        TEST_FAIL_MESSAGE("Failed to create directory 2.");
    }

    success = RecursiveRmdir(tmpPath.pString);
    TEST_ASSERT_TRUE(success);

    TEST_ASSERT_FALSE(PathExists(tmpPath.pString));
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

    TEST_ASSERT_FALSE(PathExists(tmpPath.pString));
}

//==============================
// ExtractLastPartOfPath(...)
//==============================

TEST(TestFileUtils, test_ExtractLastPartOfPath_ReturnsInvalidIfPathIsNull) {
    const char *pPath = NULL;
    char lastPartBuffer[MAX_PATH_LENGTH_WTH_TERMINATOR];
    size_t indexInPath = ExtractLastPartOfPath(pPath, &lastPartBuffer[0],
                                               MAX_PATH_LENGTH_WTH_TERMINATOR);

    TEST_ASSERT_EQUAL_size_t(SIZE_MAX, indexInPath);
}

TEST(TestFileUtils, test_ExtractLastPartOfPath_ReturnsInvalidIfPathIsEmpty) {
    const char *pPath = "";
    char lastPartBuffer[MAX_PATH_LENGTH_WTH_TERMINATOR];
    size_t indexInPath = ExtractLastPartOfPath(pPath, &lastPartBuffer[0],
                                               MAX_PATH_LENGTH_WTH_TERMINATOR);

    TEST_ASSERT_EQUAL_size_t(SIZE_MAX, indexInPath);
}

TEST(TestFileUtils,
     test_ExtractLastPartOfPath_ReturnsInvalidIfPathIsLargerThanMax) {
    char path[MAX_PATH_LENGTH_WTH_TERMINATOR + 1];
    memset(path, 'a', MAX_PATH_LENGTH_WTH_TERMINATOR);
    path[MAX_PATH_LENGTH_WTH_TERMINATOR] = '\0';

    char lastPartBuffer[MAX_PATH_LENGTH_WTH_TERMINATOR];
    size_t indexInPath = ExtractLastPartOfPath(path, &lastPartBuffer[0],
                                               MAX_PATH_LENGTH_WTH_TERMINATOR);

    TEST_ASSERT_EQUAL_size_t(SIZE_MAX, indexInPath);
}

TEST(TestFileUtils,
     test_ExtractLastPartOfPath_DoesNotReturnInvalidIfPathIsMaxSize) {
    char path[MAX_PATH_LENGTH_WTH_TERMINATOR];
    memset(path, 'a', MAX_PATH_LENGTH);
    path[MAX_PATH_LENGTH - 1] = '/';
    path[MAX_PATH_LENGTH] = '\0';

    char lastPartBuffer[MAX_PATH_LENGTH_WTH_TERMINATOR];
    size_t indexInPath = ExtractLastPartOfPath(path, &lastPartBuffer[0],
                                               MAX_PATH_LENGTH_WTH_TERMINATOR);

    TEST_ASSERT_NOT_EQUAL_size_t(SIZE_MAX, indexInPath);
    TEST_ASSERT_EQUAL_STRING(path, lastPartBuffer);
}

TEST(TestFileUtils, test_ExtractLastPartOfPath_ReturnsInvalidIfBufferIsNull) {
    const char *pPath = "./tmp/somefile.txt";
    char *pLastPartBuffer = NULL;
    size_t indexInPath = ExtractLastPartOfPath(pPath, pLastPartBuffer,
                                               MAX_PATH_LENGTH_WTH_TERMINATOR);

    TEST_ASSERT_EQUAL_size_t(SIZE_MAX, indexInPath);
}

TEST(TestFileUtils,
     test_ExtractLastPartOfPath_ReturnsInvalidIfBufferSizeIsZero) {
    const char *pPath = "./tmp/somefile.txt";
    char lastPartBuffer[MAX_PATH_LENGTH_WTH_TERMINATOR];
    size_t indexInPath = ExtractLastPartOfPath(pPath, &lastPartBuffer[0], 0);

    TEST_ASSERT_EQUAL_size_t(SIZE_MAX, indexInPath);
}

TEST(
    TestFileUtils,
    test_ExtractLastPartOfPath_ReturnsInvalidIfPathIsOnlySeparator_PosixStyle) {
    const char *pPath = "/";
    char lastPartBuffer[MAX_PATH_LENGTH_WTH_TERMINATOR];
    size_t indexInPath = ExtractLastPartOfPath(pPath, &lastPartBuffer[0],
                                               MAX_PATH_LENGTH_WTH_TERMINATOR);

    TEST_ASSERT_EQUAL_size_t(SIZE_MAX, indexInPath);
}

TEST(
    TestFileUtils,
    test_ExtractLastPartOfPath_ReturnsInvalidIfPathIsOnlySeparator_WindowsStyle) {
    const char *pPath = "\\";
    char lastPartBuffer[MAX_PATH_LENGTH_WTH_TERMINATOR];
    size_t indexInPath = ExtractLastPartOfPath(pPath, &lastPartBuffer[0],
                                               MAX_PATH_LENGTH_WTH_TERMINATOR);

    TEST_ASSERT_EQUAL_size_t(SIZE_MAX, indexInPath);
}

TEST(TestFileUtils,
     test_ExtractLastPartOfPath_ReturnsInvalidIfBufferSizeIsTooSmall_File) {
    const char *pPath = "./tmp/somefile.txt";

    // "somefile.txt" is 12 characters long,  plus null terminator = 13
    // So buffer size of 12 is too small.
    const size_t bufferSize = 12;
    char lastPartBuffer[bufferSize];
    size_t indexInPath =
        ExtractLastPartOfPath(pPath, &lastPartBuffer[0], bufferSize);

    TEST_ASSERT_EQUAL_size_t(SIZE_MAX, indexInPath);
}

TEST(
    TestFileUtils,
    test_ExtractLastPartOfPath_ReturnsInvalidIfBufferSizeIsTooSmall_Directory) {
    const char *pPath = "./tmp/someDir/";

    // "someDir/" is 8 characters long,  plus null terminator = 9
    // So buffer size of 8 is too small.
    const size_t bufferSize = 8;
    char lastPartBuffer[bufferSize];
    size_t indexInPath =
        ExtractLastPartOfPath(pPath, &lastPartBuffer[0], bufferSize);

    TEST_ASSERT_EQUAL_size_t(SIZE_MAX, indexInPath);
}

TEST(
    TestFileUtils,
    test_ExtractLastPartOfPath_ReturnsCorrectLastPartStringAndIndexIfBufferIsExcactSize_File) {
    const char *pPath = "./tmp/somefile.txt";

    // "somefile.txt" is 12 characters long,  plus null terminator = 13
    const size_t bufferSize = 13;
    char lastPartBuffer[bufferSize];
    size_t indexInPath =
        ExtractLastPartOfPath(pPath, &lastPartBuffer[0], bufferSize);

    TEST_ASSERT_EQUAL_size_t(6, indexInPath);
    TEST_ASSERT_EQUAL_STRING("somefile.txt", lastPartBuffer);
}

TEST(
    TestFileUtils,
    test_ExtractLastPartOfPath_ReturnsCorrectLastPartStringAndIndexIfBufferIsExcactSize_Directory) {
    const char *pPath = "./tmp/someDir/";

    // "someDir/" is 8 characters long,  plus null terminator = 9
    const size_t bufferSize = 9;
    char lastPartBuffer[bufferSize];
    size_t indexInPath =
        ExtractLastPartOfPath(pPath, &lastPartBuffer[0], bufferSize);

    TEST_ASSERT_EQUAL_size_t(6, indexInPath);
    TEST_ASSERT_EQUAL_STRING("someDir/", lastPartBuffer);
}

TEST(
    TestFileUtils,
    test_ExtractLastPartOfPath_ReturnsCorrectLastPartStringAndIndex_File_PosixStyle) {
    const char *pPath = "./tmp/somefile.txt";
    char lastPartBuffer[MAX_PATH_LENGTH_WTH_TERMINATOR];
    size_t indexInPath = ExtractLastPartOfPath(pPath, &lastPartBuffer[0],
                                               MAX_PATH_LENGTH_WTH_TERMINATOR);

    TEST_ASSERT_EQUAL_size_t(6, indexInPath);
    TEST_ASSERT_EQUAL_STRING("somefile.txt", lastPartBuffer);
}

TEST(
    TestFileUtils,
    test_ExtractLastPartOfPath_ReturnsCorrectLastPartStringAndIndex_Directory_PosixStyle) {
    const char *pPath = "./tmp/someDir/";
    char lastPartBuffer[MAX_PATH_LENGTH_WTH_TERMINATOR];
    size_t indexInPath = ExtractLastPartOfPath(pPath, &lastPartBuffer[0],
                                               MAX_PATH_LENGTH_WTH_TERMINATOR);

    TEST_ASSERT_EQUAL_size_t(6, indexInPath);
    TEST_ASSERT_EQUAL_STRING("someDir/", lastPartBuffer);
}

TEST(
    TestFileUtils,
    test_ExtractLastPartOfPath_ReturnsCorrectLastPartStringAndIndex_File_WindowsStyle) {
    const char *pPath = ".\\tmp\\somefile.txt";
    char lastPartBuffer[MAX_PATH_LENGTH_WTH_TERMINATOR];
    size_t indexInPath = ExtractLastPartOfPath(pPath, &lastPartBuffer[0],
                                               MAX_PATH_LENGTH_WTH_TERMINATOR);

    TEST_ASSERT_EQUAL_size_t(6, indexInPath);
    TEST_ASSERT_EQUAL_STRING("somefile.txt", lastPartBuffer);
}

TEST(
    TestFileUtils,
    test_ExtractLastPartOfPath_ReturnsCorrectLastPartStringAndIndex_Directory_WindowsStyle) {
    const char *pPath = ".\\tmp\\someDir\\";
    char lastPartBuffer[MAX_PATH_LENGTH_WTH_TERMINATOR];
    size_t indexInPath = ExtractLastPartOfPath(pPath, &lastPartBuffer[0],
                                               MAX_PATH_LENGTH_WTH_TERMINATOR);

    TEST_ASSERT_EQUAL_size_t(6, indexInPath);
    TEST_ASSERT_EQUAL_STRING("someDir\\", lastPartBuffer);
}

TEST(
    TestFileUtils,
    test_ExtractLastPartOfPath_ReturnsCorrectLastPartStringAndIndex_File_MixedStyle_PosixLast) {
    const char *pPath = ".\\tmp/somefile.txt";
    char lastPartBuffer[MAX_PATH_LENGTH_WTH_TERMINATOR];
    size_t indexInPath = ExtractLastPartOfPath(pPath, &lastPartBuffer[0],
                                               MAX_PATH_LENGTH_WTH_TERMINATOR);

    TEST_ASSERT_EQUAL_size_t(6, indexInPath);
    TEST_ASSERT_EQUAL_STRING("somefile.txt", lastPartBuffer);
}

TEST(
    TestFileUtils,
    test_ExtractLastPartOfPath_ReturnsCorrectLastPartStringAndIndex_Directory_MixedStyle_PosixLast) {
    const char *pPath = ".\\tmp\\someDir/";
    char lastPartBuffer[MAX_PATH_LENGTH_WTH_TERMINATOR];
    size_t indexInPath = ExtractLastPartOfPath(pPath, &lastPartBuffer[0],
                                               MAX_PATH_LENGTH_WTH_TERMINATOR);

    TEST_ASSERT_EQUAL_size_t(6, indexInPath);
    TEST_ASSERT_EQUAL_STRING("someDir/", lastPartBuffer);
}

TEST(
    TestFileUtils,
    test_ExtractLastPartOfPath_ReturnsCorrectLastPartStringAndIndex_File_MixedStyle_WindowsLast) {
    const char *pPath = "./tmp\\somefile.txt";
    char lastPartBuffer[MAX_PATH_LENGTH_WTH_TERMINATOR];
    size_t indexInPath = ExtractLastPartOfPath(pPath, &lastPartBuffer[0],
                                               MAX_PATH_LENGTH_WTH_TERMINATOR);

    TEST_ASSERT_EQUAL_size_t(6, indexInPath);
    TEST_ASSERT_EQUAL_STRING("somefile.txt", lastPartBuffer);
}

TEST(
    TestFileUtils,
    test_ExtractLastPartOfPath_ReturnsCorrectLastPartStringAndIndex_Directory_MixedStyle_WindowsLast) {
    const char *pPath = "./tmp/someDir\\";
    char lastPartBuffer[MAX_PATH_LENGTH_WTH_TERMINATOR];
    size_t indexInPath = ExtractLastPartOfPath(pPath, &lastPartBuffer[0],
                                               MAX_PATH_LENGTH_WTH_TERMINATOR);

    TEST_ASSERT_EQUAL_size_t(6, indexInPath);
    TEST_ASSERT_EQUAL_STRING("someDir\\", lastPartBuffer);
}

TEST(
    TestFileUtils,
    test_ExtractLastPartOfPath_ReturnsCorrectStringAndIndexWhenNoBasePath_File) {
    const char *pPath = "somefile.txt";
    char lastPartBuffer[MAX_PATH_LENGTH_WTH_TERMINATOR];
    size_t indexInPath = ExtractLastPartOfPath(pPath, &lastPartBuffer[0],
                                               MAX_PATH_LENGTH_WTH_TERMINATOR);

    TEST_ASSERT_EQUAL_size_t(0, indexInPath);
    TEST_ASSERT_EQUAL_STRING("somefile.txt", lastPartBuffer);
}

TEST(
    TestFileUtils,
    test_ExtractLastPartOfPath_ReturnsCorrectStringAndIndexWhenNoBasePath_Directory) {
    const char *pPath = "someDir/";
    char lastPartBuffer[MAX_PATH_LENGTH_WTH_TERMINATOR];
    size_t indexInPath = ExtractLastPartOfPath(pPath, &lastPartBuffer[0],
                                               MAX_PATH_LENGTH_WTH_TERMINATOR);

    TEST_ASSERT_EQUAL_size_t(0, indexInPath);
    TEST_ASSERT_EQUAL_STRING("someDir/", lastPartBuffer);
}

TEST(TestFileUtils,
     test_ExtractLastPartOfPath_IgnoresEmptyEntriesInPath_ForwardSlashes) {
    const char *pPath = "./tmp/someDir///";
    char lastPartBuffer[MAX_PATH_LENGTH_WTH_TERMINATOR];
    size_t indexInPath = ExtractLastPartOfPath(pPath, &lastPartBuffer[0],
                                               MAX_PATH_LENGTH_WTH_TERMINATOR);

    TEST_ASSERT_EQUAL_size_t(6, indexInPath);
    TEST_ASSERT_EQUAL_STRING("someDir/", lastPartBuffer);
}

TEST(TestFileUtils,
     test_ExtractLastPartOfPath_IgnoresEmptyEntriesInPath_BackwardSlashes) {
    const char *pPath = ".\\tmp\\someDir\\\\\\";
    char lastPartBuffer[MAX_PATH_LENGTH_WTH_TERMINATOR];
    size_t indexInPath = ExtractLastPartOfPath(pPath, &lastPartBuffer[0],
                                               MAX_PATH_LENGTH_WTH_TERMINATOR);

    TEST_ASSERT_EQUAL_size_t(6, indexInPath);
    TEST_ASSERT_EQUAL_STRING("someDir\\", lastPartBuffer);
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

    // GetAbsolutePath(...)
    RUN_TEST_CASE(TestFileUtils, test_GetAbsolutePath_ReturnsFalseIfPathIsNull);
    RUN_TEST_CASE(TestFileUtils,
                  test_GetAbsolutePath_ReturnsFalseIfBufferIsNull);
    RUN_TEST_CASE(TestFileUtils,
                  test_GetAbsolutePath_ReturnsFalseIfBufferSizeIsZero);
    RUN_TEST_CASE(
        TestFileUtils,
        test_GetAbsolutePath_ReturnsFalseIfBufferSizeIsLessThenPathSize);
    RUN_TEST_CASE(
        TestFileUtils,
        test_GetAbsolutePath_AllowsBuffersToBeBiggerThanMaxPathLength);
    RUN_TEST_CASE(
        TestFileUtils,
        test_GetAbsolutePath_ReturnsTrueIfPathIsAlreadyAbsoluteAndCopiesItToBuffer_TrailingSlash_WindowsStyle);
    RUN_TEST_CASE(
        TestFileUtils,
        test_GetAbsolutePath_ReturnsTrueIfPathIsAlreadyAbsoluteAndCopiesItToBuffer_NoTrailingSlash_WindowsStyle);
    RUN_TEST_CASE(
        TestFileUtils,
        test_GetAbsolutePath_ReturnsTrueIfPathIsAlreadyAbsoluteAndCopiesItToBuffer_FileExtention_WindowsStyle);
    RUN_TEST_CASE(
        TestFileUtils,
        test_GetAbsolutePath_ReturnsTrueIfPathIsAlreadyAbsoluteAndCopiesItToBuffer_TrailingSlash_PosixStyle);
    RUN_TEST_CASE(
        TestFileUtils,
        test_GetAbsolutePath_ReturnsTrueIfPathIsAlreadyAbsoluteAndCopiesItToBuffer_NoTrailingSlash_PosixStyle);
    RUN_TEST_CASE(
        TestFileUtils,
        test_GetAbsolutePath_ReturnsTrueIfPathIsAlreadyAbsoluteAndCopiesItToBuffer_FileExtentions_PosixStyle);
    RUN_TEST_CASE(
        TestFileUtils,
        test_GetAbsolutePath_ReturnsTrueIfPathIsRelativeAndCopiesItToBuffer_TrailingSlash_WindowsStyle);
    RUN_TEST_CASE(
        TestFileUtils,
        test_GetAbsolutePath_ReturnsTrueIfPathIsRelativeAndCopiesItToBuffer_TrailingSlash_PosixStyle);

    // PathExists(...)
    RUN_TEST_CASE(TestFileUtils, test_PathExists_ReturnsFalseIfPathIsNull);
    RUN_TEST_CASE(TestFileUtils, test_PathExists_ReturnsFalseIfPathIsEmpty);
    RUN_TEST_CASE(
        TestFileUtils,
        test_PathExists_ReturnsFalseIfAbsolutePathDoesNotExist_WindowsStyle);
    RUN_TEST_CASE(
        TestFileUtils,
        test_PathExists_ReturnsFalseIfAbsolutePathDoesNotExist_PosixStyle);
    RUN_TEST_CASE(TestFileUtils,
                  test_PathExists_ReturnsTrueIfAbsolutePathToDirectoryExists);
    RUN_TEST_CASE(
        TestFileUtils,
        test_PathExists_ReturnsFalseIfAbsolutePathToDirectoryDoesNotExist);
    RUN_TEST_CASE(TestFileUtils,
                  test_PathExists_ReturnsFalseIfAbsolutePathToFileDoesNotExist);
    RUN_TEST_CASE(TestFileUtils,
                  test_PathExists_ReturnsTrueIfRelativePathToDirectoryExists);
    RUN_TEST_CASE(TestFileUtils,
                  test_PathExists_ReturnsTrueIfAbsolutePathToFileExists);

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
    RUN_TEST_CASE(TestFileUtils,
                  test_RecursiveRmdir_ReturnsFalseIfDirnameIsNull);
    RUN_TEST_CASE(TestFileUtils,
                  test_RecursiveRmdir_ReturnsFalseIfDirnameIsEmpty);
    RUN_TEST_CASE(TestFileUtils,
                  test_RecursiveRmdir_ReturnsFalseIfDirnameIsLargerThanMaxPath);
    RUN_TEST_CASE(
        TestFileUtils,
        test_RecursiveRmdir_ReturnsFalseIfRelativePathDoesNotEndInSlash_WindowsStyle);
    RUN_TEST_CASE(
        TestFileUtils,
        test_RecursiveRmdir_ReturnsFalseIfRelativePathDoesNotEndInSlash_PosixStyle);
    RUN_TEST_CASE(
        TestFileUtils,
        test_RecursiveRmdir_ReturnsFalseIfAbsolutePathDoesNotEndInSlash_WindowsStyle);
    RUN_TEST_CASE(
        TestFileUtils,
        test_RecursiveRmdir_ReturnsFalseIfAbsolutePathDoesNotEndInSlash_PosixStyle);
    RUN_TEST_CASE(TestFileUtils, test_RecursiveRmdir_CanDeleteAbsolutePath);
    RUN_TEST_CASE(TestFileUtils,
                  test_RecursiveRmdir_CanDeleteRecursiveAbsolutePath);
    RUN_TEST_CASE(TestFileUtils,
                  test_RecursiveRmdir_CanDeleteRecursiveAbsolutePathWithFile);
    RUN_TEST_CASE(TestFileUtils, test_RecursiveRmdir_CanDeleteRelativePath);
    RUN_TEST_CASE(TestFileUtils,
                  test_RecursiveRmdir_CanDeleteRecursiveRelativePath);
    RUN_TEST_CASE(
        TestFileUtils,
        test_RecursiveRmdir_CanDeleteRecursiveRelativePathWithMultipleDirectories);
    RUN_TEST_CASE(TestFileUtils,
                  test_RecursiveRmdir_CanDeleteRecursiveRelativePathWithFile);

    // ExtractLastPartOfPath(...)
    RUN_TEST_CASE(TestFileUtils,
                  test_ExtractLastPartOfPath_ReturnsInvalidIfPathIsNull);
    RUN_TEST_CASE(TestFileUtils,
                  test_ExtractLastPartOfPath_ReturnsInvalidIfPathIsEmpty);
    RUN_TEST_CASE(
        TestFileUtils,
        test_ExtractLastPartOfPath_ReturnsInvalidIfPathIsLargerThanMax);
    RUN_TEST_CASE(
        TestFileUtils,
        test_ExtractLastPartOfPath_DoesNotReturnInvalidIfPathIsMaxSize);
    RUN_TEST_CASE(TestFileUtils,
                  test_ExtractLastPartOfPath_ReturnsInvalidIfBufferIsNull);
    RUN_TEST_CASE(TestFileUtils,
                  test_ExtractLastPartOfPath_ReturnsInvalidIfBufferSizeIsZero);
    RUN_TEST_CASE(
        TestFileUtils,
        test_ExtractLastPartOfPath_ReturnsInvalidIfPathIsOnlySeparator_PosixStyle);
    RUN_TEST_CASE(
        TestFileUtils,
        test_ExtractLastPartOfPath_ReturnsInvalidIfPathIsOnlySeparator_WindowsStyle);
    RUN_TEST_CASE(
        TestFileUtils,
        test_ExtractLastPartOfPath_ReturnsInvalidIfBufferSizeIsTooSmall_File);
    RUN_TEST_CASE(
        TestFileUtils,
        test_ExtractLastPartOfPath_ReturnsInvalidIfBufferSizeIsTooSmall_Directory);
    RUN_TEST_CASE(
        TestFileUtils,
        test_ExtractLastPartOfPath_ReturnsCorrectLastPartStringAndIndexIfBufferIsExcactSize_File);
    RUN_TEST_CASE(
        TestFileUtils,
        test_ExtractLastPartOfPath_ReturnsCorrectLastPartStringAndIndexIfBufferIsExcactSize_Directory);
    RUN_TEST_CASE(
        TestFileUtils,
        test_ExtractLastPartOfPath_ReturnsCorrectLastPartStringAndIndex_File_PosixStyle);
    RUN_TEST_CASE(
        TestFileUtils,
        test_ExtractLastPartOfPath_ReturnsCorrectLastPartStringAndIndex_Directory_PosixStyle);
    RUN_TEST_CASE(
        TestFileUtils,
        test_ExtractLastPartOfPath_ReturnsCorrectLastPartStringAndIndex_File_WindowsStyle);
    RUN_TEST_CASE(
        TestFileUtils,
        test_ExtractLastPartOfPath_ReturnsCorrectLastPartStringAndIndex_Directory_WindowsStyle);
    RUN_TEST_CASE(
        TestFileUtils,
        test_ExtractLastPartOfPath_ReturnsCorrectLastPartStringAndIndex_File_MixedStyle_PosixLast);
    RUN_TEST_CASE(
        TestFileUtils,
        test_ExtractLastPartOfPath_ReturnsCorrectLastPartStringAndIndex_Directory_MixedStyle_PosixLast);
    RUN_TEST_CASE(
        TestFileUtils,
        test_ExtractLastPartOfPath_ReturnsCorrectLastPartStringAndIndex_File_MixedStyle_WindowsLast);
    RUN_TEST_CASE(
        TestFileUtils,
        test_ExtractLastPartOfPath_ReturnsCorrectLastPartStringAndIndex_Directory_MixedStyle_WindowsLast);
    RUN_TEST_CASE(
        TestFileUtils,
        test_ExtractLastPartOfPath_ReturnsCorrectStringAndIndexWhenNoBasePath_File);
    RUN_TEST_CASE(
        TestFileUtils,
        test_ExtractLastPartOfPath_ReturnsCorrectStringAndIndexWhenNoBasePath_Directory);
    RUN_TEST_CASE(
        TestFileUtils,
        test_ExtractLastPartOfPath_IgnoresEmptyEntriesInPath_ForwardSlashes);
    RUN_TEST_CASE(
        TestFileUtils,
        test_ExtractLastPartOfPath_IgnoresEmptyEntriesInPath_BackwardSlashes);
}
