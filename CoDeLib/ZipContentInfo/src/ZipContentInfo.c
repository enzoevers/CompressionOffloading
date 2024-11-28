#include <CoDeLib/ZipContentInfo.h>
#include <stdlib.h>

ZipContentInfo ZipContentInfoCreate(const RaiiString *const pZipFilePath) {
    ZipContentInfo zipInfo = (ZipContentInfo){.zipFilePath = {NULL, 0},
                                              .pUnZippedFilePathArray = NULL,
                                              .unZippedFileCount = 0};

    if (pZipFilePath == NULL) {
        return zipInfo;
    }

    // TODO: Use the copy function from RaiiString once it is created.
    zipInfo.zipFilePath = RaiiStringCreateFromCString(pZipFilePath->pString);

    return zipInfo;
}

void ZipContentInfoClean(ZipContentInfo *pZipInfo) {
    if (pZipInfo == NULL) {
        return;
    }

    RaiiStringClean(&(pZipInfo->zipFilePath));

    if (pZipInfo->pUnZippedFilePathArray != NULL) {
        for (size_t i = 0; i < pZipInfo->unZippedFileCount; ++i) {
            RaiiStringClean(&(pZipInfo->pUnZippedFilePathArray[i]));
        }

        free(pZipInfo->pUnZippedFilePathArray);
        pZipInfo->pUnZippedFilePathArray = NULL;
    }

    pZipInfo->unZippedFileCount = 0;
}

bool ZipContentInfoAddUnzippedFilePath(ZipContentInfo *const pZipInfo,
                                       const RaiiString *const pFileName) {
    if (pZipInfo == NULL || pZipInfo->zipFilePath.pString == NULL ||
        pFileName == NULL || pFileName->pString == NULL) {
        return false;
    }

    const size_t newCount = pZipInfo->unZippedFileCount + 1;
    RaiiString *pNewArray = NULL;

    if (pZipInfo->pUnZippedFilePathArray == NULL) {
        pNewArray = (RaiiString *)calloc(1, sizeof(RaiiString));
    } else {
        pNewArray = (RaiiString *)realloc(pZipInfo->pUnZippedFilePathArray,
                                          newCount * sizeof(RaiiString));
    }

    if (pNewArray == NULL) {
        return false;
    }

    // TODO: Simply paths by removing redundant '/'.
    // TODO: Use the copy function from RaiiString once it is created.
    pNewArray[newCount - 1] = RaiiStringCreateFromCString(pFileName->pString);

    pZipInfo->pUnZippedFilePathArray = pNewArray;
    pZipInfo->unZippedFileCount = newCount;

    return true;
}
