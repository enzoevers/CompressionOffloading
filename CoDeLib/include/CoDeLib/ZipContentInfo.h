#pragma once

#include <CoDeLib/RaiiString/RaiiString.h>

// clang-format off
/*!
Usage example:

        RAII_STRING zipFilePath = RaiiStringCreateFromCString("SomePath/myZip.zip");
        RAII_ZIPCONTENTINFO zipContentInfo = ZipContentInfoCreate(&zipFilePath);
*/
// clang-format on

#define RAII_ZIPCONTENTINFO                                                    \
    ZipContentInfo __attribute__((cleanup(ZipContentInfoClean)))

typedef struct {
    // The name of the zip file.
    RaiiString zipFilePath;

    // The pointer to an array with all the files names in the zip file.
    // The names are stored with any (sub)directories in the name.
    //
    // When unzipping the zip file, the files array is created and filled by the
    // unzipping function.
    RaiiString *pUnZippedFilePathArray;

    // The number of files in the zip file.
    //
    // When unzipping the zip file, the fileCount is set by the unzipping
    // function.
    size_t unZippedFileCount;
} ZipContentInfo;

/*!
@brief Creates a ZipContentInfo object with the given zip file path.
@param pZipFilePath The path to the zip file. A deep copy is made and stored in
zipFilePath.
@return The ZipContentInfo object.
*/
ZipContentInfo ZipContentInfoCreate(const RaiiString *const pZipFilePath);

/*!
@brief Cleans the ZipContentInfo object. All RaiiString objects are
cleaned/freed. Vaues are set to NULL/0.
@param pZipInfo The ZipContentInfo object.
*/
void ZipContentInfoClean(ZipContentInfo *pZipInfo);

/*!
@brief Adds a file path to the list of unzipped files.
@param pZipInfo The ZipContentInfo object.
@param pFileName The file path to add. A deep copy is made and stored in
zipFilePath.
@return true if the file path was added successfully, false otherwise.
*/
bool ZipContentInfoAddUnzippedFilePath(ZipContentInfo *const pZipInfo,
                                       const RaiiString *const pFileName);
