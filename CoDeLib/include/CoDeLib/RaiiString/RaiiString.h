#pragma once

#include <stddef.h>

/*!
Usage:

    RaiiString myString __attribute__((cleanup(RaiiStringClean)));
    myString = RaiiStringCreate(lengthPath);
*/

typedef struct {
    char *pString;

    // Length of the string (including the null terminator)
    size_t lengthWithTermination;
} RaiiString;

// Maximum length of a C string (including the null terminator)
static const size_t MAX_CSTRING_INCLUDING_TERMINATION_LENGTH = 1024;

RaiiString RaiiStringCreate(size_t length);
RaiiString RaiiStringCreateFromCString(const char *pCString);
void RaiiStringClean(RaiiString *pThis);
