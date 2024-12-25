#include "TestUnZipMinizipInflateZlib.h"
#include "unity_fixture.h"
#include <CoDeLib/FileUtils/FileUtils.h>
#include <CoDeLib/Inflate_zlib/Inflate_zlib.h>
#include <CoDeLib/RaiiString/RaiiString.h>
#include <CoDeLib/UnZip_minizip/UnZip_minizip.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static char *g_pFullPathToBenchmarkTestFiles;

void SetupTestUnZipMinizipInflateZlib(char *pFullPathToBenchmarkTestFiles) {
    g_pFullPathToBenchmarkTestFiles = pFullPathToBenchmarkTestFiles;
}

TEST_GROUP(TestUnZipMinizipInflateZlib);

static RaiiString g_someUnZippedDirPath;
static RaiiString g_someZipPath;
static RaiiString g_pathToSmallBasicTextFileZipDeflate;
static RaiiString g_pathToSmallBasicTextFileZipSource;
static RaiiString g_pathToMultiTextFileZipDeflate;
static RaiiString g_pathToMultiTextFileZipSource;
static RaiiString g_pathToMultiTextFileAndSubDirZipDeflate;
static RaiiString g_pathToMultiTextFileAndSubDirZipSource;

TEST_SETUP(TestUnZipMinizipInflateZlib) {
    g_someUnZippedDirPath =
        RaiiStringCreateFromCString("SomePath/someUnZippedDirPath.zip");
    g_someZipPath = RaiiStringCreateFromCString("SomePath/someZipPath.zip");

    g_pathToSmallBasicTextFileZipDeflate =
        RaiiStringCreateFromCString(g_pFullPathToBenchmarkTestFiles);
    RaiiStringAppend_cString(&g_pathToSmallBasicTextFileZipDeflate,
                             "/SmallBasicTextFileZip/"
                             "SmallBasicTextFile_deflate.zip");

    g_pathToSmallBasicTextFileZipSource =
        RaiiStringCreateFromCString(g_pFullPathToBenchmarkTestFiles);
    RaiiStringAppend_cString(&g_pathToSmallBasicTextFileZipSource,
                             "/SmallBasicTextFileZip/");

    g_pathToMultiTextFileZipDeflate =
        RaiiStringCreateFromCString(g_pFullPathToBenchmarkTestFiles);
    RaiiStringAppend_cString(&g_pathToMultiTextFileZipDeflate,
                             "/MultiTextFileZip/"
                             "MultiTextFileZip_deflate.zip");

    g_pathToMultiTextFileZipSource =
        RaiiStringCreateFromCString(g_pFullPathToBenchmarkTestFiles);
    RaiiStringAppend_cString(&g_pathToMultiTextFileZipSource,
                             "/MultiTextFileZip/");

    g_pathToMultiTextFileAndSubDirZipDeflate =
        RaiiStringCreateFromCString(g_pFullPathToBenchmarkTestFiles);
    RaiiStringAppend_cString(&g_pathToMultiTextFileAndSubDirZipDeflate,
                             "/MultiTextFileAndSubDirZip/"
                             "MultiTextFileAndSubDirZip_deflate.zip");

    g_pathToMultiTextFileAndSubDirZipSource =
        RaiiStringCreateFromCString(g_pFullPathToBenchmarkTestFiles);
    RaiiStringAppend_cString(&g_pathToMultiTextFileAndSubDirZipSource,
                             "/MultiTextFileAndSubDirZip/");
}

TEST_TEAR_DOWN(TestUnZipMinizipInflateZlib) {
    RaiiStringClean(&g_someUnZippedDirPath);
    RaiiStringClean(&g_someZipPath);
    RaiiStringClean(&g_pathToSmallBasicTextFileZipDeflate);
    RaiiStringClean(&g_pathToSmallBasicTextFileZipSource);
    RaiiStringClean(&g_pathToMultiTextFileZipDeflate);
    RaiiStringClean(&g_pathToMultiTextFileZipSource);
    RaiiStringClean(&g_pathToMultiTextFileAndSubDirZipDeflate);
    RaiiStringClean(&g_pathToMultiTextFileAndSubDirZipSource);

    if (PathExists("./tmp/")) {
        TEST_ASSERT_TRUE(RecursiveRmdir("./tmp/"));
    }
}

//==============================
// UnZip() + Inflate()
//==============================

