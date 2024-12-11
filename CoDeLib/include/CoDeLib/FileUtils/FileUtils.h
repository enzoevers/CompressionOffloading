#pragma once

#include <CoDeLib/RaiiString/RaiiString.h>
#include <stdbool.h>
#include <stdio.h>

/*!
@brief Recursively creates a directory. If the directory already exists, nothing
happens.
@param pDirname The directory to create. The directory needs to end in '/'.
@return true if the directory was created or already exists, false otherwise.
*/
bool RecursiveMkdir(const char *pDirname);

/*!
@brief Recursively deletes a directory and all files. If the directory does not
exist, nothing happens.
@param pDirname The directory to delete. The directory needs to end in '/'.
@return true if the directory was deleted or did not exist, false otherwise.
*/
bool RecursiveRmdir(const char *pDirname);

void OpenFileWithMode(FILE **pInFile, RaiiString *pFullPath, char *pOpenMode);
size_t GetFileSizeInBytes(FILE *pFile);
bool FilesAreEqual(FILE *pFile1, FILE *pFile2);
