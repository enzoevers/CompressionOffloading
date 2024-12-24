#pragma once

#include <CoDeLib/ZipContentInfo.h>
#include <stdio.h>

typedef enum { UNZIP_SUCCESS, UNZIP_ERROR } UNZIP_RETURN_CODES;

struct IUnZip {
    /*!
     * @brief Un-zips the input zip file and writes the output to the output
     * files.
     * @param pZipInfo The information about the zip file. The filenames in
     * pZipInfo will be filled by the unzipping function. The caller is
     * responsible for cleaning up the provided pZipInfo pointer.
     * @param pOutputDirPath The path to the directory where the unzipped files
     * will be stored. The directory must exist.
     * @return UNZIP_SUCCESS if the un-zipping was successful, UNZIP_ERROR
     * otherwise.
     */
    UNZIP_RETURN_CODES(*UnZip)
    (ZipContentInfo *const pZipInfo, const RaiiString *const pOutputDirPath);
};
