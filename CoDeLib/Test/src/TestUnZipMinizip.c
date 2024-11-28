#include "TestUnZipMinizip.h"
#include "unity_fixture.h"
#include <CoDeLib/FileUtils/FileUtils.h>
#include <CoDeLib/RaiiString/RaiiString.h>
#include <CoDeLib/UnZip_minizip/UnZip_minizip.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static char *g_pFullPathToBenchmarkTestFiles;

void SetupTestUnZipMinizip(char *pFullPathToBenchmarkTestFiles) {
    g_pFullPathToBenchmarkTestFiles = pFullPathToBenchmarkTestFiles;
}

bool RaiiStringIsInArray(const RaiiString *string, const RaiiString *pArray,
                         const size_t arraySize) {
    for (size_t i = 0; i < arraySize; ++i) {
        if (strcmp(string->pString, pArray[i].pString) == 0) {
            return true;
        }
    }
    return false;
}

TEST_GROUP(TestUnZipMinizip);

static RaiiString g_someUnZippedDirPath;
static RaiiString g_someZipPath;
static RaiiString g_pathToSmallBasicTextFileZipStore;
static RaiiString g_pathToSmallBasicTextFileZipSource;
static RaiiString g_pathToMultiTextFileZipStore;
static RaiiString g_pathToMultiTextFileZipSource;
static RaiiString g_pathToMultiTextFileAndSubDirZipStore;
static RaiiString g_pathToMultiTextFileAndSubDirZipSource;

TEST_SETUP(TestUnZipMinizip) {
    g_someUnZippedDirPath =
        RaiiStringCreateFromCString("SomePath/someUnZippedDirPath.zip");
    g_someZipPath = RaiiStringCreateFromCString("SomePath/someZipPath.zip");

    g_pathToSmallBasicTextFileZipStore =
        RaiiStringCreateFromCString(g_pFullPathToBenchmarkTestFiles);
    RaiiStringAppend_cString(
        &g_pathToSmallBasicTextFileZipStore,
        "/SmallBasicTextFileZip/SmallBasicTextFile_store.zip");

    g_pathToSmallBasicTextFileZipSource =
        RaiiStringCreateFromCString(g_pFullPathToBenchmarkTestFiles);
    RaiiStringAppend_cString(&g_pathToSmallBasicTextFileZipSource,
                             "/SmallBasicTextFileZip/");

    g_pathToMultiTextFileZipStore =
        RaiiStringCreateFromCString(g_pFullPathToBenchmarkTestFiles);
    RaiiStringAppend_cString(&g_pathToMultiTextFileZipStore,
                             "/MultiTextFileZip/MultiTextFileZip_store.zip");

    g_pathToMultiTextFileZipSource =
        RaiiStringCreateFromCString(g_pFullPathToBenchmarkTestFiles);
    RaiiStringAppend_cString(&g_pathToMultiTextFileZipSource,
                             "/MultiTextFileZip/");

    g_pathToMultiTextFileAndSubDirZipStore =
        RaiiStringCreateFromCString(g_pFullPathToBenchmarkTestFiles);
    RaiiStringAppend_cString(
        &g_pathToMultiTextFileAndSubDirZipStore,
        "/MultiTextFileAndSubDirZip/MultiTextFileAndSubDirZip_store.zip");

    g_pathToMultiTextFileAndSubDirZipSource =
        RaiiStringCreateFromCString(g_pFullPathToBenchmarkTestFiles);
    RaiiStringAppend_cString(&g_pathToMultiTextFileAndSubDirZipSource,
                             "/MultiTextFileAndSubDirZip/");
}

TEST_TEAR_DOWN(TestUnZipMinizip) {
    RaiiStringClean(&g_someUnZippedDirPath);
    RaiiStringClean(&g_someZipPath);
    RaiiStringClean(&g_pathToSmallBasicTextFileZipStore);
    RaiiStringClean(&g_pathToSmallBasicTextFileZipSource);
    RaiiStringClean(&g_pathToMultiTextFileZipStore);
    RaiiStringClean(&g_pathToMultiTextFileZipSource);
    RaiiStringClean(&g_pathToMultiTextFileAndSubDirZipStore);
    RaiiStringClean(&g_pathToMultiTextFileAndSubDirZipSource);

    if (PathExists("./tmp/")) {
        TEST_ASSERT_TRUE(RecursiveRmdir("./tmp/"));
    }
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
        ZipContentInfoCreate(&g_pathToSmallBasicTextFileZipStore);

    UNZIP_RETURN_CODES result = unzip_minizip.UnZip(&zipInfo, NULL);
    TEST_ASSERT_EQUAL(UNZIP_ERROR, result);
}

