#include <CoDeLib/FileUtils/FileUtils.h>
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define __USE_XOPEN_EXTENDED 1
#include <ftw.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

//=========================
// Internal function definitions
//=========================

// Internal function. Must only be called by GetCurrentWorkingDirectory(...)
#ifdef _WIN32
char *_GetCurrentWorkingDirectory_Windows(char *pFullPath, size_t fullPathSize);
#else
char *_GetCurrentWorkingDirectory_Posix(char *pFullPath, size_t fullPathSize);
#endif

/*!
@brief Skips the root part of a path and updates the `pPath` pointer to point to
the first character after the root part. This function is platform agnostic.
@param pPath The path to skip the root part of.
@return true if the root part was skipped, false otherwise.
*/
bool _SkipRootPathOfPath(const char *pPath);

/*!
@brief Creates a directory. This function is platform agnostic.
@param pPath The path to create. All directories but the last in the path must
exist.
@return true if the directory was created or already exists, false otherwise.
*/
bool _CreateDir(const char *const pPath);

/*!
@brief Handles the callback for the nftw function to remove directories and
files.
 */
int _HandleFtwCallback_Remove(const char *fpath, const struct stat *sb,
                              int typeflag, struct FTW *ftwbuf);

//=========================
// Public function implementations
//=========================

// Based on
// https://nachtimwald.com/2019/07/10/recursive-create-directory-in-c-revisited/
bool RecursiveMkdir(const char *const pDirname) {
    if (pDirname == NULL) {
        return false;
    }

    const size_t dirnameLength = strlen(pDirname);
    if (dirnameLength == 0) {
        return false;
    }

    char targetPath[MAX_PATH_LENGTH_WTH_TERMINATOR];

    bool absolutePathSuccess = GetAbsolutePath(pDirname, &targetPath[0],
                                               MAX_PATH_LENGTH_WTH_TERMINATOR);
    if (!absolutePathSuccess) {
        return false;
    }

    bool success = true;
    const char separator = '/';
    char *pTargetPath = &targetPath[0];
    const char *pBeginOfTargetPath = pTargetPath;
    const size_t targetPathLength = strlen(pTargetPath);

    bool skipRootPath = _SkipRootPathOfPath(pTargetPath);
    if (!skipRootPath) {
        return false;
    }

    char *pPathToCreate = calloc(targetPathLength + 1, sizeof(char));
    if (pPathToCreate == NULL) {
        return false;
    }

    pTargetPath = strchr(pTargetPath, separator);
    while (pTargetPath != NULL) {
        /* Skip empty elements. Could be a Windows UNC path or
           just multiple separators which is okay. */
        if (pTargetPath != pBeginOfTargetPath &&
            *(pTargetPath - 1) == separator) {
            pTargetPath++;
            pTargetPath = strchr(pTargetPath, separator);
            continue;
        }
        // Get the length of the directory name part with the separator and
        // without the null terminator.
        const size_t dirnamePartLength =
            ((pTargetPath - pBeginOfTargetPath) / sizeof(char)) + 1;
        if (dirnamePartLength > targetPathLength) {
            success = false;
            break;
        }
        // Put the path up to this point into a temporary to pass to the make
        // directory function.
        memcpy(pPathToCreate, pBeginOfTargetPath, dirnamePartLength);
        // Adds null terminator after the seperator
        pPathToCreate[dirnamePartLength] = '\0';

        success = _CreateDir(pPathToCreate);
        if (!success) {
            break;
        }

        pTargetPath++;
        pTargetPath = strchr(pTargetPath, separator);
    }

    free(pPathToCreate);
    pPathToCreate = NULL;
    return success;
}

bool RecursiveRmdir(const char *const pDirname) {
    if (pDirname == NULL) {
        return false;
    }

    const size_t dirnameLength =
        strnlen(pDirname, MAX_PATH_LENGTH_WTH_TERMINATOR);
    if (dirnameLength == 0 || dirnameLength >= MAX_PATH_LENGTH_WTH_TERMINATOR) {
        return false;
    }

    const char lastChar = pDirname[dirnameLength - 1];
    if (lastChar != '/' && lastChar != '\\') {
        return false;
    }

    const int maxOpenFiles = 64;
    const int flags = FTW_DEPTH | FTW_PHYS;
    int status = nftw(pDirname, _HandleFtwCallback_Remove, maxOpenFiles, flags);

    return status == 0;
}

