#include "b63/b63.h"
#include <CoDeLib/Deflate_zlib/Deflate_zlib.h>
#include <CoDeLib/Inflate_zlib/Inflate_zlib.h>
#include <CoDeLib/RaiiString/RaiiString.h>
#include <CoDeLib/Test/Utility/FileUtils.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

char *g_pFullPathToBenchmarkTestFiles = NULL;

B63_BENCHMARK(Deflate_zlib_SmallTextFile, n) {
    FILE *pInFile = NULL;
    FILE *pOutCompressedFile = NULL;

    B63_SUSPEND {
        RAII_STRING pathToSmallBasicTextFile =
            RaiiStringCreateFromCString(g_pFullPathToBenchmarkTestFiles);
        RaiiStringAppend_cString(&pathToSmallBasicTextFile,
                                 "SmallBasicTextFile.txt");

        RAII_STRING pathToSmallBasicTextFileCompressedFile =
            RaiiStringCreateFromCString(g_pFullPathToBenchmarkTestFiles);
        RaiiStringAppend_cString(&pathToSmallBasicTextFileCompressedFile,
                                 "SmallBasicTextFile.compressed.txt");

        OpenFile(&pInFile, &pathToSmallBasicTextFile, "r");
        OpenFile(&pOutCompressedFile, &pathToSmallBasicTextFileCompressedFile,
                 "w");
    }

    const DEFLATE_RETURN_CODES statusDeflate =
        deflate_zlib.Deflate(pInFile, pOutCompressedFile, NULL);
    B63_KEEP(statusDeflate);

    B63_SUSPEND {
        assert(GetFileSizeInBytes(pOutCompressedFile) > 0);
        assert(statusDeflate == DEFLATE_SUCCESS);
    }

    B63_SUSPEND {
        fclose(pInFile);
        fclose(pOutCompressedFile);
    }
}

B63_BENCHMARK(Inflate_zlib_SmallTextFile, n) {
    FILE *pCompressedFile = NULL;
    FILE *pOutDecompressedFile = NULL;

    B63_SUSPEND {
        RAII_STRING pathToSmallBasicTextFileCompressed =
            RaiiStringCreateFromCString(g_pFullPathToBenchmarkTestFiles);
        RaiiStringAppend_cString(&pathToSmallBasicTextFileCompressed,
                                 "SmallBasicTextFileCompressed.txt");

        RAII_STRING pathToSmallBasicTextFileDecompressedFile =
            RaiiStringCreateFromCString(g_pFullPathToBenchmarkTestFiles);
        RaiiStringAppend_cString(&pathToSmallBasicTextFileDecompressedFile,
                                 "SmallBasicTextFile.decompressed.txt");

        OpenFile(&pCompressedFile, &pathToSmallBasicTextFileCompressed, "r");
        OpenFile(&pOutDecompressedFile,
                 &pathToSmallBasicTextFileDecompressedFile, "w");
    }

    const INFLATE_RETURN_CODES statusInflate =
        inflate_zlib.Inflate(pCompressedFile, pOutDecompressedFile, NULL);
    B63_KEEP(statusInflate);

    B63_SUSPEND {
        assert(GetFileSizeInBytes(pOutDecompressedFile) > 0);
        assert(statusInflate == INFLATE_SUCCESS);
    }

    B63_SUSPEND {
        fclose(pCompressedFile);
        fclose(pOutDecompressedFile);
    }
}

int main(int argc, char **argv) {
    // TODO: use getopt(...)
    if (argc == 1) {
        printf("No arguments provided\n");
        return 1;
    } else {
        g_pFullPathToBenchmarkTestFiles = argv[1];
    }

    const int argcForB63 = argc - 1;
    argv[1] = argv[0];
    B63_RUN(argcForB63, &argv[1]);

    return 0;
}
