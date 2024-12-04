#include "unity_fixture.h"
#include <CoDeLib/Deflate_zlib/Deflate_zlib.h>
#include <CoDeLib/Inflate_zlib/Inflate_zlib.h>
#include <CoDeLib/RaiiString/RaiiString.h>
#include <CoDeLib/Test/Utility/FileUtils.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

static char *g_pFullPathToBenchmarkTestFiles = NULL;

void SetupTestDeflateInflateZlib(char *pFullPathToBenchmarkTestFiles) {
    g_pFullPathToBenchmarkTestFiles = pFullPathToBenchmarkTestFiles;
}

TEST_GROUP(TestDeflateInflateZlib);

TEST_SETUP(TestDeflateInflateZlib) {
    // Nothing
}

TEST_TEAR_DOWN(TestDeflateInflateZlib) {
    // Nothing
}

TEST(TestDeflateInflateZlib, test_InflateZlibWorkWithDeflateZlib) {
    FILE *pInFile = NULL;
    FILE *pOutCompressedFile = NULL;
    FILE *pOutDecompressedFile = NULL;

    OpenFile(&pInFile, g_pFullPathToBenchmarkTestFiles,
             "SmallBasicTextFile.txt", "r");
    OpenFile(&pOutCompressedFile, g_pFullPathToBenchmarkTestFiles,
             "SmallBasicTextFile.compressed.txt", "w+");
    OpenFile(&pOutDecompressedFile, g_pFullPathToBenchmarkTestFiles,
             "SmallBasicTextFile.decompressed.txt", "w+");

    const DEFLATE_RETURN_CODES statusDeflate =
        deflate_zlib.Deflate(pInFile, pOutCompressedFile, NULL);

    TEST_ASSERT_GREATER_THAN(0, GetFileSizeInBytes(pOutCompressedFile));
    TEST_ASSERT_EQUAL(DEFLATE_SUCCESS, statusDeflate);

    const INFLATE_RETURN_CODES statusInflate =
        inflate_zlib.Inflate(pOutCompressedFile, pOutDecompressedFile, NULL);

    TEST_ASSERT_GREATER_THAN(0, GetFileSizeInBytes(pOutDecompressedFile));
    TEST_ASSERT_EQUAL(INFLATE_SUCCESS, statusInflate);

    TEST_ASSERT(FilesAreEqual(pInFile, pOutDecompressedFile));
    fclose(pInFile);
    fclose(pOutCompressedFile);
    fclose(pOutDecompressedFile);
}

TEST_GROUP_RUNNER(TestDeflateInflateZlib) {
    RUN_TEST_CASE(TestDeflateInflateZlib, test_InflateZlibWorkWithDeflateZlib);
}
