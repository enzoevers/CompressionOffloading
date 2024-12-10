#pragma once

#include <CoDeLib/RaiiString/RaiiString.h>
#include <stdbool.h>
#include <stdio.h>

#define MAX_PATH_LENGTH_SIZE 256

/*!
@brief Recursively creates a directory. If the directory already exists, nothing
happens.
@param pDirname The directory to create. The directory needs to end in '/'.
@return true if the directory was created or already exists, false otherwise.
*/
bool RecursiveMkdir(const char *const pDirname);

/*!
@brief Recursively deletes a directory and all files. If the directory does not
exist, nothing happens.
@param pDirname The directory to delete. The directory needs to end in '/'.
@return true if the directory was deleted or did not exist, false otherwise.
*/
bool RecursiveRmdir(const char *const pDirname);

/*!
@brief Checks if a path is absolute. This function is platform agnostic.
@param pPath The path to check.
@return true if the path is absolute, false otherwise.
*/
bool IsAbsolutePath(const char *pPath);

/*!
@brief Gets the current working directory and places it in pFullPath. If
fullPathLength is too small, the function will return NULL and nothing will be
placed in pFullPath. The path will appended with a '/' if not already present.
@param pFullPath The buffer to place the current working directory in.
@param fullPathLength The total size of the buffer.
@return pFullPath if successful, NULL otherwise.
*/
char *GetCurrentWorkingDirectory(char *pFullPath, size_t fullPathSize);

void OpenFileWithMode(FILE **pInFile, RaiiString *pFullPath, char *pOpenMode);
size_t GetFileSizeInBytes(FILE *pFile);
bool FilesAreEqual(FILE *pFile1, FILE *pFile2);
