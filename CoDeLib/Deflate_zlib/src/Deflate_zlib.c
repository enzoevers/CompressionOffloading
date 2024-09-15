#include <CoDeLib/Deflate_zlib/Deflate_zlib.h>
#include <zlib.h>

const int Defalte(FILE *input, FILE *output, void *options) {
    return ZLIB_VERNUM;
}

const struct IDeflate deflate_zlib = {
    .Defalte = Defalte,
};
