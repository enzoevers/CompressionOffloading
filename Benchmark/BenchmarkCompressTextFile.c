#include "b63/b63.h"
#include <CoDeLib/Deflate_zlib/Deflate_zlib.h>
#include <CoDeLib/Inflate_zlib/Inflate_zlib.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

char *g_pFullPathToBenchmarkTestFiles = NULL;

// TODO: Move to separate file
typedef struct {
    char *pString;
    size_t length;
} RaiiString;

RaiiString RaiiStringCreate(size_t length) {
    RaiiString newRaiistring = {NULL, length};
    newRaiistring.pString = (char *)calloc(length, sizeof(char));
    if (newRaiistring.pString == NULL) {
        newRaiistring.length = 0;
        printf("Failed to allocate memory for RaiiString\n");
        exit(1);
    }
    return newRaiistring;
}

void RaiiStringClean(RaiiString *pThis) {
    if (pThis->pString != NULL) {
        free(pThis->pString);
        pThis->pString = NULL;
    }
}

void CreateFullPathToFile(const RaiiString *pFullPath,
                          const size_t maxFullPathStringSize,
                          const char *pBasePath, const char *pFileName) {
    assert(pFullPath != NULL);
    assert(pBasePath != NULL);
    assert(pFileName != NULL);
    assert(pFullPath->pString != NULL);
    assert(pFullPath->length >= maxFullPathStringSize);

    const int charsWritten = snprintf(pFullPath->pString, maxFullPathStringSize,
                                      "%s%s", pBasePath, pFileName);

    if (charsWritten < 0) {
        printf("Failed to write to string\n");
        exit(1);
    }
}

bool FilesAreEqual(FILE *pFile1, FILE *pFile2) {
    fseek(pFile1, 0, SEEK_END);
    const size_t fileSize1 = ftell(pFile1);
    rewind(pFile1);

    fseek(pFile2, 0, SEEK_END);
    const size_t fileSize2 = ftell(pFile2);
    rewind(pFile2);

    if (fileSize1 != fileSize2) {
        return false;
    }

    const size_t bufferSize = 1024;
    char buffer1[bufferSize];
    char buffer2[bufferSize];

    size_t bytesRead1 = 0;
    size_t bytesRead2 = 0;

    do {
        bytesRead1 = fread(buffer1, 1, bufferSize, pFile1);
        bytesRead2 = fread(buffer2, 1, bufferSize, pFile2);

        if (bytesRead1 != bytesRead2) {
            return false;
        }

        const int eofFile1 = feof(pFile1);
        const int eofFile2 = feof(pFile2);
        const int errorFile1 = ferror(pFile1);
        const int errorFile2 = ferror(pFile2);
        if ((eofFile1 && !eofFile2) || (!eofFile1 && eofFile2)) {
            printf("EOF indicator set only by one file\n");
        } else if ((!eofFile1 && errorFile1) || (!eofFile2 && errorFile2)) {
            printf("Error indicator set");
            return false;
        }

        if (memcmp(buffer1, buffer2, bytesRead1) != 0) {
            return false;
        }
    } while (bytesRead1 > 0);

    return true;
}

size_t GetFileSizeInBytes(FILE *pFile) {
    fseek(pFile, 0, SEEK_END);
    const size_t fileSize = ftell(pFile);
    rewind(pFile);
    return fileSize;
}

B63_BENCHMARK(CompressDecompress_SmallTextFile_zlib, n) {
    FILE *pInFile = NULL;
    FILE *pOutCompressedFile = NULL;
    FILE *pOutDecompressedFile = NULL;

    B63_SUSPEND {
        // TODO: Move to separate function and/or simplify
        {
            char *pInFilename = "SmallBasicTextFile.txt";
            const size_t lengthPathIn =
                strlen(g_pFullPathToBenchmarkTestFiles) + strlen(pInFilename) +
                1;
            RaiiString fullInPathString
                __attribute__((cleanup(RaiiStringClean)));
            fullInPathString = RaiiStringCreate(lengthPathIn);
            CreateFullPathToFile(&fullInPathString, lengthPathIn,
                                 g_pFullPathToBenchmarkTestFiles, pInFilename);

            pInFile = fopen(fullInPathString.pString, "r");
            if (pInFile == NULL) {
                printf("Failed to open file: %s\n", fullInPathString.pString);
                exit(1);
            }
        }

        {
            char *pOutCompressedFilename = "SmallBasicTextFile.compressed.txt";
            const size_t lengthPathOutCompressed =
                strlen(g_pFullPathToBenchmarkTestFiles) +
                strlen(pOutCompressedFilename) + 1;
            RaiiString fullOutCompressedPathString
                __attribute__((cleanup(RaiiStringClean)));
            fullOutCompressedPathString =
                RaiiStringCreate(lengthPathOutCompressed);
            CreateFullPathToFile(
                &fullOutCompressedPathString, lengthPathOutCompressed,
                g_pFullPathToBenchmarkTestFiles, pOutCompressedFilename);

            pOutCompressedFile =
                fopen(fullOutCompressedPathString.pString, "w+");
            if (pOutCompressedFile == NULL) {
                printf("Failed to open file: %s\n",
                       fullOutCompressedPathString.pString);
                exit(1);
            }
            assert(GetFileSizeInBytes(pOutCompressedFile) == 0);
        }

        {
            char *pOutDecompressedFilename =
                "SmallBasicTextFile.decompressed.txt";
            const size_t lengthPathOutDecompressed =
                strlen(g_pFullPathToBenchmarkTestFiles) +
                strlen(pOutDecompressedFilename) + 1;
            RaiiString fullOutDecompressedPathString
                __attribute__((cleanup(RaiiStringClean)));
            fullOutDecompressedPathString =
                RaiiStringCreate(lengthPathOutDecompressed);
            CreateFullPathToFile(
                &fullOutDecompressedPathString, lengthPathOutDecompressed,
                g_pFullPathToBenchmarkTestFiles, pOutDecompressedFilename);

            pOutDecompressedFile =
                fopen(fullOutDecompressedPathString.pString, "w");
            if (pOutDecompressedFile == NULL) {
                printf("Failed to open file: %s\n",
                       fullOutDecompressedPathString.pString);
                exit(1);
            }
            assert(GetFileSizeInBytes(pOutDecompressedFile) == 0);
        }
    }

    const DEFLATE_RETURN_CODES statusDeflate =
        deflate_zlib.Deflate(pInFile, pOutCompressedFile, NULL);
    B63_KEEP(statusDeflate);

    B63_SUSPEND {
        assert(GetFileSizeInBytes(pOutCompressedFile) > 0);
        assert(statusDeflate == DEFLATE_SUCCESS);
    }

    const INFLATE_RETURN_CODES statusInflate =
        inflate_zlib.Inflate(pOutCompressedFile, pOutDecompressedFile, NULL);
    B63_KEEP(statusInflate);

    B63_SUSPEND {
        assert(GetFileSizeInBytes(pOutDecompressedFile) > 0);
        assert(statusInflate == INFLATE_SUCCESS);
    }

    B63_SUSPEND {
        FilesAreEqual(pInFile, pOutDecompressedFile);
        fclose(pInFile);
        fclose(pOutCompressedFile);
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