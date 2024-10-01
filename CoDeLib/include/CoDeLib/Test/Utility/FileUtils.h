#pragma once

#include <CoDeLib/RaiiString/RaiiString.h>
#include <stdbool.h>
#include <stdio.h>

void CreateFullPathToFile(const RaiiString *pFullPath,
                          const size_t maxFullPathStringSize,
                          const char *pBasePath, const char *pFileName);
void OpenFile(FILE **pInFile, char *basePath, char *pFilename, char *pOpenMode);
size_t GetFileSizeInBytes(FILE *pFile);
bool FilesAreEqual(FILE *pFile1, FILE *pFile2);