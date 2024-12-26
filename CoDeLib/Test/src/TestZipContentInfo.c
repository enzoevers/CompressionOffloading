#include "unity_fixture.h"
#include <CoDeLib/ZipContentInfo.h>
#include <assert.h>
#include <string.h>

TEST_GROUP(TestZipContentInfo);

TEST_SETUP(TestZipContentInfo) {}

TEST_TEAR_DOWN(TestZipContentInfo) {}

//==============================
// ZipContentInfoCreate(...)
//==============================

TEST(
    TestZipContentInfo,
    test_ZipContentInfoCreate_ReturnsEmptyZipContentInfoIfZipFilePathPtrIsNull) {

    RAII_ZIPCONTENTINFO zipContentInfo = ZipContentInfoCreate(NULL);

    TEST_ASSERT_NULL(zipContentInfo.zipFilePath.pString);
    TEST_ASSERT_NULL(zipContentInfo.pUnZippedFilePathArray);
    TEST_ASSERT_EQUAL(0, zipContentInfo.unZippedFileCount);
}

TEST(TestZipContentInfo, test_ZipContentInfoCreate_CreatesCopyOfZipeFilePath) {

    RAII_STRING zipFilePath = RaiiStringCreateFromCString("SomePath/myZip.zip");
    RAII_ZIPCONTENTINFO zipContentInfo = ZipContentInfoCreate(&zipFilePath);

    TEST_ASSERT_NOT_NULL(zipContentInfo.zipFilePath.pString);
    TEST_ASSERT_EQUAL_STRING(zipFilePath.pString,
                             zipContentInfo.zipFilePath.pString);
    TEST_ASSERT_NOT_EQUAL(zipFilePath.pString,
                          zipContentInfo.zipFilePath.pString);
}

TEST(TestZipContentInfo, test_ZipContentInfoCreate_SetsCorrectInitialValues) {

    RAII_STRING zipFilePath = RaiiStringCreateFromCString("myZip.zip");
    RAII_ZIPCONTENTINFO zipContentInfo = ZipContentInfoCreate(&zipFilePath);

    TEST_ASSERT_NOT_NULL(zipContentInfo.zipFilePath.pString);
    TEST_ASSERT_NULL(zipContentInfo.pUnZippedFilePathArray);
    TEST_ASSERT_EQUAL(0, zipContentInfo.unZippedFileCount);
}

//==============================
// ZipContentInfoAddUnzippedFilePath(...)
//==============================

TEST(
    TestZipContentInfo,
    test_ZipContentInfoAddUnzippedFilePath_ReturnsFalseIfZipFileInfoPtrIsNull) {

    RAII_STRING fileName = RaiiStringCreateFromCString("file1.txt");
    bool result = ZipContentInfoAddUnzippedFilePath(NULL, &fileName);
    TEST_ASSERT_FALSE(result);
}

TEST(TestZipContentInfo,
     test_ZipContentInfoAddUnzippedFilePath_ReturnsFalseIfFileNamePtrIsNull) {

    RAII_ZIPCONTENTINFO zipContentInfo = (ZipContentInfo){
        .zipFilePath = RaiiStringCreateFromCString("someZipFile.zip"),
        .pUnZippedFilePathArray = NULL,
        .unZippedFileCount = 0};

    bool result = ZipContentInfoAddUnzippedFilePath(&zipContentInfo, NULL);
    TEST_ASSERT_FALSE(result);
}

TEST(
    TestZipContentInfo,
    test_ZipContentInfoAddUnzippedFilePath_ReturnsFalseIfZipFilePathStringPtrIsNull) {

    RAII_ZIPCONTENTINFO zipContentInfo =
        (ZipContentInfo){.zipFilePath = {NULL, 0},
                         .pUnZippedFilePathArray = NULL,
                         .unZippedFileCount = 0};

    RAII_STRING fileName = RaiiStringCreateFromCString("file1.txt");

    bool result = ZipContentInfoAddUnzippedFilePath(&zipContentInfo, &fileName);
    TEST_ASSERT_FALSE(result);
}

TEST(
    TestZipContentInfo,
    test_ZipContentInfoAddUnzippedFilePath_ReturnsFalseIfFileNameStringPtrIsNull) {

    RAII_ZIPCONTENTINFO zipContentInfo = (ZipContentInfo){
        .zipFilePath = RaiiStringCreateFromCString("someZipFile.zip"),
        .pUnZippedFilePathArray = NULL,
        .unZippedFileCount = 0};

    RAII_STRING fileName = (RaiiString){NULL, 0};

    bool result = ZipContentInfoAddUnzippedFilePath(&zipContentInfo, &fileName);
    TEST_ASSERT_FALSE(result);
}