bool IsAbsolutePath(const char *pPath) {
    if (pPath == NULL || pPath[0] == '\0') {
        return false;
    }

    // Windows
    if (isalpha(pPath[0]) && pPath[1] == ':') {
        return true;
    }

    // Linux
    else if (pPath[0] == '/') {
        return true;
    }

    return false;
}

bool GetAbsolutePath(const char *pPath, char *const pAbsolutePath,
                     const size_t absolutePathSize) {
    if (pPath == NULL || pAbsolutePath == NULL || absolutePathSize == 0) {
        return false;
    }

    const size_t pathLenght = strnlen(pPath, MAX_PATH_LENGTH_WTH_TERMINATOR);
    if (pathLenght == 0 || pathLenght >= MAX_PATH_LENGTH_WTH_TERMINATOR) {
        return false;
    }

    if (absolutePathSize < (pathLenght + 1)) {
        return false;
    }

    if (!IsAbsolutePath(pPath)) {
        // Get the current working directory and append the dirname to it.
        char *currentPath =
            GetCurrentWorkingDirectory(&pAbsolutePath[0], absolutePathSize);
        if (currentPath == NULL) {
            return false;
        }
        const size_t currentPathLength = strlen(pAbsolutePath);
        // (dirnameLength + 1) to include the null terminator
        memcpy(&pAbsolutePath[currentPathLength], pPath,
               sizeof(char) * (pathLenght + 1));
    } else {
        // Copy the absolute path to the target path.
        // (dirnameLength + 1) to include the null terminator
        memcpy(&pAbsolutePath[0], pPath, sizeof(char) * (pathLenght + 1));
    }

    return true;
}

char *GetCurrentWorkingDirectory(char *pFullPath, size_t fullPathSize) {
    if (pFullPath == NULL || fullPathSize == 0) {
        return NULL;
    }

    char *pCurrentPath = NULL;

#ifdef _WIN32
    pCurrentPath = _GetCurrentWorkingDirectory_Windows(pFullPath, fullPathSize);
#else
    pCurrentPath = _GetCurrentWorkingDirectory_Posix(pFullPath, fullPathSize);
#endif

    if (pCurrentPath == NULL) {
        return NULL;
    }
    assert((pCurrentPath == pFullPath) && "buffer was reallocated");

    const size_t currentPathLength = strlen(pFullPath);
    if (pFullPath[currentPathLength - 1] != '/') {
        if (fullPathSize < currentPathLength + 2) {
            return NULL;
        }
        // Append '/' if not already present
        pFullPath[currentPathLength] = '/';
        pFullPath[currentPathLength + 1] = '\0';
    }

    return pFullPath;
}

bool PathExists(const char *pPath) {
    if (pPath == NULL) {
        return false;
    }

    const size_t pathLenght = strnlen(pPath, MAX_PATH_LENGTH_WTH_TERMINATOR);
    if (pathLenght == 0 || pathLenght > MAX_PATH_LENGTH_WTH_TERMINATOR) {
        return false;
    }

    char targetPath[MAX_PATH_LENGTH_WTH_TERMINATOR];

    bool absolutePathSuccess =
        GetAbsolutePath(pPath, &targetPath[0], MAX_PATH_LENGTH_WTH_TERMINATOR);
    if (!absolutePathSuccess) {
        return false;
    }

    bool pathExists = false;

#ifdef _WIN32
    const DWORD attributes = GetFileAttributes(&targetPath[0]);
    if (attributes != INVALID_FILE_ATTRIBUTES) {
        pathExists = true;
    }
#else
    struct stat statbuf;
    const int statResult = stat(&targetPath[0], &statbuf);
    if (statResult == 0) {
        pathExists = true;
    }
#endif

    return pathExists;
}