TEST(TestUnZipMinizipInflateZlib,
     test_UnZipAndInflateSingleFileZipWritesDataCorrectlyToTheFile) {
    RAII_ZIPCONTENTINFO zipInfo =
        ZipContentInfoCreate(&g_pathToSmallBasicTextFileZipDeflate);

    RAII_STRING pathToUnzippedFile = RaiiStringCreateFromCString(
        "./tmp/SmallBasicTextFile_deflate_unZipped/");

    const UNZIP_RETURN_CODES statusUnzip =
        unzip_minizip.UnZip(&zipInfo, &pathToUnzippedFile);
    TEST_ASSERT_EQUAL(UNZIP_SUCCESS, statusUnzip);

    RAII_STRING pathToUnzippedFilesWithFile =
        RaiiStringCreateFromCString(pathToUnzippedFile.pString);
    RaiiStringAppend_cString(&pathToUnzippedFilesWithFile,
                             "SmallBasicTextFile.txt");

    RAII_STRING pathToUnzippedInflatedFilesWithFile =
        RaiiStringCreateFromCString(pathToUnzippedFile.pString);
    RaiiStringAppend_cString(&pathToUnzippedInflatedFilesWithFile,
                             "SmallBasicTextFile.decompressed.txt");

    RAII_STRING pathToSourceWithFilesWithFile = RaiiStringCreateFromCString(
        g_pathToSmallBasicTextFileZipSource.pString);
    RaiiStringAppend_cString(&pathToSourceWithFilesWithFile,
                             "SmallBasicTextFile.txt");

    FILE *pFileUnzipped = NULL;
    FILE *pFileUnzippedInflated = NULL;

    OpenFileWithMode(&pFileUnzipped, &pathToUnzippedFilesWithFile, "rb");

    // Note: It is important to open the file in binary mode.
    // Otherwise, a \r (cariage return) character will implicitely
    // add a \n (newline) character.
    OpenFileWithMode(&pFileUnzippedInflated,
                     &pathToUnzippedInflatedFilesWithFile, "w+b");

    const INFLATE_RETURN_CODES statusInflate =
        inflate_zlib.Inflate(pFileUnzipped, pFileUnzippedInflated, NULL);
    TEST_ASSERT_EQUAL(INFLATE_SUCCESS, statusInflate);

    fclose(pFileUnzipped);

    FILE *pFileSource = NULL;
    OpenFileWithMode(&pFileSource, &pathToSourceWithFilesWithFile, "rb");

    TEST_ASSERT_TRUE(FilesAreEqual(pFileSource, pFileUnzippedInflated));

    fclose(pFileSource);
    fclose(pFileUnzippedInflated);
}

TEST(TestUnZipMinizipInflateZlib,
     test_UnZipAndInflateMultiFileZipWritesDataCorrectlyToTheFiles) {
    RAII_ZIPCONTENTINFO zipInfo =
        ZipContentInfoCreate(&g_pathToMultiTextFileZipDeflate);

    RAII_STRING pathToUnzippedFiles =
        RaiiStringCreateFromCString("./tmp/MultiTextFileZip_deflate_unZipped/");

    const UNZIP_RETURN_CODES statusUnzip =
        unzip_minizip.UnZip(&zipInfo, &pathToUnzippedFiles);
    TEST_ASSERT_EQUAL(UNZIP_SUCCESS, statusUnzip);

    RaiiString pExpectedPaths[] = {
        RaiiStringCreateFromCString("TextFileOne.txt"),
        RaiiStringCreateFromCString("TextFileTwo.txt"),
        RaiiStringCreateFromCString("ThirdTextFile.txt"),
    };
    const size_t expectedFileCount =
        sizeof(pExpectedPaths) / sizeof(pExpectedPaths[0]);

    RaiiString pPathsToDecompressedFiles[] = {
        RaiiStringCreateFromCString("TextFileOne.decompressed.txt"),
        RaiiStringCreateFromCString("TextFileTwo.decompressed.txt"),
        RaiiStringCreateFromCString("ThirdTextFile.decompressed.txt"),
    };

    for (size_t i = 0; i < expectedFileCount; ++i) {

        RAII_STRING pathToUnzippedFilesWithFile =
            RaiiStringCreateFromCString(pathToUnzippedFiles.pString);
        RaiiStringAppend_cString(&pathToUnzippedFilesWithFile,
                                 pExpectedPaths[i].pString);

        RAII_STRING pathToUnzippedInflatedFilesWithFile =
            RaiiStringCreateFromCString(pathToUnzippedFiles.pString);
        RaiiStringAppend_cString(&pathToUnzippedInflatedFilesWithFile,
                                 pPathsToDecompressedFiles[i].pString);

        RAII_STRING pathToSourceWithFilesWithFile =
            RaiiStringCreateFromCString(g_pathToMultiTextFileZipSource.pString);
        RaiiStringAppend_cString(&pathToSourceWithFilesWithFile,
                                 pExpectedPaths[i].pString);

        FILE *pFileUnzipped = NULL;
        FILE *pFileUnzippedInflated = NULL;

        OpenFileWithMode(&pFileUnzipped, &pathToUnzippedFilesWithFile, "rb");

        // Note: It is important to open the file in binary mode.
        // Otherwise, a \r (cariage return) character will implicitely
        // add a \n (newline) character.
        OpenFileWithMode(&pFileUnzippedInflated,
                         &pathToUnzippedInflatedFilesWithFile, "w+b");

        const INFLATE_RETURN_CODES statusInflate =
            inflate_zlib.Inflate(pFileUnzipped, pFileUnzippedInflated, NULL);
        TEST_ASSERT_EQUAL(INFLATE_SUCCESS, statusInflate);

        fclose(pFileUnzipped);

        FILE *pFileSource = NULL;
        OpenFileWithMode(&pFileSource, &pathToSourceWithFilesWithFile, "rb");

        TEST_ASSERT_TRUE(FilesAreEqual(pFileSource, pFileUnzippedInflated));

        fclose(pFileSource);
        fclose(pFileUnzippedInflated);
    }

    for (size_t i = 0; i < expectedFileCount; ++i) {
        RaiiStringClean(&pExpectedPaths[i]);
        RaiiStringClean(&pPathsToDecompressedFiles[i]);
    }
}