TEST(TestZipContentInfo,
     test_ZipContentInfoAddUnzippedFilePath_ReturnsTrueIfAllPtrsValid) {

    RAII_STRING zipFilePath = RaiiStringCreateFromCString("SomePath/myZip.zip");
    RAII_ZIPCONTENTINFO zipContentInfo = ZipContentInfoCreate(&zipFilePath);

    RAII_STRING fileName = RaiiStringCreateFromCString("file1.txt");

    bool result = ZipContentInfoAddUnzippedFilePath(&zipContentInfo, &fileName);
    TEST_ASSERT_TRUE(result);
}

TEST(TestZipContentInfo,
     test_ZipContentInfoAddUnzippedFilePath_CreatesCopyOfFileName) {

    RAII_STRING zipFilePath = RaiiStringCreateFromCString("SomePath/myZip.zip");
    RAII_ZIPCONTENTINFO zipContentInfo = ZipContentInfoCreate(&zipFilePath);

    RAII_STRING fileName1 = RaiiStringCreateFromCString("file1.txt");
    ZipContentInfoAddUnzippedFilePath(&zipContentInfo, &fileName1);

    TEST_ASSERT_NOT_NULL(zipContentInfo.pUnZippedFilePathArray[0].pString);
    TEST_ASSERT_NOT_EQUAL(fileName1.pString,
                          zipContentInfo.pUnZippedFilePathArray[0].pString);
}

TEST(
    TestZipContentInfo,
    test_ZipContentInfoAddUnzippedFilePath_IncrementsFileCounterFileOneAndAddsFileNameToArray) {

    RAII_STRING zipFilePath = RaiiStringCreateFromCString("SomePath/myZip.zip");
    RAII_ZIPCONTENTINFO zipContentInfo = ZipContentInfoCreate(&zipFilePath);

    RAII_STRING fileName1 = RaiiStringCreateFromCString("file1.txt");
    ZipContentInfoAddUnzippedFilePath(&zipContentInfo, &fileName1);

    TEST_ASSERT_EQUAL(1, zipContentInfo.unZippedFileCount);
    TEST_ASSERT_EQUAL_STRING(fileName1.pString,
                             zipContentInfo.pUnZippedFilePathArray[0].pString);
}

TEST(TestZipContentInfo,
     test_ZipContentInfoAddUnzippedFilePath_CanAddMultipleFiles) {

    RAII_STRING zipFilePath = RaiiStringCreateFromCString("SomePath/myZip.zip");
    RAII_ZIPCONTENTINFO zipContentInfo = ZipContentInfoCreate(&zipFilePath);

    RAII_STRING fileName1 = RaiiStringCreateFromCString("file1.txt");
    ZipContentInfoAddUnzippedFilePath(&zipContentInfo, &fileName1);

    RAII_STRING fileName2 = RaiiStringCreateFromCString("file2.txt");
    ZipContentInfoAddUnzippedFilePath(&zipContentInfo, &fileName2);

    TEST_ASSERT_EQUAL(2, zipContentInfo.unZippedFileCount);
    TEST_ASSERT_EQUAL_STRING(fileName1.pString,
                             zipContentInfo.pUnZippedFilePathArray[0].pString);
    TEST_ASSERT_EQUAL_STRING(fileName2.pString,
                             zipContentInfo.pUnZippedFilePathArray[1].pString);
}

//==============================
// ZipContentInfoClean(...)
//==============================

TEST(
    TestZipContentInfo,
    test_ZipContentInfoClean_SetsZipFileNameToNullptrIfThereIsNoFileNameArray) {

    RAII_STRING zipFilePath = RaiiStringCreateFromCString("SomePath/myZip.zip");
    ZipContentInfo zipContentInfo = ZipContentInfoCreate(&zipFilePath);

    ZipContentInfoClean(&zipContentInfo);
    TEST_ASSERT_NULL(zipContentInfo.zipFilePath.pString);
    TEST_ASSERT_NULL(zipContentInfo.pUnZippedFilePathArray);
    TEST_ASSERT_EQUAL(0, zipContentInfo.unZippedFileCount);
}