size_t ExtractLastPartOfPath(const char *const pPath, char *const pDestBuffer,
                             size_t destBufferSize) {
    if (pPath == NULL || pDestBuffer == NULL || destBufferSize == 0) {
        return SIZE_MAX;
    }

    size_t pathLenght = strnlen(pPath, MAX_PATH_LENGTH_WTH_TERMINATOR);
    if (pathLenght == 0 || pathLenght == MAX_PATH_LENGTH_WTH_TERMINATOR) {
        return SIZE_MAX;
    }

    if (pathLenght == 1 && (pPath[0] == '/' || pPath[0] == '\\')) {
        return SIZE_MAX;
    }

    char lastChar = pPath[pathLenght - 1];
    bool isDir = lastChar == '/' || lastChar == '\\';

    if (isDir) {
        // Ignore empty sections of the path
        for (size_t i = pathLenght - 2; i > 0; --i) {
            if (pPath[i] == '/' || pPath[i] == '\\') {
                // Assigning again because this may be different from the last
                lastChar = pPath[i];
                pathLenght--;
            } else {
                break;
            }
        }
    }

    char *pLocalPath = (char *)calloc(pathLenght + 1, sizeof(char));
    memcpy(pLocalPath, pPath, pathLenght + 1);
    pLocalPath[pathLenght] = '\0';

    if (isDir) {
        // Remove the last separator to make getting the file or directory name
        // more generic.
        pLocalPath[pathLenght - 1] = '\0';
    }

    char *pLastForwardSeparator = strrchr(pLocalPath, '/');
    char *pLastBackwardSeparator = strrchr(pLocalPath, '\\');
    char *pLastSeparator = pLastForwardSeparator > pLastBackwardSeparator
                               ? pLastForwardSeparator
                               : pLastBackwardSeparator;

    size_t startIndexOfNameInPath = SIZE_MAX;
    char *pLastPart = NULL;

    if (pLastSeparator == NULL) {
        // No separator found, so the last part is the whole path.
        startIndexOfNameInPath = 0;
        pLastPart = pLocalPath;
    } else {
        startIndexOfNameInPath = (pLastSeparator - pLocalPath) + 1;
        pLastPart = pLastSeparator + 1;
    }

    if (isDir) {
        // Add the last separator back if it was removed.
        pLocalPath[pathLenght - 1] = lastChar;
    }

    const size_t lastPartLength =
        strnlen(pLastPart, MAX_PATH_LENGTH_WTH_TERMINATOR);

    if (lastPartLength == MAX_PATH_LENGTH_WTH_TERMINATOR) {
        free(pLocalPath);
        return SIZE_MAX;
    }

    if (lastPartLength >= destBufferSize) {
        free(pLocalPath);
        return SIZE_MAX;
    }

    memcpy(pDestBuffer, pLastPart, lastPartLength + 1);

    free(pLocalPath);
    return startIndexOfNameInPath;
}

void OpenFileWithMode(FILE **pInFile, const RaiiString *const pFullPath,
                      const char *const pOpenMode) {
    *pInFile = fopen(pFullPath->pString, pOpenMode);
    if (*pInFile == NULL) {
        printf("Failed to open file: %s\n", pFullPath->pString);
        exit(1);
    }
}

uint64_t GetFileSizeInBytes(FILE *pFile) {
    if (pFile == NULL) {
        return 0;
    }

    fseeko(pFile, 0, SEEK_END);
    const off_t fileSize = ftello(pFile);
    rewind(pFile);

    if (fileSize == -1) {
        printf("Failed to get file size: %s\n", strerror(errno));
        return 0;
    }

    return fileSize;
}

bool FilesAreEqual(FILE *pFile1, FILE *pFile2) {
    const uint64_t fileSize1 = GetFileSizeInBytes(pFile1);
    const uint64_t fileSize2 = GetFileSizeInBytes(pFile2);

    if (fileSize1 != fileSize2) {
        printf("File sizes not equal:\n");
        printf("    fileSize1: %llu\n", (unsigned long long)fileSize1);
        printf("    fileSize2: %llu\n", (unsigned long long)fileSize2);
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
            printf("Bytes read not equal:\n");
            printf("    bytesRead1: %lu\n", (unsigned long)bytesRead1);
            printf("    bytesRead2: %lu\n", (unsigned long)bytesRead2);
            return false;
        }

        const int eofFile1 = feof(pFile1);
        const int eofFile2 = feof(pFile2);
        const int errorFile1 = ferror(pFile1);
        const int errorFile2 = ferror(pFile2);
        if ((eofFile1 && !eofFile2) || (!eofFile1 && eofFile2)) {
            printf("EOF indicator set only by one file\n");
            return false;
        } else if ((!eofFile1 && errorFile1) || (!eofFile2 && errorFile2)) {
            printf("Error indicator set");
            return false;
        }

        if (memcmp(buffer1, buffer2, bytesRead1) != 0) {
            printf("Buffers not equal\n");
            printf("    buffer1: %s\n", buffer1);
            printf("    buffer2: %s\n", buffer2);
            return false;
        }
    } while (bytesRead1 > 0);

    return true;
}

