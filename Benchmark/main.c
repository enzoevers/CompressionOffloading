#include "b63/b63.h"
#include <CoDeLib/Deflate_zlib/Deflate_zlib.h>
#include <assert.h>

B63_BASELINE(basic, n) {
    const int status = deflate_zlib.Defalte(NULL, NULL, NULL);
    assert(status == 0x1311);
    B63_KEEP(status);
}

B63_BENCHMARK(same_bit_different, n) {
    const int status = deflate_zlib.Defalte(NULL, NULL, NULL) + 1;
    assert(status == 0x1312);
    B63_KEEP(status);
}

int main(int argc, char **argv) {
    B63_RUN(argc, argv);
    return 0;
}