TEST(TestZipContentInfo,
     test_ZipContentInfoClean_DoesNotCleanProvidedZipFilePath) {

    RAII_STRING zipFilePath = RaiiStringCreateFromCString("SomePath/myZip.zip");
    ZipContentInfo zipContentInfo = ZipContentInfoCreate(&zipFilePath);

    ZipContentInfoClean(&zipContentInfo);
    TEST_ASSERT_NOT_NULL(zipFilePath.pString);
    TEST_ASSERT_EQUAL_STRING("SomePath/myZip.zip", zipFilePath.pString);
    TEST_ASSERT_EQUAL(19, zipFilePath.lengthWithTermination);
}

TEST(TestZipContentInfo, test_ZipContentInfoClean_CleansAllFileNamesInArray) {

    RAII_STRING zipFilePath = RaiiStringCreateFromCString("SomePath/myZip.zip");
    ZipContentInfo zipContentInfo = ZipContentInfoCreate(&zipFilePath);

    RAII_STRING file1 = RaiiStringCreateFromCString("file1.txt");
    RAII_STRING file2 = RaiiStringCreateFromCString("SomeDir/file2.txt");
    RAII_STRING file3 = RaiiStringCreateFromCString("file3.txt");
    ZipContentInfoAddUnzippedFilePath(&zipContentInfo, &file1);
    ZipContentInfoAddUnzippedFilePath(&zipContentInfo, &file2);
    ZipContentInfoAddUnzippedFilePath(&zipContentInfo, &file3);

    ZipContentInfoClean(&zipContentInfo);
    TEST_ASSERT_NULL(zipContentInfo.zipFilePath.pString);
    TEST_ASSERT_NULL(zipContentInfo.pUnZippedFilePathArray);
    TEST_ASSERT_EQUAL(0, zipContentInfo.unZippedFileCount);
}

//==============================
// RAII_ZIPCONTENTINFO
//==============================

// TODO: Create tests

//==============================
// TEST_GROUP_RUNNER
//==============================

TEST_GROUP_RUNNER(TestZipContentInfo) {
    // ZipContentInfoCreate()
    RUN_TEST_CASE(
        TestZipContentInfo,
        test_ZipContentInfoCreate_ReturnsEmptyZipContentInfoIfZipFilePathPtrIsNull);
    RUN_TEST_CASE(TestZipContentInfo,
                  test_ZipContentInfoCreate_CreatesCopyOfZipeFilePath);
    RUN_TEST_CASE(TestZipContentInfo,
                  test_ZipContentInfoCreate_SetsCorrectInitialValues);

    // ZipContentInfoAddUnzippedFilePath()
    RUN_TEST_CASE(
        TestZipContentInfo,
        test_ZipContentInfoAddUnzippedFilePath_ReturnsFalseIfZipFileInfoPtrIsNull);
    RUN_TEST_CASE(
        TestZipContentInfo,
        test_ZipContentInfoAddUnzippedFilePath_ReturnsFalseIfFileNamePtrIsNull);
    RUN_TEST_CASE(
        TestZipContentInfo,
        test_ZipContentInfoAddUnzippedFilePath_ReturnsFalseIfZipFilePathStringPtrIsNull);
    RUN_TEST_CASE(
        TestZipContentInfo,
        test_ZipContentInfoAddUnzippedFilePath_ReturnsFalseIfFileNameStringPtrIsNull);
    RUN_TEST_CASE(
        TestZipContentInfo,
        test_ZipContentInfoAddUnzippedFilePath_ReturnsTrueIfAllPtrsValid);
    RUN_TEST_CASE(TestZipContentInfo,
                  test_ZipContentInfoAddUnzippedFilePath_CreatesCopyOfFileName);
    RUN_TEST_CASE(
        TestZipContentInfo,
        test_ZipContentInfoAddUnzippedFilePath_IncrementsFileCounterFileOneAndAddsFileNameToArray);
    RUN_TEST_CASE(TestZipContentInfo,
                  test_ZipContentInfoAddUnzippedFilePath_CanAddMultipleFiles);

    // ZipContentInfoClean()
    RUN_TEST_CASE(
        TestZipContentInfo,
        test_ZipContentInfoClean_SetsZipFileNameToNullptrIfThereIsNoFileNameArray);
    RUN_TEST_CASE(TestZipContentInfo,
                  test_ZipContentInfoClean_DoesNotCleanProvidedZipFilePath);
    RUN_TEST_CASE(TestZipContentInfo,
                  test_ZipContentInfoClean_CleansAllFileNamesInArray);
}
