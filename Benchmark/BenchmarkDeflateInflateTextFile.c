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
        OpenFile(&pInFile, g_pFullPathToBenchmarkTestFiles,
                 "SmallBasicTextFile.txt", "r");
        OpenFile(&pOutCompressedFile, g_pFullPathToBenchmarkTestFiles,
                 "SmallBasicTextFile.compressed.txt", "w");
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
        OpenFile(&pCompressedFile, g_pFullPathToBenchmarkTestFiles,
                 "SmallBasicTextFileCompressed.txt", "r");
        OpenFile(&pOutDecompressedFile, g_pFullPathToBenchmarkTestFiles,
                 "SmallBasicTextFile.decompressed.txt", "w+");
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
