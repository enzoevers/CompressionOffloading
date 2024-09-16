#include "b63/b63.h"
#include <CoDeLib/Deflate_zlib/Deflate_zlib.h>
#include <assert.h>
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
    // printf("Created RaiiString at: 0x%x\n", newRaiistring.pString);
    return newRaiistring;
}

void RaiiStringClean(RaiiString *pThis) {
    // printf("Freeing RaiiString at: 0x%x\n", pThis->pString);
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

size_t GetFileSizeInBytes(FILE *pFile) {
    fseek(pFile, 0, SEEK_END);
    const size_t fileSize = ftell(pFile);
    rewind(pFile);
    return fileSize;
}

B63_BASELINE(basic, n) {
    FILE *pInFile = NULL;
    FILE *pOutFile = NULL;

    B63_SUSPEND {
        char *pInFilename = "SmallBasicTextFile.txt";
        const size_t lengthPathIn =
            strlen(g_pFullPathToBenchmarkTestFiles) + strlen(pInFilename) + 1;
        RaiiString fullInPathString __attribute__((cleanup(RaiiStringClean)));
        fullInPathString = RaiiStringCreate(lengthPathIn);
        CreateFullPathToFile(&fullInPathString, lengthPathIn,
                             g_pFullPathToBenchmarkTestFiles, pInFilename);

        char *pOutFilename = "SmallBasicTextFile.compressed.txt";
        const size_t lengthPathOut =
            strlen(g_pFullPathToBenchmarkTestFiles) + strlen(pOutFilename) + 1;
        RaiiString fullOutPathString __attribute__((cleanup(RaiiStringClean)));
        fullOutPathString = RaiiStringCreate(lengthPathOut);
        CreateFullPathToFile(&fullOutPathString, lengthPathOut,
                             g_pFullPathToBenchmarkTestFiles, pOutFilename);

        // printf("Opening input file: %s\n", fullInPathString.pString);
        pInFile = fopen(fullInPathString.pString, "r");
        if (pInFile == NULL) {
            printf("Failed to open file: %s\n", fullInPathString.pString);
            exit(1);
        }

        // printf("Opening/Creating output file: %s\n",
        // fullOutPathString.pString);
        pOutFile = fopen(fullOutPathString.pString, "w");
        if (pOutFile == NULL) {
            printf("Failed to open file: %s\n", fullOutPathString.pString);
            exit(1);
        }
        assert(GetFileSizeInBytes(pOutFile) == 0);
    }

    const DEFLATE_RETURN_CODES status =
        deflate_zlib.Deflate(pInFile, pOutFile, NULL);
    B63_KEEP(status);

    B63_SUSPEND {
        assert(GetFileSizeInBytes(pOutFile) > 0);
        assert(status == DEFLATE_SUCCESS);
        fclose(pInFile);
        fclose(pOutFile);
    }
}

B63_BENCHMARK(same_bit_different, n) {
    // const DEFLATE_RETURN_CODES status = deflate_zlib.Defalte(NULL, NULL,
    // NULL);
    //  assert(status == DEFLATE_SUCCESS);
    // B63_KEEP(status);
}

int main(int argc, char **argv) {
    // TODO: use getopt(...)
    if (argc == 1) {
        printf("No arguments provided\n");
        return 1;
    } else {
        g_pFullPathToBenchmarkTestFiles = argv[1];
    }
    // printf("Full path to benchmark test files: %s\n",
    //       g_pFullPathToBenchmarkTestFiles);

    const int argcForB63 = argc - 1;
    argv[1] = argv[0];
    B63_RUN(argcForB63, &argv[1]);
    return 0;
}