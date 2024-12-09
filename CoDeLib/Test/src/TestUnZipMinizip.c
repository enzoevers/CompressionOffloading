#include "unity_fixture.h"
#include <CoDeLib/RaiiString/RaiiString.h>
#include <CoDeLib/Test/Utility/FileUtils.h>
#include <CoDeLib/UnZip_minizip/UnZip_minizip.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static char *g_pFullPathToBenchmarkTestFiles;

void SetupTestUnZipMinizip(char *pFullPathToBenchmarkTestFiles) {
    g_pFullPathToBenchmarkTestFiles = pFullPathToBenchmarkTestFiles;
}

TEST_GROUP(TestUnZipMinizip);

static RaiiString g_someUnZippedDirPath;
static RaiiString g_someZipPath;
static RaiiString g_pathToSmallBasicTextFileZip;
static RaiiString g_pathToMultiTextFileZip;
static RaiiString g_pathToMultiTextFileAndSubDirZip;

TEST_SETUP(TestUnZipMinizip) {
    g_someUnZippedDirPath =
        RaiiStringCreateFromCString("SomePath/someUnZippedDirPath.zip");
    g_someZipPath = RaiiStringCreateFromCString("SomePath/someZipPath.zip");

    g_pathToSmallBasicTextFileZip =
        RaiiStringCreateFromCString(g_pFullPathToBenchmarkTestFiles);
    RaiiStringAppend_cString(
        &g_pathToSmallBasicTextFileZip,
        "/SmallBasicTextFileZip_store/SmallBasicTextFile_store.zip");

    g_pathToMultiTextFileZip =
        RaiiStringCreateFromCString(g_pFullPathToBenchmarkTestFiles);
    RaiiStringAppend_cString(
        &g_pathToMultiTextFileZip,
        "/MultiTextFileZip_store/MultiTextFileZip_store.zip");

    g_pathToMultiTextFileAndSubDirZip =
        RaiiStringCreateFromCString(g_pFullPathToBenchmarkTestFiles);
    RaiiStringAppend_cString(
        &g_pathToMultiTextFileAndSubDirZip,
        "/MultiTextFileAndSubDirZip_store/MultiTextFileAndSubDirZip_store.zip");
}

TEST_TEAR_DOWN(TestUnZipMinizip) {
    RaiiStringClean(&g_someUnZippedDirPath);
    RaiiStringClean(&g_someZipPath);
    RaiiStringClean(&g_pathToSmallBasicTextFileZip);
    RaiiStringClean(&g_pathToMultiTextFileZip);
    RaiiStringClean(&g_pathToMultiTextFileAndSubDirZip);
}

//==============================
// UnZip()
//==============================

TEST(TestUnZipMinizip, test_UnZip_ReturnsUnZipErrorIfZipFileInfoPtrIsNull) {

    UNZIP_RETURN_CODES result =
        unzip_minizip.UnZip(NULL, &g_someUnZippedDirPath);
    TEST_ASSERT_EQUAL(UNZIP_ERROR, result);
}

TEST(TestUnZipMinizip, test_UnZip_ReturnsUnZipErrorIfZipFileStrPtrIsNull) {
    RAII_ZIPCONTENTINFO zipInfo =
        (ZipContentInfo){.zipFilePath = (RaiiString){NULL, 0},
                         .pUnZippedFilePathArray = NULL,
                         .unZippedFileCount = 0};

    UNZIP_RETURN_CODES result =
        unzip_minizip.UnZip(&zipInfo, &g_someUnZippedDirPath);

    TEST_ASSERT_EQUAL(UNZIP_ERROR, result);
}

TEST(TestUnZipMinizip, test_UnZip_ReturnsUnZipErrorIfDstPathPtrIsNull) {
    RAII_ZIPCONTENTINFO zipInfo =
        ZipContentInfoCreate(&g_pathToSmallBasicTextFileZip);

    UNZIP_RETURN_CODES result = unzip_minizip.UnZip(&zipInfo, NULL);
    TEST_ASSERT_EQUAL(UNZIP_ERROR, result);
}

TEST(TestUnZipMinizip, test_UnZip_ReturnsUnZipErrorIfDstPathStrPtrIsNull) {
    RAII_ZIPCONTENTINFO zipInfo =
        ZipContentInfoCreate(&g_pathToSmallBasicTextFileZip);

    RAII_STRING unZippedDirPathNull = {NULL, 0};

    UNZIP_RETURN_CODES result =
        unzip_minizip.UnZip(&zipInfo, &unZippedDirPathNull);
    TEST_ASSERT_EQUAL(UNZIP_ERROR, result);
}

