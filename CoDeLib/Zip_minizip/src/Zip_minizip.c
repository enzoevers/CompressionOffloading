#include <CoDeLib/FileUtils/FileUtils.h>
#include <CoDeLib/Zip_minizip/Zip_minizip.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <fts.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

// minizip
#include <mz.h>
#include <zip.h>

//==============================
// Helper functions
//==============================

/*
@brief Zips the content of a path into a zip file. It also handles directories
by recursively going through them.
@param zFile The zip file to write to. This should already have been opened with
zipOpen64.
@param pInputPath The full path to zip. This can be a file or a directory path.
@param level This is the level at which the file or directory is in the zip
file.
*/
ZIP_RETURN_CODES ZipPath(zipFile zFile, const char *const pInputPath,
                         uint8_t level);

#ifdef _WIN32
ZIP_RETURN_CODES ZipDirectory_Windows(zipFile zFile,
                                      const char *const pInputPath,
                                      uint8_t level);
#else
ZIP_RETURN_CODES ZipDirectory_Posix(zipFile zFile,
                                    const char *const pInputPath);
#endif
//==============================
// Interface functions
//==============================

ZIP_RETURN_CODES
Zip(const char *pOutputZipPath, const char **pInputPathArray,
    const size_t inputPathArraySize) {
    if (pOutputZipPath == NULL || pInputPathArray == NULL ||
        inputPathArraySize == 0) {
        return ZIP_ERROR;
    }

    if (PathExists(pOutputZipPath)) {
        printf("Output zip file already exists: %s\n", pOutputZipPath);
        return ZIP_ERROR;
    }

    ZIP_RETURN_CODES returnCode = ZIP_SUCCESS;

    // It doesn't matter that the path contains the file name. This is
    // simply ignored.
    RecursiveMkdir(pOutputZipPath);

    const int append = 0;
    zipFile zFile = zipOpen64(pOutputZipPath, append);
    if (zFile == NULL) {
        // TODO: make the string handling safer by limiting the size
        printf("Could not open %s for zipping\n", pOutputZipPath);
        zipClose(zFile, NULL);
        return false;
    }

    for (size_t i = 0; i < inputPathArraySize && returnCode == ZIP_SUCCESS;
         ++i) {
        const size_t pathLength =
            strnlen(pInputPathArray[i], MAX_PATH_LENGTH_WTH_TERMINATOR);
        if (pathLength == MAX_PATH_LENGTH_WTH_TERMINATOR) {
            printf("Path is too long: %s\n", pInputPathArray[i]);
            returnCode = ZIP_ERROR;
            break;
        }

        const char lastChar = pInputPathArray[i][pathLength - 1];
        bool isDir = lastChar == '/' || lastChar == '\\';

        if (!PathExists(pInputPathArray[i])) {
            printf("Path does not exist: %s\n", pInputPathArray[i]);
            returnCode = ZIP_ERROR;
            break;
        }

        if (!isDir) {
            returnCode = ZipPath(zFile, pInputPathArray[i], 0);
        } else {
#ifdef _WIN32
            returnCode = ZipDirectory_Windows(zFile, pInputPathArray[i], 0);
#else
            returnCode = ZipDirectory_Posix(zFile, pInputPathArray[i]);
#endif
        }
    }

    const char *global_comment = NULL;
    zipClose(zFile, global_comment);

    return returnCode;
}

const struct IZip zip_minizip = {
    .Zip = Zip,
};

//==============================
// Helper functions
//==============================

