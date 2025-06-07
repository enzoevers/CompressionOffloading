#include "unity_fixture.h"

#include "TestDeflateInflateZlib.h"
#include "TestFileUtils.h"
#include "TestUnZipMinizip.h"
#include "TestUnZipMinizipInflateZlib.h"
#include "TestZipMinizip.h"
#include "TestZipMinizipUnZipMinizip.h"
#include <CoDeLib/FileUtils/FileUtils.h>
#include <CoDeLib/RaiiString/RaiiString.h>
#include <string.h>

static void RunAllTests(void) {
    RUN_TEST_GROUP(TestRaiiString);
    RUN_TEST_GROUP(TestDeflateInflateZlib);
    RUN_TEST_GROUP(TestFileUtils);
    RUN_TEST_GROUP(TestUnZipMinizip);
    RUN_TEST_GROUP(TestZipMinizip);
    RUN_TEST_GROUP(TestUnZipMinizipInflateZlib);
    RUN_TEST_GROUP(TestZipMinizipUnZipMinizip);
    RUN_TEST_GROUP(TestZipContentInfo);
}

int main(int argc, const char **argv) {
    RAII_STRING fullPathToBenchmarkTestFiles = {NULL, 0};

    // Should end with '/'
    RAII_STRING currentWorkingDirectory = {NULL, 0};
    bool runLongTests = false;

    // TODO: use getopt(...)
    if (argc < 4) {
        printf("Not enough arguments provided\n");
        return 1;
    } else {
        RAII_STRING unNormalizedFullPathToBenchmarkTestFiles =
            RaiiStringCreateFromCString(argv[1]);
        fullPathToBenchmarkTestFiles =
            RaiiStringCreateFromCString(NormailizePathSeparatorsInPlace(
                unNormalizedFullPathToBenchmarkTestFiles.pString));

        RAII_STRING unNormalizedCurrentWorkingDirectory =
            RaiiStringCreateFromCString(argv[2]);
        currentWorkingDirectory =
            RaiiStringCreateFromCString(NormailizePathSeparatorsInPlace(
                unNormalizedCurrentWorkingDirectory.pString));

        runLongTests = strcmp(argv[3], "true") == 0;
    }

    SetupTestDeflateInflateZlib(fullPathToBenchmarkTestFiles.pString);
    SetupTestFileUtils(fullPathToBenchmarkTestFiles.pString,
                       currentWorkingDirectory.pString, runLongTests);
    SetupTestUnZipMinizip(fullPathToBenchmarkTestFiles.pString);
    SetupTestZipMinizip(fullPathToBenchmarkTestFiles.pString);
    SetupTestUnZipMinizipInflateZlib(fullPathToBenchmarkTestFiles.pString);
    SetupTestZipMinizipUnZipMinizip(fullPathToBenchmarkTestFiles.pString,
                                    runLongTests);

    return UnityMain(argc, argv, RunAllTests);
}
