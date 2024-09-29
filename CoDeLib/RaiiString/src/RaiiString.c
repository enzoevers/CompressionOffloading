#include <CoDeLib/RaiiString/RaiiString.h>
#include <stdio.h>
#include <stdlib.h>

RaiiString RaiiStringCreate(size_t length) {
    RaiiString newRaiistring = {NULL, length};
    newRaiistring.pString = (char *)calloc(length, sizeof(char));
    if (newRaiistring.pString == NULL) {
        newRaiistring.length = 0;
        printf("Failed to allocate memory for RaiiString\n");
        exit(1);
    }
    return newRaiistring;
}

void RaiiStringClean(RaiiString *pThis) {
    if (pThis->pString != NULL) {
        free(pThis->pString);
        pThis->pString = NULL;
    }
}
