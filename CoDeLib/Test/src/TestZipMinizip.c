#include "TestZipMinizip.h"
#include "unity_fixture.h"
#include <CoDeLib/FileUtils/FileUtils.h>
#include <CoDeLib/RaiiString/RaiiString.h>
#include <CoDeLib/Zip_minizip/Zip_minizip.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static char *g_pFullPathToBenchmarkTestFiles;

void SetupTestZipMinizip(char *pFullPathToBenchmarkTestFiles) {
    g_pFullPathToBenchmarkTestFiles = pFullPathToBenchmarkTestFiles;
}

TEST_GROUP(TestZipMinizip);

static RaiiString g_pathToSmallBasicTextFileZipDeflate;
static RaiiString g_pathToSmallBasicTextFileZipSource;
static RaiiString g_pathToMultiTextFileZipSource;
static RaiiString g_pathToMultiTextFileAndSubDirZipSource;

TEST_SETUP(TestZipMinizip) {
    if (PathExists("./tmp/")) {
        TEST_ASSERT_TRUE(RecursiveRmdir("./tmp/"));
    }

    g_pathToSmallBasicTextFileZipDeflate =
        RaiiStringCreateFromCString(g_pFullPathToBenchmarkTestFiles);
    RaiiStringAppend_cString(&g_pathToSmallBasicTextFileZipDeflate,
                             "/SmallBasicTextFileZip/"
                             "SmallBasicTextFile_deflate.zip");

    g_pathToSmallBasicTextFileZipSource =
        RaiiStringCreateFromCString(g_pFullPathToBenchmarkTestFiles);
    RaiiStringAppend_cString(&g_pathToSmallBasicTextFileZipSource,
                             "/SmallBasicTextFileZip/");

    g_pathToMultiTextFileZipSource =
        RaiiStringCreateFromCString(g_pFullPathToBenchmarkTestFiles);
    RaiiStringAppend_cString(&g_pathToMultiTextFileZipSource,
                             "/MultiTextFileZip/");

    g_pathToMultiTextFileAndSubDirZipSource =
        RaiiStringCreateFromCString(g_pFullPathToBenchmarkTestFiles);
    RaiiStringAppend_cString(&g_pathToMultiTextFileAndSubDirZipSource,
                             "/MultiTextFileAndSubDirZip/");
}

TEST_TEAR_DOWN(TestZipMinizip) {
    RaiiStringClean(&g_pathToSmallBasicTextFileZipDeflate);
    RaiiStringClean(&g_pathToSmallBasicTextFileZipSource);
    RaiiStringClean(&g_pathToMultiTextFileZipSource);
    RaiiStringClean(&g_pathToMultiTextFileAndSubDirZipSource);

    if (PathExists("./tmp/")) {
        TEST_ASSERT_TRUE(RecursiveRmdir("./tmp/"));
    }
}

//==============================
// Zip()
//==============================

TEST(TestZipMinizip, test_Zip_ReturnsErrorIfOutputZipPathIsNullptr) {
    const char *pInputPathArray[] = {
        "123",
        "asdf",
    };

    const ZIP_RETURN_CODES statusZip =
        zip_minizip.Zip(NULL, &pInputPathArray[0], 2);
    TEST_ASSERT_EQUAL(ZIP_ERROR, statusZip);
}

TEST(TestZipMinizip, test_Zip_ReturnsErrorIfInputPathArrayIsNullptr) {
    const char dummyString[] = "123";

    const ZIP_RETURN_CODES statusZip =
        zip_minizip.Zip(&dummyString[0], NULL, 2);
    TEST_ASSERT_EQUAL(ZIP_ERROR, statusZip);
}

TEST(TestZipMinizip, test_Zip_ReturnsErrorIfProvidedSizeIsZero) {
    const char dummyString[] = "123";
    const char *pInputPathArray[] = {
        "123",
        "asdf",
    };

    const ZIP_RETURN_CODES statusZip =
        zip_minizip.Zip(&dummyString[0], &pInputPathArray[0], 0);
    TEST_ASSERT_EQUAL(ZIP_ERROR, statusZip);
}

