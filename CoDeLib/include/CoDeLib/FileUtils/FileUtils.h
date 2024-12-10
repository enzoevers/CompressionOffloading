#pragma once

#include <CoDeLib/RaiiString/RaiiString.h>
#include <stdbool.h>
#include <stdio.h>

void OpenFile(FILE **pInFile, RaiiString *pFullPath, char *pOpenMode);
size_t GetFileSizeInBytes(FILE *pFile);
bool FilesAreEqual(FILE *pFile1, FILE *pFile2);
