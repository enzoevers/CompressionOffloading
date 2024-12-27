#include <CoDeLib/RaiiString/RaiiString.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

RaiiString RaiiStringCreateFromCString(const char *pCString) {
    const size_t lengthWithoutTermination =
        strnlen(pCString, MAX_CSTRING_INCLUDING_TERMINATION_LENGTH);

    RaiiString newRaiistring = {NULL, 0};

    if (lengthWithoutTermination == MAX_CSTRING_INCLUDING_TERMINATION_LENGTH) {
        return newRaiistring;
    }

    const size_t lengthWithTermination = lengthWithoutTermination + 1;

    newRaiistring.lengthWithTermination = lengthWithTermination;
    newRaiistring.pString = (char *)calloc(lengthWithTermination, sizeof(char));

    if (newRaiistring.pString == NULL) {
        newRaiistring.lengthWithTermination = 0;
        printf("Failed to allocate memory for RaiiString\n");
        exit(1);
    }

    strncpy(newRaiistring.pString, pCString,
            newRaiistring.lengthWithTermination - 1);
    newRaiistring.pString[newRaiistring.lengthWithTermination - 1] = '\0';

    return newRaiistring;
}

void RaiiStringClean(RaiiString *pThis) {
    if (pThis == NULL) {
        return;
    }

    if (pThis->pString != NULL) {
        free(pThis->pString);
        pThis->pString = NULL;
    }
    pThis->lengthWithTermination = 0;
}

bool RaiiStringAppend_RaiiString(RaiiString *pThis,
                                 const RaiiString *pStringToAppend) {
    const bool pointersAreValid =
        (pThis != NULL && pThis->pString != NULL) &&
        (pStringToAppend != NULL && pStringToAppend->pString != NULL);

    if (!pointersAreValid) {
        return false;
    }

    // Subtract one to account for the redundant null-terminator
    const size_t newLengthWithTermination =
        pThis->lengthWithTermination - 1 +
        pStringToAppend->lengthWithTermination;

    if (newLengthWithTermination > MAX_CSTRING_INCLUDING_TERMINATION_LENGTH) {
        return false;
    }

    char *pNewString = (char *)realloc(pThis->pString,
                                       newLengthWithTermination * sizeof(char));
    if (pNewString == NULL) {
        return false;
    }

    // This method takes into account that the strings are appended to
    // themselves.
    memmove(&pNewString[pThis->lengthWithTermination - 1],
            pStringToAppend->pString,
            (pStringToAppend->lengthWithTermination - 1) * sizeof(char));
    pNewString[newLengthWithTermination - 1] = '\0';

    pThis->pString = pNewString;
    pThis->lengthWithTermination = newLengthWithTermination;

    return true;
}

bool RaiiStringAppend_cString(RaiiString *pThis, const char *pOther) {
    RAII_STRING other = RaiiStringCreateFromCString(pOther);
    if (other.pString == NULL) {
        return false;
    }
    return RaiiStringAppend_RaiiString(pThis, &other);
}
