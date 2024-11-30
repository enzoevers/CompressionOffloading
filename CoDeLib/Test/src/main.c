#include "unity_fixture.h"

#include "TestDeflateInflateZlib.h"
#include <CoDeLib/RaiiString/RaiiString.h>

static void RunAllTests(void) {
    RUN_TEST_GROUP(TestRaiiString);
    RUN_TEST_GROUP(TestDeflateInflateZlib);
}

int main(int argc, const char **argv) {
    RaiiString fullPathToBenchmarkTestFiles
        __attribute__((cleanup(RaiiStringClean)));

    // TODO: use getopt(...)
    if (argc == 1) {
        printf("No arguments provided\n");
        return 1;
    } else {
        fullPathToBenchmarkTestFiles = RaiiStringCreateFromCString(argv[1]);
    }

    SetupTestDeflateInflateZlib(fullPathToBenchmarkTestFiles.pString);

    return UnityMain(argc, argv, RunAllTests);
}