TEST(TestUnZipMinizipInflateZlib,
     test_UnZipAndInflateMultiFileAndSubDirZipWritesDataCorrectlyToTheFiles) {
    RAII_ZIPCONTENTINFO zipInfo =
        ZipContentInfoCreate(&g_pathToMultiTextFileAndSubDirZipDeflate);

    RAII_STRING pathToUnzippedFiles = RaiiStringCreateFromCString(
        "./tmp/MultiTextFileAndSubDirZip_deflate_unZipped/");

    const UNZIP_RETURN_CODES statusUnzip =
        unzip_minizip.UnZip(&zipInfo, &pathToUnzippedFiles);
    TEST_ASSERT_EQUAL(UNZIP_SUCCESS, statusUnzip);

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

    RaiiString pPathsToDecompressedFiles[] = {
        RaiiStringCreateFromCString("DirFileOne/TextFileOne.decompressed.txt"),
        RaiiStringCreateFromCString(
            "DirOfDirs/OtherDir/DummyFile.decompressed.txt"),
        RaiiStringCreateFromCString(
            "DirOfDirs/OtherDir/FilleInDirectory.decompressed.txt"),
        RaiiStringCreateFromCString(
            "DirOfDirs/SubDirWithCopyOfTopLevelFile/TextFile.decompressed.txt"),
        RaiiStringCreateFromCString("TextFile.decompressed.txt"),
    };

    for (size_t i = 0; i < expectedFileCount; ++i) {

        RAII_STRING pathToUnzippedFilesWithFile =
            RaiiStringCreateFromCString(pathToUnzippedFiles.pString);
        RaiiStringAppend_cString(&pathToUnzippedFilesWithFile,
                                 pExpectedPaths[i].pString);

        RAII_STRING pathToUnzippedInflatedFilesWithFile =
            RaiiStringCreateFromCString(pathToUnzippedFiles.pString);
        RaiiStringAppend_cString(&pathToUnzippedInflatedFilesWithFile,
                                 pPathsToDecompressedFiles[i].pString);

        RAII_STRING pathToSourceWithFilesWithFile = RaiiStringCreateFromCString(
            g_pathToMultiTextFileAndSubDirZipSource.pString);
        RaiiStringAppend_cString(&pathToSourceWithFilesWithFile,
                                 pExpectedPaths[i].pString);

        FILE *pFileUnzipped = NULL;
        FILE *pFileUnzippedInflated = NULL;

        OpenFileWithMode(&pFileUnzipped, &pathToUnzippedFilesWithFile, "rb");

        // Note: It is important to open the file in binary mode.
        // Otherwise, a \r (cariage return) character will implicitely
        // add a \n (newline) character.
        OpenFileWithMode(&pFileUnzippedInflated,
                         &pathToUnzippedInflatedFilesWithFile, "w+b");

        const INFLATE_RETURN_CODES statusInflate =
            inflate_zlib.Inflate(pFileUnzipped, pFileUnzippedInflated, NULL);
        TEST_ASSERT_EQUAL(INFLATE_SUCCESS, statusInflate);

        fclose(pFileUnzipped);

        FILE *pFileSource = NULL;
        OpenFileWithMode(&pFileSource, &pathToSourceWithFilesWithFile, "rb");

        TEST_ASSERT_TRUE(FilesAreEqual(pFileSource, pFileUnzippedInflated));

        fclose(pFileSource);
        fclose(pFileUnzippedInflated);
    }

    for (size_t i = 0; i < expectedFileCount; ++i) {
        RaiiStringClean(&pExpectedPaths[i]);
        RaiiStringClean(&pPathsToDecompressedFiles[i]);
    }
}

//==============================
// TEST_GROUP_RUNNER
//==============================

TEST_GROUP_RUNNER(TestUnZipMinizipInflateZlib) {
    RUN_TEST_CASE(
        TestUnZipMinizipInflateZlib,
        test_UnZipAndInflateSingleFileZipWritesDataCorrectlyToTheFile);
    RUN_TEST_CASE(
        TestUnZipMinizipInflateZlib,
        test_UnZipAndInflateMultiFileZipWritesDataCorrectlyToTheFiles);
    RUN_TEST_CASE(
        TestUnZipMinizipInflateZlib,
        test_UnZipAndInflateMultiFileAndSubDirZipWritesDataCorrectlyToTheFiles);
}
