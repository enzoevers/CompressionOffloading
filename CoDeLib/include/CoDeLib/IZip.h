#pragma once

#include <CoDeLib/ZipContentInfo.h>
#include <stdio.h>

typedef enum { ZIP_SUCCESS, ZIP_ERROR } ZIP_RETURN_CODES;

struct IZip {
    /*!
     * @brief Zips the input file(s) and write the output to the output zip
     * file.
     * @param pOutputZipPath The path to the output zip file. If the directory
     * does not exist, it will be created.
     * @param pInputPathArray The array of paths to the files or directories to
     * be zipped. Both absolute and relative paths are supported. Path to
     * directories will zip all files and sub-directories in the directory. Path
     * to files will zip the file without any directories. The array can be a
     * mix of absolute, relative, directies and files.
     * @param inputPathArraySize The number of elements in pInputPathArray.
     * @return ZIP_SUCCESS if the zipping was successful, ZIP_ERROR
     * otherwise.
     */
    ZIP_RETURN_CODES(*Zip)
    (const char *pOutputZipPath, const char **pInputPathArray,
     const size_t inputPathArraySize);
};
