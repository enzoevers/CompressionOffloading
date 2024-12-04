#include <CoDeLib/RaiiString/RaiiString.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

RaiiString RaiiStringCreate(size_t lengthWithTermination) {
    RaiiString newRaiistring = {NULL, 0};

    if (lengthWithTermination == 0 ||
        lengthWithTermination >= MAX_CSTRING_INCLUDING_TERMINATION_LENGTH) {
        return newRaiistring;
    }

    newRaiistring.lengthWithTermination = lengthWithTermination;
    newRaiistring.pString = (char *)calloc(lengthWithTermination, sizeof(char));
    if (newRaiistring.pString == NULL) {
        newRaiistring.lengthWithTermination = 0;
        printf("Failed to allocate memory for RaiiString\n");
        exit(1);
    }
    return newRaiistring;
}

RaiiString RaiiStringCreateFromCString(const char *pCString) {
    const size_t length =
        strnlen(pCString, MAX_CSTRING_INCLUDING_TERMINATION_LENGTH);
    RaiiString newRaiistring = RaiiStringCreate(length + 1);

    if (newRaiistring.pString == NULL) {
        return newRaiistring;
    }

    strncpy(newRaiistring.pString, pCString,
            newRaiistring.lengthWithTermination - 1);
    newRaiistring.pString[newRaiistring.lengthWithTermination - 1] = '\0';
    return newRaiistring;
}

void RaiiStringClean(RaiiString *pThis) {
    if (pThis->pString != NULL) {
        free(pThis->pString);
        pThis->pString = NULL;
    }
    pThis->lengthWithTermination = 0;
}