ZIP_RETURN_CODES ZipPath(zipFile zFile, const char *const pInputPath,
                         uint8_t level) {
    ZIP_RETURN_CODES returnCode = ZIP_SUCCESS;

    RAII_STRING pathToSaveInZip = RaiiStringCreateFromCString("");

    RAII_STRING tmpInputPath = RaiiStringCreateFromCString(pInputPath);
    for (size_t i = 0; i <= level; ++i) {
        char lastPartOfPath[MAX_PATH_LENGTH_WTH_TERMINATOR];
        size_t startIndexOfLastPart =
            ExtractLastPartOfPath(tmpInputPath.pString, lastPartOfPath,
                                  MAX_PATH_LENGTH_WTH_TERMINATOR);

        if (startIndexOfLastPart == SIZE_MAX) {
            printf("startIndexOfLastPart == SIZE_MAX\n");
            return ZIP_ERROR;
        }

        char tmpBuf[MAX_PATH_LENGTH_WTH_TERMINATOR];
        const size_t charsWritten =
            snprintf(&tmpBuf[0], MAX_PATH_LENGTH_WTH_TERMINATOR, "%s%s",
                     lastPartOfPath, pathToSaveInZip.pString);
        if (charsWritten < strlen(lastPartOfPath)) {
            printf("The whole path was not written. \n  Expected: '%s%s'\n  "
                   "Actual: '%s'\n",
                   lastPartOfPath, pathToSaveInZip.pString, tmpBuf);
            return ZIP_ERROR;
        }
        RaiiStringClean(&pathToSaveInZip);
        pathToSaveInZip = RaiiStringCreateFromCString(tmpBuf);

        tmpInputPath.pString[startIndexOfLastPart] = '\0';
        tmpInputPath.lengthWithTermination = startIndexOfLastPart + 1;
    }
    RaiiStringClean(&tmpInputPath);

    RAII_STRING inputPath = RaiiStringCreateFromCString(pInputPath);

    const size_t lastPartOfPathLenght =
        pathToSaveInZip.lengthWithTermination - 1;
    if (lastPartOfPathLenght == MAX_PATH_LENGTH_WTH_TERMINATOR) {
        printf("lastPartOfPathLenght == MAX_PATH_LENGTH_WTH_TERMINATOR\n");
        return ZIP_ERROR;
    }

    const char lastChar = pathToSaveInZip.pString[lastPartOfPathLenght - 1];
    bool isDir = lastChar == '/' || lastChar == '\\';

    uint64_t fileSize = 0;

    if (!isDir) {
        FILE *pFile = NULL;
        OpenFileWithMode(&pFile, &inputPath, "rb");
        fileSize = GetFileSizeInBytes(pFile);
        fclose(pFile);
    } else if (lastChar == '\\') {
        // Zip requeres '/' as a separator to indicate directories
        pathToSaveInZip.pString[lastPartOfPathLenght - 1] = '/';
    }

    const int compressoinsMethod = MZ_COMPRESS_METHOD_STORE;
    const int compressionLevel = 0;
    // Don't store as raw file. Otherwise the CRC is not properly handled.
    const int raw = 0;
    const int requiresZip64 = (fileSize > 0xffffffff) ? 1 : 0;

    int status = zipOpenNewFileInZip2_64(
        zFile, pathToSaveInZip.pString, NULL, NULL, 0, NULL, 0, NULL,
        compressoinsMethod, compressionLevel, raw, requiresZip64);

    if (status != MZ_OK) {
        zipCloseFileInZip(zFile);
        printf("Failed to zip with zipOpenNewFileInZip2_64(...)\n");
        return ZIP_ERROR;
    }

    if (!isDir) {
        FILE *pFile = NULL;
        OpenFileWithMode(&pFile, &inputPath, "rb");

        const size_t bufSize = 1024;
        char buf[bufSize];

        bool stopWriting = false;
        do {
            size_t charsToWrite = fread(buf, 1, bufSize, pFile);

            if (charsToWrite != bufSize) {
                if (feof(pFile)) {
                    stopWriting = true;
                } else if (ferror(pFile)) {
                    perror("Error reading file to be written to zip\n");
                    returnCode = ZIP_ERROR;
                    break;
                }
            }

            status = zipWriteInFileInZip(zFile, buf, charsToWrite);
            if (status != MZ_OK) {
                perror("Failed to write to zip with zipWriteInFileInZip(...)");
                returnCode = ZIP_ERROR;
                break;
            }
        } while (!stopWriting);

        fclose(pFile);
    }

    zipCloseFileInZip(zFile);
    return returnCode;
}

#ifdef _WIN32
ZIP_RETURN_CODES ZipDirectory_Windows(zipFile zFile,
                                      const char *const pInputPath,
                                      uint8_t level) {
    ZIP_RETURN_CODES returnCode = ZIP_SUCCESS;

    // Add the direcotry to the zip
    returnCode = ZipPath(zFile, pInputPath, level);
    if (returnCode != ZIP_SUCCESS) {
        printf("Failed to zip: %s\n", pInputPath);
        return ZIP_ERROR;
    }

    RAII_STRING searchPath = RaiiStringCreateFromCString(pInputPath);
    RaiiStringAppend_cString(&searchPath, "*");

    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile(searchPath.pString, &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        printf("Failed to find first file in directory: %s\n", pInputPath);
        return ZIP_ERROR;
    }

    do {
        const char *const name = findFileData.cFileName;

        // Skip the special entries "." and ".."
        if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
            continue;
        }

        // Construct the full path of the current file or directory
        RAII_STRING fullPath = RaiiStringCreateFromCString(pInputPath);
        RaiiStringAppend_cString(&fullPath, name);

        // Check if the entry is a directory
        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // Directory found does not have a trailing '/' or '\\' by default
            RaiiStringAppend_cString(&fullPath, "/");
            returnCode =
                ZipDirectory_Windows(zFile, fullPath.pString, level + 1);
            if (returnCode != ZIP_SUCCESS) {
                printf("Failed to zip directory: %s\n", fullPath.pString);
                break;
            }
        } else {
            returnCode = ZipPath(zFile, fullPath.pString, level + 1);
            if (returnCode != ZIP_SUCCESS) {
                printf("Failed to zip file: %s\n", fullPath.pString);
                break;
            }
        }
    } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind);

    return returnCode;
}

#else

ZIP_RETURN_CODES ZipDirectory_Posix(zipFile zFile,
                                    const char *const pInputPath) {
    ZIP_RETURN_CODES returnCode = ZIP_SUCCESS;

    // TODO: Determine if pInputPath should be converted to an
    // absolute path.
    RAII_STRING inputPathCopy = RaiiStringCreateFromCString(pInputPath);
    char *const inputPathArray[] = {inputPathCopy.pString, NULL};
    FTS *pFileSystem =
        fts_open(&inputPathArray[0], FTS_COMFOLLOW | FTS_NOCHDIR, NULL);
    if (pFileSystem == NULL) {
        printf("Failed to open file system for: %s\n", pInputPath);
        return ZIP_ERROR;
    }

    FTSENT *node = fts_read(pFileSystem);
    while (node != NULL && returnCode == ZIP_SUCCESS) {
        const char *const pFpath = node->fts_path;
        const int level = node->fts_level;
        const int typeflag = node->fts_info;

        RAII_STRING fpath = RaiiStringCreateFromCString(pFpath);

        if (typeflag == FTS_D &&
            fpath.pString[fpath.lengthWithTermination - 2] != '/') {
            RaiiStringAppend_cString(&fpath, "/");
        }

        if (typeflag == FTS_F || typeflag == FTS_D) {
            returnCode = ZipPath(zFile, fpath.pString, level);
            if (returnCode != ZIP_SUCCESS) {
                printf("Failed to zip: %s\n", fpath.pString);
            }
        }

        node = fts_read(pFileSystem);
    }
    fts_close(pFileSystem);

    return returnCode;
}
#endif
