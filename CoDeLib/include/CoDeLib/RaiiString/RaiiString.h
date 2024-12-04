#pragma once

#include <stdbool.h>
#include <stddef.h>

/*!
Usage:

    RAII_STRING myString = RaiiStringCreateFromCString("Hello, world!");
*/

#define RAII_STRING RaiiString __attribute__((cleanup(RaiiStringClean)))

typedef struct {
    char *pString;

    // Length of the string (including the null terminator)
    size_t lengthWithTermination;
} RaiiString;

// Maximum length of a C string (including the null terminator)
static const size_t MAX_CSTRING_INCLUDING_TERMINATION_LENGTH = 1024;

/*!
@brief Creates a RaiiString object from a C string. The provided C string is
copied and not altered.
*/
RaiiString RaiiStringCreateFromCString(const char *pCString);
void RaiiStringClean(RaiiString *pThis);

bool RaiiStringAppend_RaiiString(RaiiString *pThis, const RaiiString *pOther);
bool RaiiStringAppend_cString(RaiiString *pThis, const char *pOther);
