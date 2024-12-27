#include "TestZipMinizipUnZipMinizip.h"
#include "unity_fixture.h"
#include <CoDeLib/FileUtils/FileUtils.h>
#include <CoDeLib/RaiiString/RaiiString.h>
#include <CoDeLib/UnZip_minizip/UnZip_minizip.h>
#include <CoDeLib/Zip_minizip/Zip_minizip.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static char *g_pFullPathToBenchmarkTestFiles;
static bool g_runLongTests = false;

void SetupTestZipMinizipUnZipMinizip(char *pFullPathToBenchmarkTestFiles,
                                     bool runLongTests) {
    g_pFullPathToBenchmarkTestFiles = pFullPathToBenchmarkTestFiles;
    g_runLongTests = runLongTests;
}

TEST_GROUP(TestZipMinizipUnZipMinizip);

static RaiiString g_pathToSmallBasicTextFileZipSource;
static RaiiString g_pathToMultiTextFileZipSource;
static RaiiString g_pathToMultiTextFileAndSubDirZipSource;

TEST_SETUP(TestZipMinizipUnZipMinizip) {
    if (PathExists("./tmp/")) {
        TEST_ASSERT_TRUE(RecursiveRmdir("./tmp/"));
    }

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

TEST_TEAR_DOWN(TestZipMinizipUnZipMinizip) {
    RaiiStringClean(&g_pathToSmallBasicTextFileZipSource);
    RaiiStringClean(&g_pathToMultiTextFileZipSource);
    RaiiStringClean(&g_pathToMultiTextFileAndSubDirZipSource);

    if (PathExists("./tmp/")) {
        TEST_ASSERT_TRUE(RecursiveRmdir("./tmp/"));
    }
}

void TestZipMinizipUnZipMinizip_Store(
    const RaiiString *const inputPathArray, const size_t inputPathArraySize,
    const RaiiString *const sourceFilePathArrayToCompare,
    const size_t sourceFilePathArrayToCompareSize,
    const RaiiString *const unZippedFilePathArrayToCompare,
    const size_t unZippedFilePathArrayToCompareSize,
    const RaiiString *const pZippedResultPath,
    const RaiiString *const pUnZippedResultPath) {

    TEST_ASSERT_EQUAL(sourceFilePathArrayToCompareSize,
                      unZippedFilePathArrayToCompareSize);
    //----------
    // Generic setup
    //----------

    const char *charInputPathArray[inputPathArraySize];
    for (size_t i = 0; i < inputPathArraySize; ++i) {
        charInputPathArray[i] = inputPathArray[i].pString;
    }

    //----------
    // Zipping
    //----------

    printf("Zipping\n");
    const ZIP_RETURN_CODES statusZip = zip_minizip.Zip(
        pZippedResultPath->pString, &charInputPathArray[0], inputPathArraySize);

    TEST_ASSERT_EQUAL(ZIP_SUCCESS, statusZip);

    //----------
    // Un-zipping
    //----------

    printf("Unzipping\n");
    RAII_ZIPCONTENTINFO zipInfo = ZipContentInfoCreate(pZippedResultPath);

    const UNZIP_RETURN_CODES statusUnZip =
        unzip_minizip.UnZip(&zipInfo, pUnZippedResultPath);

    TEST_ASSERT_EQUAL(UNZIP_SUCCESS, statusUnZip);

    //----------
    // Check if files are identical
    //----------

    printf("Comparing files\n");
    for (size_t i = 0; i < sourceFilePathArrayToCompareSize; ++i) {
        FILE *pFileSource = NULL;
        FILE *pFileUnzipped = NULL;

        OpenFileWithMode(&pFileSource, &sourceFilePathArrayToCompare[i], "rb");
        OpenFileWithMode(&pFileUnzipped, &unZippedFilePathArrayToCompare[i],
                         "rb");

        TEST_ASSERT_TRUE(FilesAreEqual(pFileSource, pFileUnzipped));

        fclose(pFileSource);
        fclose(pFileUnzipped);
    }

    //----------
    // Clean up
    //----------

    for (size_t i = 0; i < inputPathArraySize; ++i) {
        charInputPathArray[i] = NULL;
    }
}

//==============================
// Zip() + UnZip()
//==============================

TEST(TestZipMinizipUnZipMinizip,
     test_ZippingAndUnzippingRawTextGivesIdenticalOutput_SingleFile) {

    //--------------------
    // Setup
    //--------------------

    // clang-format off

    //----- Input path to zip -----

    const size_t inputPathArraySize = 1;
    RaiiString inputPathArray[inputPathArraySize];

    for (size_t i = 0; i < inputPathArraySize; ++i) {
        inputPathArray[i] = RaiiStringCreateFromCString(g_pathToSmallBasicTextFileZipSource.pString);
    }

    RaiiStringAppend_cString(&inputPathArray[0], "SmallBasicTextFile.txt");

    //----- Source files to compare with -----

    const size_t sourceFilePathArrayToCompareSize = 1;
    RaiiString sourceFilePathArrayToCompare[sourceFilePathArrayToCompareSize];

    for (size_t i = 0; i < sourceFilePathArrayToCompareSize; ++i) {
        sourceFilePathArrayToCompare[i] = RaiiStringCreateFromCString(g_pathToSmallBasicTextFileZipSource.pString);
    }

    RaiiStringAppend_cString(&sourceFilePathArrayToCompare[0], "SmallBasicTextFile.txt");

    //----- Unzipped files to compare with -----

    RAII_STRING zippedResultPath = RaiiStringCreateFromCString("./tmp/out.zip");
    RAII_STRING unZippedResultPath = RaiiStringCreateFromCString("./tmp/out_unzipped/");

    const size_t unZippedFilePathArrayToCompareSize = sourceFilePathArrayToCompareSize;
    RaiiString unZippedFilePathArrayToCompare[unZippedFilePathArrayToCompareSize];

    for (size_t i = 0; i < unZippedFilePathArrayToCompareSize; ++i) {
        unZippedFilePathArrayToCompare[i] = RaiiStringCreateFromCString(unZippedResultPath.pString);
    }

    RaiiStringAppend_cString(&unZippedFilePathArrayToCompare[0], "SmallBasicTextFile.txt");

    // clang-format on

    //--------------------
    // Test
    //--------------------

    TestZipMinizipUnZipMinizip_Store(
        &inputPathArray[0], inputPathArraySize,
        &sourceFilePathArrayToCompare[0], sourceFilePathArrayToCompareSize,
        &unZippedFilePathArrayToCompare[0], unZippedFilePathArrayToCompareSize,
        &zippedResultPath, &unZippedResultPath);

    //--------------------
    // Cleanup
    //--------------------

    for (size_t i = 0; i < inputPathArraySize; ++i) {
        RaiiStringClean(&inputPathArray[i]);
    }

    for (size_t i = 0; i < sourceFilePathArrayToCompareSize; ++i) {
        RaiiStringClean(&sourceFilePathArrayToCompare[i]);
    }

    for (size_t i = 0; i < unZippedFilePathArrayToCompareSize; ++i) {
        RaiiStringClean(&unZippedFilePathArrayToCompare[i]);
    }
}

TEST(TestZipMinizipUnZipMinizip,
     test_ZippingAndUnzippingRawTextGivesIdenticalOutput_MultiFile) {

    //--------------------
    // Setup
    //--------------------

    // clang-format off

    //----- Input path to zip -----

    const size_t inputPathArraySize = 3;
    RaiiString inputPathArray[inputPathArraySize];

    for (size_t i = 0; i < inputPathArraySize; ++i) {
        inputPathArray[i] = RaiiStringCreateFromCString(g_pathToMultiTextFileZipSource.pString);
    }

    RaiiStringAppend_cString(&inputPathArray[0], "TextFileOne.txt");
    RaiiStringAppend_cString(&inputPathArray[1], "TextFileTwo.txt");
    RaiiStringAppend_cString(&inputPathArray[2], "ThirdTextFile.txt");

    //----- Source files to compare with -----

    const size_t sourceFilePathArrayToCompareSize = 3;
    RaiiString sourceFilePathArrayToCompare[sourceFilePathArrayToCompareSize];

    for (size_t i = 0; i < sourceFilePathArrayToCompareSize; ++i) {
        sourceFilePathArrayToCompare[i] = RaiiStringCreateFromCString(g_pathToMultiTextFileZipSource.pString);
    }

    RaiiStringAppend_cString(&sourceFilePathArrayToCompare[0], "TextFileOne.txt");
    RaiiStringAppend_cString(&sourceFilePathArrayToCompare[1], "TextFileTwo.txt");
    RaiiStringAppend_cString(&sourceFilePathArrayToCompare[2], "ThirdTextFile.txt");

    //----- Unzipped files to compare with -----

    RAII_STRING zippedResultPath = RaiiStringCreateFromCString("./tmp/out.zip");
    RAII_STRING unZippedResultPath = RaiiStringCreateFromCString("./tmp/out_unzipped/");

    const size_t unZippedFilePathArrayToCompareSize = sourceFilePathArrayToCompareSize;
    RaiiString unZippedFilePathArrayToCompare[unZippedFilePathArrayToCompareSize];

    for (size_t i = 0; i < unZippedFilePathArrayToCompareSize; ++i) {
        unZippedFilePathArrayToCompare[i] = RaiiStringCreateFromCString(unZippedResultPath.pString);
    }

    RaiiStringAppend_cString(&unZippedFilePathArrayToCompare[0], "TextFileOne.txt");
    RaiiStringAppend_cString(&unZippedFilePathArrayToCompare[1], "TextFileTwo.txt");
    RaiiStringAppend_cString(&unZippedFilePathArrayToCompare[2], "ThirdTextFile.txt");

    // clang-format on

    //--------------------
    // Test
    //--------------------

    TestZipMinizipUnZipMinizip_Store(
        &inputPathArray[0], inputPathArraySize,
        &sourceFilePathArrayToCompare[0], sourceFilePathArrayToCompareSize,
        &unZippedFilePathArrayToCompare[0], unZippedFilePathArrayToCompareSize,
        &zippedResultPath, &unZippedResultPath);

    //--------------------
    // Cleanup
    //--------------------

    for (size_t i = 0; i < inputPathArraySize; ++i) {
        RaiiStringClean(&inputPathArray[i]);
    }

    for (size_t i = 0; i < sourceFilePathArrayToCompareSize; ++i) {
        RaiiStringClean(&sourceFilePathArrayToCompare[i]);
    }

    for (size_t i = 0; i < unZippedFilePathArrayToCompareSize; ++i) {
        RaiiStringClean(&unZippedFilePathArrayToCompare[i]);
    }
}

TEST(TestZipMinizipUnZipMinizip,
     test_ZippingAndUnzippingRawTextGivesIdenticalOutput_MultiFileAndSubDir) {

    //--------------------
    // Setup
    //--------------------

    // clang-format off

    //----- Input path to zip -----

    const size_t inputPathArraySize = 3;
    RaiiString inputPathArray[inputPathArraySize];

    for (size_t i = 0; i < inputPathArraySize; ++i) {
        inputPathArray[i] = RaiiStringCreateFromCString(g_pathToMultiTextFileAndSubDirZipSource.pString);
    }

    RaiiStringAppend_cString(&inputPathArray[0], "DirFileOne/");
    RaiiStringAppend_cString(&inputPathArray[1], "DirOfDirs/");
    RaiiStringAppend_cString(&inputPathArray[2], "TextFile.txt");

    //----- Source files to compare with -----

    const size_t sourceFilePathArrayToCompareSize = 5;
    RaiiString sourceFilePathArrayToCompare[sourceFilePathArrayToCompareSize];

    for (size_t i = 0; i < sourceFilePathArrayToCompareSize; ++i) {
        sourceFilePathArrayToCompare[i] = RaiiStringCreateFromCString(g_pathToMultiTextFileAndSubDirZipSource.pString);
    }

    RaiiStringAppend_cString(&sourceFilePathArrayToCompare[0], "DirFileOne/TextFileOne.txt"),
    RaiiStringAppend_cString(&sourceFilePathArrayToCompare[1], "DirOfDirs/OtherDir/DummyFile.txt");
    RaiiStringAppend_cString(&sourceFilePathArrayToCompare[2], "DirOfDirs/OtherDir/FilleInDirectory.txt");
    RaiiStringAppend_cString(&sourceFilePathArrayToCompare[3], "DirOfDirs/SubDirWithCopyOfTopLevelFile/TextFile.txt");
    RaiiStringAppend_cString(&sourceFilePathArrayToCompare[4], "TextFile.txt");

    //----- Unzipped files to compare with -----

    RAII_STRING zippedResultPath = RaiiStringCreateFromCString("./tmp/out.zip");
    RAII_STRING unZippedResultPath = RaiiStringCreateFromCString("./tmp/out_unzipped/");

    const size_t unZippedFilePathArrayToCompareSize = sourceFilePathArrayToCompareSize;
    RaiiString unZippedFilePathArrayToCompare[unZippedFilePathArrayToCompareSize];

    for (size_t i = 0; i < unZippedFilePathArrayToCompareSize; ++i) {
        unZippedFilePathArrayToCompare[i] = RaiiStringCreateFromCString(unZippedResultPath.pString);
    }

    RaiiStringAppend_cString(&unZippedFilePathArrayToCompare[0], "DirFileOne/TextFileOne.txt"),
    RaiiStringAppend_cString(&unZippedFilePathArrayToCompare[1], "DirOfDirs/OtherDir/DummyFile.txt");
    RaiiStringAppend_cString(&unZippedFilePathArrayToCompare[2], "DirOfDirs/OtherDir/FilleInDirectory.txt");
    RaiiStringAppend_cString(&unZippedFilePathArrayToCompare[3], "DirOfDirs/SubDirWithCopyOfTopLevelFile/TextFile.txt");
    RaiiStringAppend_cString(&unZippedFilePathArrayToCompare[4], "TextFile.txt");

    // clang-format on

    //--------------------
    // Test
    //--------------------

    TestZipMinizipUnZipMinizip_Store(
        &inputPathArray[0], inputPathArraySize,
        &sourceFilePathArrayToCompare[0], sourceFilePathArrayToCompareSize,
        &unZippedFilePathArrayToCompare[0], unZippedFilePathArrayToCompareSize,
        &zippedResultPath, &unZippedResultPath);

    //--------------------
    // Cleanup
    //--------------------

    for (size_t i = 0; i < inputPathArraySize; ++i) {
        RaiiStringClean(&inputPathArray[i]);
    }

    for (size_t i = 0; i < sourceFilePathArrayToCompareSize; ++i) {
        RaiiStringClean(&sourceFilePathArrayToCompare[i]);
    }

    for (size_t i = 0; i < unZippedFilePathArrayToCompareSize; ++i) {
        RaiiStringClean(&unZippedFilePathArrayToCompare[i]);
    }
}

TEST(
    TestZipMinizipUnZipMinizip,
    test_ZippingAndUnzippingRawTextGivesIdenticalOutput_MultiFileAndSubDirDifferentBaseDir) {

    //--------------------
    // Setup
    //--------------------

    // clang-format off

    //----- Input path to zip -----

    const size_t inputPathArraySize = 2;
    RaiiString inputPathArray[inputPathArraySize];

    inputPathArray[0] = RaiiStringCreateFromCString(g_pathToMultiTextFileAndSubDirZipSource.pString);
    inputPathArray[1] = RaiiStringCreateFromCString(g_pathToSmallBasicTextFileZipSource.pString);

    RaiiStringAppend_cString(&inputPathArray[0], "DirOfDirs/");
    RaiiStringAppend_cString(&inputPathArray[1], "SmallBasicTextFile.txt");

    //----- Source files to compare with -----

    const size_t sourceFilePathArrayToCompareSize = 4;
    RaiiString sourceFilePathArrayToCompare[sourceFilePathArrayToCompareSize];

    for (size_t i = 0; i < 3; ++i) {
        sourceFilePathArrayToCompare[i] = RaiiStringCreateFromCString(g_pathToMultiTextFileAndSubDirZipSource.pString);
    }
    sourceFilePathArrayToCompare[3] = RaiiStringCreateFromCString(g_pathToSmallBasicTextFileZipSource.pString);

    RaiiStringAppend_cString(&sourceFilePathArrayToCompare[0], "DirOfDirs/OtherDir/DummyFile.txt");
    RaiiStringAppend_cString(&sourceFilePathArrayToCompare[1], "DirOfDirs/OtherDir/FilleInDirectory.txt");
    RaiiStringAppend_cString(&sourceFilePathArrayToCompare[2], "DirOfDirs/SubDirWithCopyOfTopLevelFile/TextFile.txt");
    RaiiStringAppend_cString(&sourceFilePathArrayToCompare[3], "SmallBasicTextFile.txt");

    //----- Unzipped files to compare with -----

    RAII_STRING zippedResultPath = RaiiStringCreateFromCString("./tmp/out.zip");
    RAII_STRING unZippedResultPath = RaiiStringCreateFromCString("./tmp/out_unzipped/");

    const size_t unZippedFilePathArrayToCompareSize = sourceFilePathArrayToCompareSize;
    RaiiString unZippedFilePathArrayToCompare[unZippedFilePathArrayToCompareSize];

    for (size_t i = 0; i < unZippedFilePathArrayToCompareSize; ++i) {
        unZippedFilePathArrayToCompare[i] = RaiiStringCreateFromCString(unZippedResultPath.pString);
    }

    RaiiStringAppend_cString(&unZippedFilePathArrayToCompare[0], "DirOfDirs/OtherDir/DummyFile.txt");
    RaiiStringAppend_cString(&unZippedFilePathArrayToCompare[1], "DirOfDirs/OtherDir/FilleInDirectory.txt");
    RaiiStringAppend_cString(&unZippedFilePathArrayToCompare[2], "DirOfDirs/SubDirWithCopyOfTopLevelFile/TextFile.txt");
    RaiiStringAppend_cString(&unZippedFilePathArrayToCompare[3], "SmallBasicTextFile.txt");

    // clang-format on

    //--------------------
    // Test
    //--------------------

    TestZipMinizipUnZipMinizip_Store(
        &inputPathArray[0], inputPathArraySize,
        &sourceFilePathArrayToCompare[0], sourceFilePathArrayToCompareSize,
        &unZippedFilePathArrayToCompare[0], unZippedFilePathArrayToCompareSize,
        &zippedResultPath, &unZippedResultPath);

    //--------------------
    // Cleanup
    //--------------------

    for (size_t i = 0; i < inputPathArraySize; ++i) {
        RaiiStringClean(&inputPathArray[i]);
    }

    for (size_t i = 0; i < sourceFilePathArrayToCompareSize; ++i) {
        RaiiStringClean(&sourceFilePathArrayToCompare[i]);
    }

    for (size_t i = 0; i < unZippedFilePathArrayToCompareSize; ++i) {
        RaiiStringClean(&unZippedFilePathArrayToCompare[i]);
    }
}

TEST(
    TestZipMinizipUnZipMinizip,
    test_ZippingAndUnzippingRawTextGivesIdenticalOutput_CanZipAndUnZipFileOf1GB) {

    if (!g_runLongTests) {
        TEST_IGNORE_MESSAGE("Not running because g_runLongTests is false");
    }

    //--------------------
    // Setup
    //--------------------

    //----- Genereate 1GB file -----

    const char *const pRelativePathToTmp = "./tmp/";
    const char *const pFileName = "1GB.txt";
    RAII_STRING fileWith1GB = RaiiStringCreateFromCString(pRelativePathToTmp);
    RaiiStringAppend_cString(&fileWith1GB, pFileName);

    RecursiveMkdir(pRelativePathToTmp);

    FILE *pFile = NULL;
    OpenFileWithMode(&pFile, &fileWith1GB, "wb");

    const uint64_t oneGB = 1000000000;
    const uint64_t bufSize = 5000;

    char buf[bufSize];
    memset(buf, 'a', bufSize - 1);
    buf[bufSize - 1] = '\0';

    printf("Writing 1GB file\n");
    uint64_t bufSizeWritten = 0;
    for (uint64_t i = 0; i < oneGB; i += bufSize) {
        bufSizeWritten += fwrite(buf, 1, bufSize, pFile);
    }
    if (bufSizeWritten != oneGB) {
        printf("Expected (oneGB): %llu\n", (unsigned long long)oneGB);
        printf("Actual (bufSizeWritten): %llu\n",
               (unsigned long long)bufSizeWritten);
        TEST_FAIL_MESSAGE("Bytes written does not match expected size.");
    }

    fclose(pFile);

    // clang-format off

    //----- Input path to zip -----

    const size_t inputPathArraySize = 1;
    RaiiString inputPathArray[inputPathArraySize];

    for (size_t i = 0; i < inputPathArraySize; ++i) {
        inputPathArray[i] = RaiiStringCreateFromCString(pRelativePathToTmp);
    }

    RaiiStringAppend_cString(&inputPathArray[0], pFileName);

    //----- Source files to compare with -----

    const size_t sourceFilePathArrayToCompareSize = 1;
    RaiiString sourceFilePathArrayToCompare[sourceFilePathArrayToCompareSize];

    for (size_t i = 0; i < sourceFilePathArrayToCompareSize; ++i) {
        sourceFilePathArrayToCompare[i] = RaiiStringCreateFromCString(pRelativePathToTmp);
    }

    RaiiStringAppend_cString(&sourceFilePathArrayToCompare[0], pFileName);

    //----- Unzipped files to compare with -----

    RAII_STRING zippedResultPath = RaiiStringCreateFromCString("./tmp/out.zip");
    RAII_STRING unZippedResultPath = RaiiStringCreateFromCString("./tmp/out_unzipped/");

    const size_t unZippedFilePathArrayToCompareSize = sourceFilePathArrayToCompareSize;
    RaiiString unZippedFilePathArrayToCompare[unZippedFilePathArrayToCompareSize];

    for (size_t i = 0; i < unZippedFilePathArrayToCompareSize; ++i) {
        unZippedFilePathArrayToCompare[i] = RaiiStringCreateFromCString(unZippedResultPath.pString);
    }

    RaiiStringAppend_cString(&unZippedFilePathArrayToCompare[0], pFileName);

    // clang-format on

    //--------------------
    // Test
    //--------------------

    TestZipMinizipUnZipMinizip_Store(
        &inputPathArray[0], inputPathArraySize,
        &sourceFilePathArrayToCompare[0], sourceFilePathArrayToCompareSize,
        &unZippedFilePathArrayToCompare[0], unZippedFilePathArrayToCompareSize,
        &zippedResultPath, &unZippedResultPath);

    //--------------------
    // Cleanup
    //--------------------

    for (size_t i = 0; i < inputPathArraySize; ++i) {
        RaiiStringClean(&inputPathArray[i]);
    }

    for (size_t i = 0; i < sourceFilePathArrayToCompareSize; ++i) {
        RaiiStringClean(&sourceFilePathArrayToCompare[i]);
    }

    for (size_t i = 0; i < unZippedFilePathArrayToCompareSize; ++i) {
        RaiiStringClean(&unZippedFilePathArrayToCompare[i]);
    }
}

TEST(
    TestZipMinizipUnZipMinizip,
    test_ZippingAndUnzippingRawTextGivesIdenticalOutput_CanZipAndUnZipFileOf3GB) {

    if (!g_runLongTests) {
        TEST_IGNORE_MESSAGE("Not running because g_runLongTests is false");
    }

    //--------------------
    // Setup
    //--------------------

    //----- Genereate 3GB file -----

    const char *const pRelativePathToTmp = "./tmp/";
    const char *const pFileName = "3GB.txt";
    RAII_STRING fileWith3GB = RaiiStringCreateFromCString(pRelativePathToTmp);
    RaiiStringAppend_cString(&fileWith3GB, pFileName);

    RecursiveMkdir(pRelativePathToTmp);

    FILE *pFile = NULL;
    OpenFileWithMode(&pFile, &fileWith3GB, "wb");

    const uint64_t threeGB = 3000000000;
    const uint64_t bufSize = 5000;

    char buf[bufSize];
    memset(buf, 'a', bufSize - 1);
    buf[bufSize - 1] = '\0';

    printf("Writing 3GB file\n");
    uint64_t bufSizeWritten = 0;
    for (uint64_t i = 0; i < threeGB; i += bufSize) {
        bufSizeWritten += fwrite(buf, 1, bufSize, pFile);
    }
    if (bufSizeWritten != threeGB) {
        printf("Expected (threeGB): %llu\n", (unsigned long long)threeGB);
        printf("Actual (bufSizeWritten): %llu\n",
               (unsigned long long)bufSizeWritten);
        TEST_FAIL_MESSAGE("Bytes written does not match expected size.");
    }

    fclose(pFile);

    // clang-format off

    //----- Input path to zip -----

    const size_t inputPathArraySize = 1;
    RaiiString inputPathArray[inputPathArraySize];

    for (size_t i = 0; i < inputPathArraySize; ++i) {
        inputPathArray[i] = RaiiStringCreateFromCString(pRelativePathToTmp);
    }

    RaiiStringAppend_cString(&inputPathArray[0], pFileName);

    //----- Source files to compare with -----

    const size_t sourceFilePathArrayToCompareSize = 1;
    RaiiString sourceFilePathArrayToCompare[sourceFilePathArrayToCompareSize];

    for (size_t i = 0; i < sourceFilePathArrayToCompareSize; ++i) {
        sourceFilePathArrayToCompare[i] = RaiiStringCreateFromCString(pRelativePathToTmp);
    }

    RaiiStringAppend_cString(&sourceFilePathArrayToCompare[0], pFileName);

    //----- Unzipped files to compare with -----

    RAII_STRING zippedResultPath = RaiiStringCreateFromCString("./tmp/out.zip");
    RAII_STRING unZippedResultPath = RaiiStringCreateFromCString("./tmp/out_unzipped/");

    const size_t unZippedFilePathArrayToCompareSize = sourceFilePathArrayToCompareSize;
    RaiiString unZippedFilePathArrayToCompare[unZippedFilePathArrayToCompareSize];

    for (size_t i = 0; i < unZippedFilePathArrayToCompareSize; ++i) {
        unZippedFilePathArrayToCompare[i] = RaiiStringCreateFromCString(unZippedResultPath.pString);
    }

    RaiiStringAppend_cString(&unZippedFilePathArrayToCompare[0], pFileName);

    // clang-format on

    //--------------------
    // Test
    //--------------------

    TestZipMinizipUnZipMinizip_Store(
        &inputPathArray[0], inputPathArraySize,
        &sourceFilePathArrayToCompare[0], sourceFilePathArrayToCompareSize,
        &unZippedFilePathArrayToCompare[0], unZippedFilePathArrayToCompareSize,
        &zippedResultPath, &unZippedResultPath);

    //--------------------
    // Cleanup
    //--------------------

    for (size_t i = 0; i < inputPathArraySize; ++i) {
        RaiiStringClean(&inputPathArray[i]);
    }

    for (size_t i = 0; i < sourceFilePathArrayToCompareSize; ++i) {
        RaiiStringClean(&sourceFilePathArrayToCompare[i]);
    }

    for (size_t i = 0; i < unZippedFilePathArrayToCompareSize; ++i) {
        RaiiStringClean(&unZippedFilePathArrayToCompare[i]);
    }
}

TEST(
    TestZipMinizipUnZipMinizip,
    test_ZippingAndUnzippingRawTextGivesIdenticalOutput_CanZipAndUnZipFileOf6GB) {

    if (!g_runLongTests) {
        TEST_IGNORE_MESSAGE("Not running because g_runLongTests is false");
    }

    //--------------------
    // Setup
    //--------------------

    //----- Genereate 6GB file -----

    const char *const pRelativePathToTmp = "./tmp/";
    const char *const pFileName = "6GB.txt";
    RAII_STRING fileWith6GB = RaiiStringCreateFromCString(pRelativePathToTmp);
    RaiiStringAppend_cString(&fileWith6GB, pFileName);

    RecursiveMkdir(pRelativePathToTmp);

    FILE *pFile = NULL;
    OpenFileWithMode(&pFile, &fileWith6GB, "wb");

    const uint64_t sixGB = 6000000000;
    const uint64_t bufSize = 5000;

    char buf[bufSize];
    memset(buf, 'a', bufSize - 1);
    buf[bufSize - 1] = '\0';

    printf("Writing 6GB file\n");
    uint64_t bufSizeWritten = 0;
    for (uint64_t i = 0; i < sixGB; i += bufSize) {
        bufSizeWritten += fwrite(buf, 1, bufSize, pFile);
    }
    if (bufSizeWritten != sixGB) {
        printf("Expected (sixGB): %llu\n", (unsigned long long)sixGB);
        printf("Actual (bufSizeWritten): %llu\n",
               (unsigned long long)bufSizeWritten);
        TEST_FAIL_MESSAGE("Bytes written does not match expected size.");
    }

    fclose(pFile);

    // clang-format off

    //----- Input path to zip -----

    const size_t inputPathArraySize = 1;
    RaiiString inputPathArray[inputPathArraySize];

    for (size_t i = 0; i < inputPathArraySize; ++i) {
        inputPathArray[i] = RaiiStringCreateFromCString(pRelativePathToTmp);
    }

    RaiiStringAppend_cString(&inputPathArray[0], pFileName);

    //----- Source files to compare with -----

    const size_t sourceFilePathArrayToCompareSize = 1;
    RaiiString sourceFilePathArrayToCompare[sourceFilePathArrayToCompareSize];

    for (size_t i = 0; i < sourceFilePathArrayToCompareSize; ++i) {
        sourceFilePathArrayToCompare[i] = RaiiStringCreateFromCString(pRelativePathToTmp);
    }

    RaiiStringAppend_cString(&sourceFilePathArrayToCompare[0], pFileName);

    //----- Unzipped files to compare with -----

    RAII_STRING zippedResultPath = RaiiStringCreateFromCString("./tmp/out.zip");
    RAII_STRING unZippedResultPath = RaiiStringCreateFromCString("./tmp/out_unzipped/");

    const size_t unZippedFilePathArrayToCompareSize = sourceFilePathArrayToCompareSize;
    RaiiString unZippedFilePathArrayToCompare[unZippedFilePathArrayToCompareSize];

    for (size_t i = 0; i < unZippedFilePathArrayToCompareSize; ++i) {
        unZippedFilePathArrayToCompare[i] = RaiiStringCreateFromCString(unZippedResultPath.pString);
    }

    RaiiStringAppend_cString(&unZippedFilePathArrayToCompare[0], pFileName);

    // clang-format on

    //--------------------
    // Test
    //--------------------

    TestZipMinizipUnZipMinizip_Store(
        &inputPathArray[0], inputPathArraySize,
        &sourceFilePathArrayToCompare[0], sourceFilePathArrayToCompareSize,
        &unZippedFilePathArrayToCompare[0], unZippedFilePathArrayToCompareSize,
        &zippedResultPath, &unZippedResultPath);

    //--------------------
    // Cleanup
    //--------------------

    for (size_t i = 0; i < inputPathArraySize; ++i) {
        RaiiStringClean(&inputPathArray[i]);
    }

    for (size_t i = 0; i < sourceFilePathArrayToCompareSize; ++i) {
        RaiiStringClean(&sourceFilePathArrayToCompare[i]);
    }

    for (size_t i = 0; i < unZippedFilePathArrayToCompareSize; ++i) {
        RaiiStringClean(&unZippedFilePathArrayToCompare[i]);
    }
}

//==============================
// TEST_GROUP_RUNNER
//==============================

TEST_GROUP_RUNNER(TestZipMinizipUnZipMinizip) {
    // Zip() + UnZip()
    RUN_TEST_CASE(
        TestZipMinizipUnZipMinizip,
        test_ZippingAndUnzippingRawTextGivesIdenticalOutput_SingleFile);
    RUN_TEST_CASE(
        TestZipMinizipUnZipMinizip,
        test_ZippingAndUnzippingRawTextGivesIdenticalOutput_MultiFile);
    RUN_TEST_CASE(
        TestZipMinizipUnZipMinizip,
        test_ZippingAndUnzippingRawTextGivesIdenticalOutput_MultiFileAndSubDir);
    RUN_TEST_CASE(
        TestZipMinizipUnZipMinizip,
        test_ZippingAndUnzippingRawTextGivesIdenticalOutput_MultiFileAndSubDirDifferentBaseDir);
    RUN_TEST_CASE(
        TestZipMinizipUnZipMinizip,
        test_ZippingAndUnzippingRawTextGivesIdenticalOutput_CanZipAndUnZipFileOf1GB);
    RUN_TEST_CASE(
        TestZipMinizipUnZipMinizip,
        test_ZippingAndUnzippingRawTextGivesIdenticalOutput_CanZipAndUnZipFileOf3GB);
    RUN_TEST_CASE(
        TestZipMinizipUnZipMinizip,
        test_ZippingAndUnzippingRawTextGivesIdenticalOutput_CanZipAndUnZipFileOf6GB);
}