//=========================
// Internal function implementations
//=========================

#ifdef _WIN32
// Internal function. Must only be called by GetCurrentWorkingDirectory(...)
char *_GetCurrentWorkingDirectory_Windows(char *pFullPath,
                                          size_t fullPathSize) {
    // GetCurrentDirectory() return the size including the null terminator
    // if (0, NULL) is provided.
    const size_t currentPathLengthWithTerminator = GetCurrentDirectory(0, NULL);
    if (currentPathLengthWithTerminator == 0 ||
        currentPathLengthWithTerminator > fullPathSize) {
        return NULL;
    }
    char *pCurrentWorkingDirectory =
        calloc(currentPathLengthWithTerminator, sizeof(char));

    const size_t numCharsWritten = GetCurrentDirectory(
        currentPathLengthWithTerminator, pCurrentWorkingDirectory);
    if (numCharsWritten == 0) {
        return NULL;
        free(pCurrentWorkingDirectory);
        pCurrentWorkingDirectory = NULL;
    }

    pFullPath = strncpy(pFullPath, pCurrentWorkingDirectory, fullPathSize - 1);
    if (pFullPath == NULL) {
        free(pCurrentWorkingDirectory);
        pCurrentWorkingDirectory = NULL;
        return NULL;
    }
    pFullPath[fullPathSize - 1] = '\0';

    free(pCurrentWorkingDirectory);
    pCurrentWorkingDirectory = NULL;

    return pFullPath;
}
#endif

#ifndef _WIN32
// Internal function. Must only be called by GetCurrentWorkingDirectory(...)
char *_GetCurrentWorkingDirectory_Posix(char *pFullPath, size_t fullPathSize) {
    return getcwd(pFullPath, fullPathSize);
}
#endif

bool _SkipRootPathOfPath(const char *pPath) {
    if (pPath == NULL) {
        return false;
    }

    const size_t pathLenght = strnlen(pPath, MAX_PATH_LENGTH_WTH_TERMINATOR);
    if (pathLenght == 0 || pathLenght > MAX_PATH_LENGTH_WTH_TERMINATOR) {
        return false;
    }

#ifdef _WIN32
    if (pathLenght < 3) {
        return false;
    }
    // Skip Windows drive letter for Windows.
    if (isalpha(pPath[0]) && pPath[1] == ':') {
        pPath = &pPath[2];
    } else {
        // Path if not absolute
        return false;
    }
#else
    if (pathLenght < 2) {
        return false;
    }
    // Skip root separator for non-Windows.
    if (pPath[0] == '/') {
        pPath = &pPath[1];
    } else {
        // Path if not absolute
        return false;
    }
#endif

    return true;
}

bool _CreateDir(const char *const pPath) {
    if (pPath == NULL) {
        return false;
    }

#ifdef _WIN32
    const int creationSuccess = CreateDirectory(pPath, NULL);
    if (creationSuccess == FALSE) {
        const DWORD lastError = GetLastError();
        if (lastError != ERROR_ALREADY_EXISTS) {
            return false;
        }
    }
#else
    const int creationSuccess = mkdir(pPath, 0774);
    if (creationSuccess != 0) {
        if (errno != EEXIST) {
            return false;
        }
    }
#endif

    return true;
}

int _HandleFtwCallback_Remove(const char *fpath,
                              const struct stat *sb __attribute__((unused)),
                              int typeflag,
                              struct FTW *ftwbuf __attribute__((unused))) {
    if (typeflag == FTW_DP) {
        // Remove directory
        return rmdir(fpath);
    }

    // Remove file
    return remove(fpath);
}
