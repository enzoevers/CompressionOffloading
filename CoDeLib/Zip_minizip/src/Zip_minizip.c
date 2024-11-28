#include <CoDeLib/Zip_minizip/Zip_minizip.h>

#include <zip.h> // minizip

ZIP_RETURN_CODES Zip(const ZipContentInfo *const pZipInfo
                     __attribute__((unused))) {

    // TODO: Dummy code to test if it compiles and links
    if (ZIP_OK) {
        return ZIP_SUCCESS;
    }
    return ZIP_ERROR;
}

const struct IZip zip_minizip = {
    .Zip = Zip,
};
