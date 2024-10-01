#include "unity.h"
#include <CoDeLib/Deflate_zlib/Deflate_zlib.h>
#include <CoDeLib/Inflate_zlib/Inflate_zlib.h>
#include <CoDeLib/RaiiString/RaiiString.h>
#include <CoDeLib/Test/Utility/FileUtils.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

char *g_pFullPathToBenchmarkTestFiles = NULL;

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void test_InflateZlibWorkWithDeflateZlib(void) {
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

    FilesAreEqual(pInFile, pOutDecompressedFile);
    fclose(pInFile);
    fclose(pOutCompressedFile);
    fclose(pOutDecompressedFile);
}

int main(int argc, char **argv) {
    // TODO: use getopt(...)
    if (argc == 1) {
        printf("No arguments provided\n");
        return 1;
    } else {
        g_pFullPathToBenchmarkTestFiles = argv[1];
    }

    UNITY_BEGIN();
    RUN_TEST(test_InflateZlibWorkWithDeflateZlib);
    return UNITY_END();
}