#include <CoDeLib/FileUtils/FileUtils.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void OpenFile(FILE **pInFile, RaiiString *pFullPath, char *pOpenMode) {
    *pInFile = fopen(pFullPath->pString, pOpenMode);
    if (*pInFile == NULL) {
        printf("Failed to open file: %s\n", pFullPath->pString);
        exit(1);
    }
}

size_t GetFileSizeInBytes(FILE *pFile) {
    fseek(pFile, 0, SEEK_END);
    const size_t fileSize = ftell(pFile);
    rewind(pFile);
    return fileSize;
}

bool FilesAreEqual(FILE *pFile1, FILE *pFile2) {
    const size_t fileSize1 = GetFileSizeInBytes(pFile1);
    const size_t fileSize2 = GetFileSizeInBytes(pFile2);

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