TEST(TestUnZipMinizip, test_UnZip_ReturnsUnZipErrorIfDstPathStrPtrIsNull) {
    RAII_ZIPCONTENTINFO zipInfo =
        ZipContentInfoCreate(&g_pathToSmallBasicTextFileZipStore);

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

TEST(
    TestUnZipMinizip,
    test_UnZip_UnZipSingleFileZipGivesCorrectFileInZipInZipContentInfoAndCreatesFiles) {

    RAII_ZIPCONTENTINFO zipInfo =
        ZipContentInfoCreate(&g_pathToSmallBasicTextFileZipStore);

    RAII_STRING pathToUnzippedFiles =
        RaiiStringCreateFromCString("./tmp/SmallBasicTextFile_store_unZipped/");
    RAII_STRING pathToUnzippedFilesWithFile =
        RaiiStringCreateFromCString(pathToUnzippedFiles.pString);
    RaiiStringAppend_cString(&pathToUnzippedFilesWithFile,
                             "SmallBasicTextFile.txt");

    const UNZIP_RETURN_CODES statusUnzip =
        unzip_minizip.UnZip(&zipInfo, &pathToUnzippedFiles);
    TEST_ASSERT_EQUAL(UNZIP_SUCCESS, statusUnzip);

    TEST_ASSERT_EQUAL(1, zipInfo.unZippedFileCount);
    TEST_ASSERT_NOT_NULL(zipInfo.pUnZippedFilePathArray);
    TEST_ASSERT_EQUAL_STRING(pathToUnzippedFilesWithFile.pString,
                             zipInfo.pUnZippedFilePathArray[0].pString);

    TEST_ASSERT_TRUE(PathExists(pathToUnzippedFilesWithFile.pString));
}

TEST(TestUnZipMinizip,
     test_UnZip_UnZipAddsTrailingForwardslashToOutputDirPathIfNotPresent) {

    RAII_ZIPCONTENTINFO zipInfo =
        ZipContentInfoCreate(&g_pathToSmallBasicTextFileZipStore);

    RAII_STRING pathToUnzippedFiles =
        RaiiStringCreateFromCString("./tmp/SmallBasicTextFile_store_unZipped");
    RAII_STRING pathToUnzippedFilesWithFile =
        RaiiStringCreateFromCString(pathToUnzippedFiles.pString);
    RaiiStringAppend_cString(&pathToUnzippedFilesWithFile,
                             "/SmallBasicTextFile.txt");

    const UNZIP_RETURN_CODES statusUnzip =
        unzip_minizip.UnZip(&zipInfo, &pathToUnzippedFiles);
    TEST_ASSERT_EQUAL(UNZIP_SUCCESS, statusUnzip);

    TEST_ASSERT_EQUAL(1, zipInfo.unZippedFileCount);
    TEST_ASSERT_NOT_NULL(zipInfo.pUnZippedFilePathArray);
    TEST_ASSERT_EQUAL_STRING(pathToUnzippedFilesWithFile.pString,
                             zipInfo.pUnZippedFilePathArray[0].pString);

    TEST_ASSERT_TRUE(PathExists(pathToUnzippedFilesWithFile.pString));
}

TEST(
    TestUnZipMinizip,
    test_UnZip_UnZipMultiFileZipGivesCorrectFilesInZipInZipContentInfoAndCreatesFiles) {

    RAII_ZIPCONTENTINFO zipInfo =
        ZipContentInfoCreate(&g_pathToMultiTextFileZipStore);

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

    const size_t expectedFileCount =
        sizeof(pExpectedPaths) / sizeof(pExpectedPaths[0]);

    const UNZIP_RETURN_CODES statusUnzip =
        unzip_minizip.UnZip(&zipInfo, &pathToUnzippedFiles);
    TEST_ASSERT_EQUAL(UNZIP_SUCCESS, statusUnzip);

    TEST_ASSERT_EQUAL(expectedFileCount, zipInfo.unZippedFileCount);
    TEST_ASSERT_NOT_NULL(zipInfo.pUnZippedFilePathArray);
    for (size_t i = 0; i < expectedFileCount; ++i) {
        TEST_ASSERT_TRUE(RaiiStringIsInArray(&pExpectedPaths[i],
                                             zipInfo.pUnZippedFilePathArray,
                                             zipInfo.unZippedFileCount));
        TEST_ASSERT_TRUE(PathExists(pExpectedPaths[i].pString));
    }

    for (size_t i = 0; i < expectedFileCount; ++i) {
        RaiiStringClean(&pExpectedPaths[i]);
    }
}

TEST(
    TestUnZipMinizip,
    test_UnZip_UnZipMultiFileAndSubDirZipGivesCorrectFilesInZipInZipContentInfoAndCreatesFiles) {

    RAII_ZIPCONTENTINFO zipInfo =
        ZipContentInfoCreate(&g_pathToMultiTextFileAndSubDirZipStore);

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

    const size_t expectedFileCount =
        sizeof(pExpectedPaths) / sizeof(pExpectedPaths[0]);

    const UNZIP_RETURN_CODES statusUnzip =
        unzip_minizip.UnZip(&zipInfo, &pathToUnzippedFiles);
    TEST_ASSERT_EQUAL(UNZIP_SUCCESS, statusUnzip);

    TEST_ASSERT_EQUAL(expectedFileCount, zipInfo.unZippedFileCount);
    TEST_ASSERT_NOT_NULL(zipInfo.pUnZippedFilePathArray);
    for (size_t i = 0; i < expectedFileCount; ++i) {
        TEST_ASSERT_TRUE(RaiiStringIsInArray(&pExpectedPaths[i],
                                             zipInfo.pUnZippedFilePathArray,
                                             zipInfo.unZippedFileCount));
        TEST_ASSERT_TRUE(PathExists(pExpectedPaths[i].pString));
    }

    for (size_t i = 0; i < expectedFileCount; ++i) {
        RaiiStringClean(&pExpectedPaths[i]);
    }
}

TEST(TestUnZipMinizip,
     test_UnZip_UnZipSingleFileZipWritesDataCorrectlyToTheFile) {

    RAII_ZIPCONTENTINFO zipInfo =
        ZipContentInfoCreate(&g_pathToSmallBasicTextFileZipStore);

    RAII_STRING pathToUnzippedFile =
        RaiiStringCreateFromCString("./tmp/SmallBasicTextFile_store_unZipped/");

    RAII_STRING pathToSourceWithFilesWithFile = RaiiStringCreateFromCString(
        g_pathToSmallBasicTextFileZipSource.pString);

    RAII_STRING pathToUnzippedFilesWithFile =
        RaiiStringCreateFromCString(pathToUnzippedFile.pString);
    RaiiStringAppend_cString(&pathToUnzippedFilesWithFile,
                             "SmallBasicTextFile.txt");

    RaiiStringAppend_cString(&pathToSourceWithFilesWithFile,
                             "SmallBasicTextFile.txt");

    const UNZIP_RETURN_CODES statusUnzip =
        unzip_minizip.UnZip(&zipInfo, &pathToUnzippedFile);
    TEST_ASSERT_EQUAL(UNZIP_SUCCESS, statusUnzip);

    FILE *pFileSource = NULL;
    FILE *pFileUnzipped = NULL;

    OpenFileWithMode(&pFileSource, &pathToSourceWithFilesWithFile, "rb");
    OpenFileWithMode(&pFileUnzipped, &pathToUnzippedFilesWithFile, "rb");

    TEST_ASSERT_TRUE(FilesAreEqual(pFileSource, pFileUnzipped));

    fclose(pFileSource);
    fclose(pFileUnzipped);
}

TEST(TestUnZipMinizip,
     test_UnZip_UnZipMultiFileZipWritesDataCorrectlyToTheFiles) {
    RAII_ZIPCONTENTINFO zipInfo =
        ZipContentInfoCreate(&g_pathToMultiTextFileZipStore);

    RaiiString pExpectedPaths[] = {
        RaiiStringCreateFromCString("TextFileOne.txt"),
        RaiiStringCreateFromCString("TextFileTwo.txt"),
        RaiiStringCreateFromCString("ThirdTextFile.txt"),
    };
    const size_t expectedFileCount =
        sizeof(pExpectedPaths) / sizeof(pExpectedPaths[0]);

    RAII_STRING pathToUnzippedFiles =
        RaiiStringCreateFromCString("./tmp/MultiTextFileZip_store_unZipped/");

    const UNZIP_RETURN_CODES statusUnzip =
        unzip_minizip.UnZip(&zipInfo, &pathToUnzippedFiles);
    TEST_ASSERT_EQUAL(UNZIP_SUCCESS, statusUnzip);

    for (size_t i = 0; i < expectedFileCount; ++i) {
        RAII_STRING pathToUnzippedFilesWithFile =
            RaiiStringCreateFromCString(pathToUnzippedFiles.pString);
        RaiiStringAppend_cString(&pathToUnzippedFilesWithFile,
                                 pExpectedPaths[i].pString);

        FILE *pFileSource = NULL;
        FILE *pFileUnzipped = NULL;

        RAII_STRING pathToSourceWithFilesWithFile =
            RaiiStringCreateFromCString(g_pathToMultiTextFileZipSource.pString);
        RaiiStringAppend_cString(&pathToSourceWithFilesWithFile,
                                 pExpectedPaths[i].pString);

        OpenFileWithMode(&pFileSource, &pathToSourceWithFilesWithFile, "rb");
        OpenFileWithMode(&pFileUnzipped, &pathToUnzippedFilesWithFile, "rb");

        TEST_ASSERT_TRUE(FilesAreEqual(pFileSource, pFileUnzipped));

        fclose(pFileSource);
        fclose(pFileUnzipped);
    }

    for (size_t i = 0; i < expectedFileCount; ++i) {
        RaiiStringClean(&pExpectedPaths[i]);
    }
}

TEST(TestUnZipMinizip,
     test_UnZip_UnZipMultiFileAndSubDirZipWritesDataCorrectlyToTheFiles) {
    RAII_ZIPCONTENTINFO zipInfo =
        ZipContentInfoCreate(&g_pathToMultiTextFileAndSubDirZipStore);

    RaiiString pExpectedPaths[] = {
        RaiiStringCreateFromCString("DirFileOne/TextFileOne.txt"),
        RaiiStringCreateFromCString("DirOfDirs/OtherDir/DummyFile.txt"),
        RaiiStringCreateFromCString("DirOfDirs/OtherDir/FilleInDirectory.txt"),
        RaiiStringCreateFromCString(
            "DirOfDirs/SubDirWithCopyOfTopLevelFile/TextFile.txt"),
        RaiiStringCreateFromCString("TextFile.txt"),
    };
    const size_t expectedFileCount =
        sizeof(pExpectedPaths) / sizeof(pExpectedPaths[0]);

    RAII_STRING pathToUnzippedFiles = RaiiStringCreateFromCString(
        "./tmp/MultiTextFileAndSubDirZip_store_unZipped/");

    const UNZIP_RETURN_CODES statusUnzip =
        unzip_minizip.UnZip(&zipInfo, &pathToUnzippedFiles);
    TEST_ASSERT_EQUAL(UNZIP_SUCCESS, statusUnzip);

    for (size_t i = 0; i < expectedFileCount; ++i) {
        RAII_STRING pathToUnzippedFilesWithFile =
            RaiiStringCreateFromCString(pathToUnzippedFiles.pString);
        RaiiStringAppend_cString(&pathToUnzippedFilesWithFile,
                                 pExpectedPaths[i].pString);

        FILE *pFileSource = NULL;
        FILE *pFileUnzipped = NULL;

        RAII_STRING pathToSourceWithFilesWithFile = RaiiStringCreateFromCString(
            g_pathToMultiTextFileAndSubDirZipSource.pString);
        RaiiStringAppend_cString(&pathToSourceWithFilesWithFile,
                                 pExpectedPaths[i].pString);

        OpenFileWithMode(&pFileSource, &pathToSourceWithFilesWithFile, "rb");
        OpenFileWithMode(&pFileUnzipped, &pathToUnzippedFilesWithFile, "rb");

        TEST_ASSERT_TRUE(FilesAreEqual(pFileSource, pFileUnzipped));

        fclose(pFileSource);
        fclose(pFileUnzipped);
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
        test_UnZip_UnZipSingleFileZipGivesCorrectFileInZipInZipContentInfoAndCreatesFiles);
    RUN_TEST_CASE(
        TestUnZipMinizip,
        test_UnZip_UnZipAddsTrailingForwardslashToOutputDirPathIfNotPresent);
    RUN_TEST_CASE(
        TestUnZipMinizip,
        test_UnZip_UnZipMultiFileZipGivesCorrectFilesInZipInZipContentInfoAndCreatesFiles);
    RUN_TEST_CASE(
        TestUnZipMinizip,
        test_UnZip_UnZipMultiFileAndSubDirZipGivesCorrectFilesInZipInZipContentInfoAndCreatesFiles);
    RUN_TEST_CASE(TestUnZipMinizip,
                  test_UnZip_UnZipSingleFileZipWritesDataCorrectlyToTheFile);
    RUN_TEST_CASE(TestUnZipMinizip,
                  test_UnZip_UnZipMultiFileZipWritesDataCorrectlyToTheFiles);
    RUN_TEST_CASE(
        TestUnZipMinizip,
        test_UnZip_UnZipMultiFileAndSubDirZipWritesDataCorrectlyToTheFiles);
}
