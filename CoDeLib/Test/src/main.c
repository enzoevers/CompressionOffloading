#include "unity_fixture.h"

#include "TestDeflateInflateZlib.h"
#include "TestFileUtils.h"
#include <CoDeLib/FileUtils/FileUtils.h>

static void RunAllTests(void) {
    RUN_TEST_GROUP(TestRaiiString);
    RUN_TEST_GROUP(TestDeflateInflateZlib);
    RUN_TEST_GROUP(TestFileUtils);
}

int main(int argc, const char **argv) {
    RAII_STRING fullPathToBenchmarkTestFiles;

    // Should end with '/'
    RAII_STRING currentWorkingDirectory;

    // TODO: use getopt(...)
    if (argc < 3) {
        printf("Not enough arguments provided\n");
        return 1;
    } else {
        fullPathToBenchmarkTestFiles = RaiiStringCreateFromCString(argv[1]);
        currentWorkingDirectory = RaiiStringCreateFromCString(argv[2]);
    }

    SetupTestDeflateInflateZlib(fullPathToBenchmarkTestFiles.pString);
    SetupTestFileUtils(fullPathToBenchmarkTestFiles.pString,
                       currentWorkingDirectory.pString);

    return UnityMain(argc, argv, RunAllTests);
}
