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
@return The filename that zfile is currently handling. The caller is responsible
for cleaning up the returned RaiiString.
*/
RaiiString GetCurrentFilenameInZip(unzFile zfile, bool *pIsutf8);

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

    UNZIP_RETURN_CODES status = UNZIP_SUCCESS;
    do {
        // TODO: Create a RaiiStringAppend_ function that returns a copy of the
        // new string. That way the following line can be moved outside the
        // loop.
        RAII_STRING unZippedDestPath =
            RaiiStringCreateFromCString(pOutputDirPath->pString);

        // TODO: Check of the path ends in a '/' and ad.
        if (unZippedDestPath
                .pString[unZippedDestPath.lengthWithTermination - 2] != '/') {
            RaiiStringAppend_cString(&unZippedDestPath, "/");
        }

        // So far, pIsutf8 is not used and can thus be NULL
        RAII_STRING currentFilename = GetCurrentFilenameInZip(uzfile, NULL);
        if (currentFilename.pString == NULL) {
            status = UNZIP_ERROR;
            break;
        }

        RaiiStringAppend_RaiiString(&unZippedDestPath, &currentFilename);
        ZipContentInfoAddUnzippedFilePath(pZipInfo, &unZippedDestPath);

        int nextFileStatus = unzGoToNextFile(uzfile);

        if (nextFileStatus != MZ_OK) {
            status = UNZIP_ERROR;
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

RaiiString GetCurrentFilenameInZip(unzFile zfile, bool *pIsutf8) {
    RaiiString emptyString = (RaiiString){NULL, 0};

    if (zfile == NULL) {
        return emptyString;
    }

    char filename[128];
    unz_file_info64 fInfo;

    int getFileInfoStatus = unzGetCurrentFileInfo64(
        zfile, &fInfo, &filename[0], sizeof(filename), NULL, 0, NULL, 0);
    if (getFileInfoStatus != MZ_OK) {
        return emptyString;
    }

    if (pIsutf8 != NULL) {
        *pIsutf8 = (fInfo.flag & (1 << 11)) ? true : false;
    }

    return RaiiStringCreateFromCString(filename);
}