TEST(TestUnZipMinizip, test_UnZip_ReturnsUnZipErrorIfZipFileDoesNotExist) {
    RAII_ZIPCONTENTINFO zipInfo = ZipContentInfoCreate(&g_someZipPath);
    UNZIP_RETURN_CODES result =
        unzip_minizip.UnZip(&zipInfo, &g_someUnZippedDirPath);
    TEST_ASSERT_EQUAL(UNZIP_ERROR, result);
}

TEST(TestUnZipMinizip,
     test_UnZip_UnZipSingleFileZipGivesCorrectFileInZipInZipContentInfo) {

    RAII_ZIPCONTENTINFO zipInfo =
        ZipContentInfoCreate(&g_pathToSmallBasicTextFileZip);

    RAII_STRING pathToUnzippedFiles =
        RaiiStringCreateFromCString("./tmp/SmallBasicTextFile_store_unZipped/");

    unzip_minizip.UnZip(&zipInfo, &pathToUnzippedFiles);

    TEST_ASSERT_EQUAL(1, zipInfo.unZippedFileCount);
    TEST_ASSERT_NOT_NULL(zipInfo.pUnZippedFilePathArray);
    TEST_ASSERT_EQUAL_STRING(
        "./tmp/SmallBasicTextFile_store_unZipped/SmallBasicTextFile.txt",
        zipInfo.pUnZippedFilePathArray[0].pString);
}

TEST(TestUnZipMinizip,
     test_UnZip_UnZipAddsTrailingForwardslashToOutputDirPathIfNotPresent) {

    RAII_ZIPCONTENTINFO zipInfo =
        ZipContentInfoCreate(&g_pathToSmallBasicTextFileZip);

    RAII_STRING pathToUnzippedFiles =
        RaiiStringCreateFromCString("./tmp/SmallBasicTextFile_store_unZipped");

    unzip_minizip.UnZip(&zipInfo, &pathToUnzippedFiles);

    TEST_ASSERT_EQUAL(1, zipInfo.unZippedFileCount);
    TEST_ASSERT_NOT_NULL(zipInfo.pUnZippedFilePathArray);
    TEST_ASSERT_EQUAL_STRING(
        "./tmp/SmallBasicTextFile_store_unZipped/SmallBasicTextFile.txt",
        zipInfo.pUnZippedFilePathArray[0].pString);
}

TEST(TestUnZipMinizip,
     test_UnZip_UnZipMultiFileZipGivesCorrectFilesInZipInZipContentInfo) {

    RAII_ZIPCONTENTINFO zipInfo =
        ZipContentInfoCreate(&g_pathToMultiTextFileZip);

    RAII_STRING pathToUnzippedFiles =
        RaiiStringCreateFromCString("./tmp/MultiTextFileZip_store_unZipped/");

    RaiiString pExpectedPaths[] = {
        RaiiStringCreateFromCString(pathToUnzippedFiles.pString),
        RaiiStringCreateFromCString(pathToUnzippedFiles.pString),
        RaiiStringCreateFromCString(pathToUnzippedFiles.pString),
    };

    RaiiStringAppend_cString(&pExpectedPaths[0], "TextFileOne.txt");
    RaiiStringAppend_cString(&pExpectedPaths[1], "TextFileTwo.txt");
    RaiiStringAppend_cString(&pExpectedPaths[2], "ThirdTextFile.txt");

    const size_t expectedFileCount = 3;

    unzip_minizip.UnZip(&zipInfo, &pathToUnzippedFiles);

    TEST_ASSERT_EQUAL(expectedFileCount, zipInfo.unZippedFileCount);
    TEST_ASSERT_NOT_NULL(zipInfo.pUnZippedFilePathArray);
    for (size_t i = 0; i < expectedFileCount; ++i) {
        TEST_ASSERT_EQUAL_STRING(pExpectedPaths[i].pString,
                                 zipInfo.pUnZippedFilePathArray[i].pString);
    }

    for (size_t i = 0; i < expectedFileCount; ++i) {
        RaiiStringClean(&pExpectedPaths[i]);
    }
}

