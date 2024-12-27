#include <CoDeLib/FileUtils/FileUtils.h>
#include <CoDeLib/UnZip_minizip/UnZip_minizip.h>

// minizip
#include <mz.h>
#include <unzip.h>

//==============================
// Helper functions
//==============================

/*!
@brief Gets the filename that zfile is currently handling.
@param zfile The unzFile object.
@param pFInfo The unz_file_info64 object to be filled.
@return The filename that zfile is currently handling. The caller is responsible
for cleaning up the returned RaiiString.
*/
RaiiString _GetCurrentFilenameInZip(unzFile zfile, unz_file_info64 *pFInfo,
                                    bool *pIsutf8);

//==============================
// Interface functions
//==============================

UNZIP_RETURN_CODES
UnZip(ZipContentInfo *const pZipInfo, const RaiiString *const pOutputDirPath) {
    if (pZipInfo == NULL || pZipInfo->zipFilePath.pString == NULL ||
        pOutputDirPath == NULL || pOutputDirPath->pString == NULL) {
        return UNZIP_ERROR;
    }

    unzFile uzfile = unzOpen64(pZipInfo->zipFilePath.pString);
    if (uzfile == NULL) {
        printf("Could not open %s for unzipping\n",
               pZipInfo->zipFilePath.pString);
        return UNZIP_ERROR;
    }

    // TODO: Create a RaiiStringAppend_ function that returns a copy of the
    // new string. That way the following line can be moved outside the
    // loop.
    RAII_STRING localOutputDirPath =
        RaiiStringCreateFromCString(pOutputDirPath->pString);

    if (localOutputDirPath
            .pString[localOutputDirPath.lengthWithTermination - 2] != '/') {
        RaiiStringAppend_cString(&localOutputDirPath, "/");
    }

    if (!PathExists(localOutputDirPath.pString)) {
        RecursiveMkdir(localOutputDirPath.pString);
    }

    UNZIP_RETURN_CODES status = UNZIP_SUCCESS;
    do {
        // TODO: Create a RaiiStringAppend_ function that returns a copy of the
        // new string. That way the following line can be moved outside the
        // loop.
        RAII_STRING unZippedDestPath =
            RaiiStringCreateFromCString(localOutputDirPath.pString);

        // So far, pIsutf8 is not used and can thus be NULL
        unz_file_info64 fInfo;
        RAII_STRING currentFilename =
            _GetCurrentFilenameInZip(uzfile, &fInfo, NULL);
        if (currentFilename.pString == NULL) {
            status = UNZIP_ERROR;
            break;
        }

        if (currentFilename.lengthWithTermination == 1) {
            // Empty filename
            status = UNZIP_ERROR;
            break;
        }

        RaiiStringAppend_RaiiString(&unZippedDestPath, &currentFilename);
        ZipContentInfoAddUnzippedFilePath(pZipInfo, &unZippedDestPath);

        if (fInfo.uncompressed_size == 0 &&
            unZippedDestPath.pString[unZippedDestPath.lengthWithTermination -
                                     2] == '/') {
            // Is a directory
            RecursiveMkdir(unZippedDestPath.pString);
        } else {
            // Is a file
            const int raw = 1; // 1: unzip as raw data
            int unzOpenStatus = unzOpenCurrentFile2(uzfile, NULL, NULL, raw);
            if (unzOpenStatus != UNZ_OK) {
                status = UNZIP_ERROR;
                break;
            }

            // Note: It is important to open the file in binary mode.
            // Otherwise, a \r (cariage return) character will implicitely
            // add a \n (newline) character.
            FILE *pFile = fopen(unZippedDestPath.pString, "wb");
            if (pFile == NULL) {
                status = UNZIP_ERROR;
                break;
            }

            int readCount = 0;
            do {
                const uint32_t bufSize = 1024;
                char buffer[bufSize];
                readCount = unzReadCurrentFile(uzfile, buffer, bufSize - 1);

                // It is safe to index with `readCount`. Because it
                // will be at most `bufSize - 1`.
                buffer[readCount] = '\0';
                if (readCount < 0) {
                    status = UNZIP_ERROR;
                    break;
                }

                if (readCount > 0) {
                    fwrite(buffer, 1, readCount, pFile);
                }
            } while (readCount > 0);

            fclose(pFile);
            unzCloseCurrentFile(uzfile);
        }

        if (status != UNZIP_SUCCESS) {
            break;
        }

        int nextFileStatus = unzGoToNextFile(uzfile);
        if (nextFileStatus != MZ_OK) {
            // If the next file is not found all files are processed and the
            // loop will exit.
            break;
        }
    } while (status == UNZIP_SUCCESS);

    unzClose(uzfile);
    return status;
}

const struct IUnZip unzip_minizip = {
    .UnZip = UnZip,
};

//==============================
// Helper functions
//
// Based on code from:
// https://nachtimwald.com/2019/09/08/making-minizip-easier-to-use/
//==============================

RaiiString _GetCurrentFilenameInZip(unzFile zfile, unz_file_info64 *pFInfo,
                                    bool *pIsutf8) {
    RaiiString emptyString = (RaiiString){NULL, 0};

    if (zfile == NULL || pFInfo == NULL) {
        return emptyString;
    }

    char filename[MAX_PATH_LENGTH_WTH_TERMINATOR];

    int getFileInfoStatus = unzGetCurrentFileInfo64(
        zfile, pFInfo, &filename[0], sizeof(filename), NULL, 0, NULL, 0);
    if (getFileInfoStatus != MZ_OK) {
        return emptyString;
    }

    if (pIsutf8 != NULL) {
        *pIsutf8 = (pFInfo->flag & (1 << 11)) ? true : false;
    }

    return RaiiStringCreateFromCString(filename);
}