TEST(TestZipMinizip, test_Zip_ReturnsErrorIfZipFileAlreadyExists) {
    RAII_STRING dummyZipPath = RaiiStringCreateFromCString("./tmp/");
    RecursiveMkdir(dummyZipPath.pString);

    RaiiStringAppend_cString(&dummyZipPath, "SomeZip.zip");
    FILE *pDummyFile = fopen(dummyZipPath.pString, "w");
    TEST_ASSERT_NOT_NULL(pDummyFile);
    fclose(pDummyFile);

    const char *pInputPathArray[] = {
        "123",
    };

    const ZIP_RETURN_CODES statusZip =
        zip_minizip.Zip(dummyZipPath.pString, &pInputPathArray[0], 1);
    TEST_ASSERT_EQUAL(ZIP_ERROR, statusZip);
}

TEST(TestZipMinizip,
     test_Zip_CreatesZipFileAfterZippingInCorrectPathProvidingRelativePath) {
    RAII_STRING inputTextFilePath = RaiiStringCreateFromCString(
        g_pathToSmallBasicTextFileZipSource.pString);
    RaiiStringAppend_cString(&inputTextFilePath, "SmallBasicTextFile.txt");

    const char *pInputPathArray[] = {
        inputTextFilePath.pString,
    };

    RAII_STRING outputZipPath =
        RaiiStringCreateFromCString("./tmp/SmallBasicTextFile.zip");

    TEST_ASSERT_FALSE(PathExists(outputZipPath.pString));

    const ZIP_RETURN_CODES statusZip =
        zip_minizip.Zip(outputZipPath.pString, &pInputPathArray[0], 1);
    TEST_ASSERT_EQUAL(ZIP_SUCCESS, statusZip);

    TEST_ASSERT_TRUE(PathExists(outputZipPath.pString));
}

TEST(TestZipMinizip,
     test_Zip_CreatesZipFileAfterZippingInCorrectPathProvidingAbsolutePath) {
    RAII_STRING inputTextFilePath = RaiiStringCreateFromCString(
        g_pathToSmallBasicTextFileZipSource.pString);
    RaiiStringAppend_cString(&inputTextFilePath, "SmallBasicTextFile.txt");

    const char *pInputPathArray[] = {
        inputTextFilePath.pString,
    };

    char cwdBuffer[MAX_PATH_LENGTH_WTH_TERMINATOR];
    GetCurrentWorkingDirectory(&cwdBuffer[0], MAX_PATH_LENGTH_WTH_TERMINATOR);

    RAII_STRING outputZipPath = RaiiStringCreateFromCString(&cwdBuffer[0]);
    RaiiStringAppend_cString(&outputZipPath, "tmp/SmallBasicTextFile.zip");

    TEST_ASSERT_FALSE(PathExists(outputZipPath.pString));

    const ZIP_RETURN_CODES statusZip =
        zip_minizip.Zip(outputZipPath.pString, &pInputPathArray[0], 1);
    TEST_ASSERT_EQUAL(ZIP_SUCCESS, statusZip);

    TEST_ASSERT_TRUE(PathExists(outputZipPath.pString));
}

//==============================
// TEST_GROUP_RUNNER
//==============================

TEST_GROUP_RUNNER(TestZipMinizip) {
    // Zip()
    RUN_TEST_CASE(TestZipMinizip,
                  test_Zip_ReturnsErrorIfOutputZipPathIsNullptr);
    RUN_TEST_CASE(TestZipMinizip,
                  test_Zip_ReturnsErrorIfInputPathArrayIsNullptr);
    RUN_TEST_CASE(TestZipMinizip, test_Zip_ReturnsErrorIfProvidedSizeIsZero);
    RUN_TEST_CASE(TestZipMinizip, test_Zip_ReturnsErrorIfZipFileAlreadyExists);
    RUN_TEST_CASE(
        TestZipMinizip,
        test_Zip_CreatesZipFileAfterZippingInCorrectPathProvidingRelativePath);
    RUN_TEST_CASE(
        TestZipMinizip,
        test_Zip_CreatesZipFileAfterZippingInCorrectPathProvidingAbsolutePath);
}