TEST(
    TestUnZipMinizip,
    test_UnZip_UnZipMultiFileAndSubDirZipGivesCorrectFilesInZipInZipContentInfo) {

    RAII_ZIPCONTENTINFO zipInfo =
        ZipContentInfoCreate(&g_pathToMultiTextFileAndSubDirZip);

    RAII_STRING pathToUnzippedFiles = RaiiStringCreateFromCString(
        "./tmp/MultiTextFileAndSubDirZip_store_unZipped/");

    RaiiString pExpectedPaths[] = {
        RaiiStringCreateFromCString(pathToUnzippedFiles.pString),
        RaiiStringCreateFromCString(pathToUnzippedFiles.pString),
        RaiiStringCreateFromCString(pathToUnzippedFiles.pString),
        RaiiStringCreateFromCString(pathToUnzippedFiles.pString),
        RaiiStringCreateFromCString(pathToUnzippedFiles.pString),
        RaiiStringCreateFromCString(pathToUnzippedFiles.pString),
        RaiiStringCreateFromCString(pathToUnzippedFiles.pString),
        RaiiStringCreateFromCString(pathToUnzippedFiles.pString),
        RaiiStringCreateFromCString(pathToUnzippedFiles.pString),
    };

    RaiiStringAppend_cString(&pExpectedPaths[0], "DirFileOne/");
    RaiiStringAppend_cString(&pExpectedPaths[1], "DirFileOne/TextFileOne.txt");
    RaiiStringAppend_cString(&pExpectedPaths[2], "DirOfDirs/");
    RaiiStringAppend_cString(&pExpectedPaths[3], "DirOfDirs/OtherDir/");
    RaiiStringAppend_cString(&pExpectedPaths[4],
                             "DirOfDirs/OtherDir/DummyFile.txt");
    RaiiStringAppend_cString(&pExpectedPaths[5],
                             "DirOfDirs/OtherDir/FilleInDirectory.txt");
    RaiiStringAppend_cString(&pExpectedPaths[6],
                             "DirOfDirs/SubDirWithCopyOfTopLevelFile/");
    RaiiStringAppend_cString(
        &pExpectedPaths[7],
        "DirOfDirs/SubDirWithCopyOfTopLevelFile/TextFile.txt");
    RaiiStringAppend_cString(&pExpectedPaths[8], "TextFile.txt");

    const size_t expectedFileCount = 9;

    unzip_minizip.UnZip(&zipInfo, &pathToUnzippedFiles);

    TEST_ASSERT_EQUAL(expectedFileCount, zipInfo.unZippedFileCount);
    TEST_ASSERT_NOT_NULL(zipInfo.pUnZippedFilePathArray);
    for (size_t i = 0; i < expectedFileCount; ++i) {
        TEST_ASSERT_EQUAL_STRING(pExpectedPaths[i].pString,
                                 zipInfo.pUnZippedFilePathArray[i].pString);
    }

    for (size_t i = 0; i < expectedFileCount; ++i) {
        RaiiStringClean(&pExpectedPaths[i]);
    }
}

//==============================
// TEST_GROUP_RUNNER
//==============================

TEST_GROUP_RUNNER(TestUnZipMinizip) {
    // UnZip()
    RUN_TEST_CASE(TestUnZipMinizip,
                  test_UnZip_ReturnsUnZipErrorIfZipFileInfoPtrIsNull);
    RUN_TEST_CASE(TestUnZipMinizip,
                  test_UnZip_ReturnsUnZipErrorIfZipFileStrPtrIsNull);

    RUN_TEST_CASE(TestUnZipMinizip,
                  test_UnZip_ReturnsUnZipErrorIfDstPathPtrIsNull);
    RUN_TEST_CASE(TestUnZipMinizip,
                  test_UnZip_ReturnsUnZipErrorIfDstPathStrPtrIsNull);
    RUN_TEST_CASE(TestUnZipMinizip,
                  test_UnZip_ReturnsUnZipErrorIfZipFileDoesNotExist);
    RUN_TEST_CASE(
        TestUnZipMinizip,
        test_UnZip_UnZipSingleFileZipGivesCorrectFileInZipInZipContentInfo);
    RUN_TEST_CASE(
        TestUnZipMinizip,
        test_UnZip_UnZipAddsTrailingForwardslashToOutputDirPathIfNotPresent);
    RUN_TEST_CASE(
        TestUnZipMinizip,
        test_UnZip_UnZipMultiFileZipGivesCorrectFilesInZipInZipContentInfo);
    RUN_TEST_CASE(
        TestUnZipMinizip,
        test_UnZip_UnZipMultiFileAndSubDirZipGivesCorrectFilesInZipInZipContentInfo);
}
