#include "unity_fixture.h"

#include "TestDeflateInflateZlib.h"
#include "TestFileUtils.h"
#include "TestUnZipMinizip.h"
#include "TestUnZipMinizipInflateZlib.h"
#include <CoDeLib/FileUtils/FileUtils.h>
#include <CoDeLib/RaiiString/RaiiString.h>

static void RunAllTests(void) {
    RUN_TEST_GROUP(TestRaiiString);
    RUN_TEST_GROUP(TestDeflateInflateZlib);
    RUN_TEST_GROUP(TestFileUtils);
    RUN_TEST_GROUP(TestUnZipMinizip);
    RUN_TEST_GROUP(TestUnZipMinizipInflateZlib);
    RUN_TEST_GROUP(TestZipContentInfo);
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
    SetupTestUnZipMinizip(fullPathToBenchmarkTestFiles.pString);
    SetupTestUnZipMinizipInflateZlib(fullPathToBenchmarkTestFiles.pString);

    return UnityMain(argc, argv, RunAllTests);
}
