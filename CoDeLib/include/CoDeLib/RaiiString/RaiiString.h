#pragma once

#include <stddef.h>

/*!
Usage:

    RaiiString myString __attribute__((cleanup(RaiiStringClean)));
    myString = RaiiStringCreate(lengthPath);
*/

typedef struct {
    char *pString;
    size_t length;
} RaiiString;

RaiiString RaiiStringCreate(size_t length);
void RaiiStringClean(RaiiString *pThis);