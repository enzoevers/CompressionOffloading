#pragma once

#include <CoDeLib/RaiiString/RaiiString.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define MAX_PATH_LENGTH 256
#define MAX_PATH_LENGTH_WTH_TERMINATOR (MAX_PATH_LENGTH + 1)

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
@param pDirname The directory to delete. The directory needs to end in '/' or
'\\' ('\' escaped).
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
@brief Gets the absolute path of a path. If pPath is already an absolute path,
pAbsolutePath will be a copy of this string . This function is platform
agnostic.
@param pPath The path to get the absolute path of.
@param pAbsolutePath The buffer to place the absolute path in.
@param absolutePathSize The total size of the buffer.
@return true if the absolute path was successfully placed in pAbsolutePath,
false otherwise.
*/
bool GetAbsolutePath(const char *pPath, char *const pAbsolutePath,
                     const size_t absolutePathSize);

/*!
@brief Gets the current working directory and places it in pFullPath. If
fullPathLength is too small, the function will return NULL and nothing will be
placed in pFullPath. The path will appended with a '/' if not already present.
@param pFullPath The buffer to place the current working directory in.
@param fullPathLength The total size of the buffer.
@return pFullPath if successful, NULL otherwise.
*/
char *GetCurrentWorkingDirectory(char *pFullPath, size_t fullPathSize);

/*!
@brief Checks if a file or directory exists.
@param pPath The path to check. Can be a relative of absolute path. Can end in
either a directory or a file. A path to a directory is assumed to end in a '/'.
@return true if the file or directory exists, false otherwise.
*/
bool PathExists(const char *pPath);

/*!
@brief Extracts the last part of a path. If the last character of pPath is '/'
or '\\', the name of the last directory appended with '/' or '\\' respectively
will be returned. Otherwise, the name of the file (including extensions) will be
returned.
@param pPath The path to extract the last part of.
@param pDestBuffer The buffer to place the last part of the path in. This will
be the last part of the path as a null-terminated string. If pPath is NULL, if
the pPath string is empty, if the pPath string is only a separater, if the pPath
string is larger than MAX_PATH_LENGTH, or if destBufferSize is too small,
nothing will be done with pDestBuffer. If pPath contains only a filename or
directory name, a copy of the pPath string is assigned.
@param destBufferSize The total size of the buffer.
@return The position in pPath where the last part starts. On error, SIZE_MAX is
returned.
*/
size_t ExtractLastPartOfPath(const char *const pPath, char *const pDestBuffer,
                             size_t destBufferSize);

void OpenFileWithMode(FILE **pInFile, const RaiiString *const pFullPath,
                      const char *const pOpenMode);
uint64_t GetFileSizeInBytes(FILE *pFile);
bool FilesAreEqual(FILE *pFile1, FILE *pFile2);
