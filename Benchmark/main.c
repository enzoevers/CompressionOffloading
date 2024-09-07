#include "b63/b63.h"

B63_BASELINE(basic, n) {
    int i = 0, res = 0;
    for (i = 0; i < n; i++) {
        res += rand();
    }
    B63_KEEP(res);
}

B63_BENCHMARK(basic_half, n) {
    int i = 0, res = 0;
    for (i = 0; i < n; i += 2) {
        res += rand();
    }
    B63_KEEP(res);
}

int main(int argc, char **argv) {
    B63_RUN(argc, argv);
    return 0;
}