#include <CoDeLib/FileUtils/FileUtils.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <Windows.h>
#include <ctype.h>
#else
#include <errno.h>
#include <sys/stat.h>
#endif

// Based on
// https://nachtimwald.com/2019/07/10/recursive-create-directory-in-c-revisited/
bool RecursiveMkdir(const char *pDirname) {
    if (pDirname == NULL) {
        return false;
    }

    bool success = true;

    const char separator = '/';

    const size_t dirnameLength = strlen(pDirname);
    const char *pDirnameRef = pDirname;

    /* Skip Windows drive letter. */
#ifdef _WIN32
    if (dirnameLength < 3) {
        return false;
    }
    pDirnameRef = strchr(pDirnameRef, ':');
    if (pDirnameRef != NULL) {
        pDirnameRef++;
    }
#endif

    char *pDirnamePart = calloc(dirnameLength + 1, sizeof(char));
    pDirnameRef = strchr(pDirnameRef, separator);
    while (pDirnameRef != NULL) {
        /* Skip empty elements. Could be a Windows UNC path or
           just multiple separators which is okay. */
        if (pDirnameRef != pDirname && *(pDirnameRef - 1) == separator) {
            pDirnameRef++;
            continue;
        }
        /* Put the path up to this point into a temporary to
           pass to the make directory function. */
        const size_t dirnamePartLength =
            (pDirnameRef - pDirname) / sizeof(char);
        memcpy(pDirnamePart, pDirname, dirnamePartLength);
        // Adds null terminator after the seperator
        pDirnamePart[dirnamePartLength] = '\0';

#ifdef _WIN32
        const int creationSuccess = CreateDirectory(pDirnamePart, NULL);
        if (creationSuccess == FALSE) {
            const DWORD lastError = GetLastError();
            if (lastError != ERROR_ALREADY_EXISTS) {
                success = false;
                break;
            }
        }
#else
        const int creationSuccess = mkdir(pDirnamePart, 0774);
        if (creationSuccess != 0) {
            if (errno != EEXIST) {
                printf("\npDirnamePart: %s\n", pDirnamePart);
                printf("error: %s\n", strerror(errno));
                success = false;
                break;
            }
        }
#endif
        pDirnameRef++;
        pDirnameRef = strchr(pDirnameRef, separator);
    }
    free(pDirnamePart);
    return success;
}

void OpenFileWithMode(FILE **pInFile, RaiiString *pFullPath, char *pOpenMode) {
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
