#pragma once

#include <CoDeLib/ZipContentInfo.h>
#include <stdio.h>

typedef enum { ZIP_SUCCESS, ZIP_ERROR } ZIP_RETURN_CODES;

struct IZip {
    /*!
     * @brief Zips the input file(s) and write the output to the output file.
     * @param pZipInfo The information about the zip file.  The caller is
     * responsible for cleaning up the provided pZipInfo pointer.
     * @return ZIP_SUCCESS if the zipping was successful, ZIP_ERROR
     * otherwise.
     */
    ZIP_RETURN_CODES (*Zip)(const ZipContentInfo *const pZipInfo);
};
