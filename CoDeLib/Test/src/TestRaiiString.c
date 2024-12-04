#include "unity_fixture.h"
#include <CoDeLib/RaiiString/RaiiString.h>
#include <assert.h>
#include <string.h>

TEST_GROUP(TestRaiiString);

static RaiiString raiiString;

TEST_SETUP(TestRaiiString) {}

TEST_TEAR_DOWN(TestRaiiString) { RaiiStringClean(&raiiString); }

//==============================
// RaiiStringCreate()
//==============================

TEST(TestRaiiString, test_RaiiStringCreate_SetsCorrectsLengthInReturnedObject) {
    const size_t length = 10;
    raiiString = RaiiStringCreate(length);
    TEST_ASSERT_EQUAL(length, raiiString.lengthWithTermination);
}

TEST(TestRaiiString, test_RaiiStringCreate_SetsNonNullPointerInReturnedObject) {
    const size_t length = 10;
    raiiString = RaiiStringCreate(length);
    TEST_ASSERT_NOT_NULL(raiiString.pString);
}

TEST(TestRaiiString,
     test_RaiiStringCreate_SetsNullptrAndLengthZeroIfGivenLengthIsZero) {
    const size_t length = 0;
    raiiString = RaiiStringCreate(length);
    TEST_ASSERT_NULL(raiiString.pString);
    TEST_ASSERT_EQUAL(0, raiiString.lengthWithTermination);
}

TEST(TestRaiiString,
     test_RaiiStringCreate_SetsZeroLengthAndNullptrIfLengthIsMaxLength) {
    const size_t length = MAX_CSTRING_INCLUDING_TERMINATION_LENGTH;
    raiiString = RaiiStringCreate(length);
    TEST_ASSERT_NULL(raiiString.pString);
    TEST_ASSERT_EQUAL(0, raiiString.lengthWithTermination);
}

TEST(
    TestRaiiString,
    test_RaiiStringCreate_SetsZeroLengthAndNullptrIfLengthIsGreaterThanMaxLength) {
    const size_t length = MAX_CSTRING_INCLUDING_TERMINATION_LENGTH + 1;
    raiiString = RaiiStringCreate(length);
    TEST_ASSERT_NULL(raiiString.pString);
    TEST_ASSERT_EQUAL(0, raiiString.lengthWithTermination);
}

//==============================
// RaiiStringCreateFromCString()
//==============================

TEST(
    TestRaiiString,
    test_RaiiStringCreateFromCString_SetsLengthOfOneAndNonNullptrIfProvidedEmptyString) {
    const char *pCString = "";
    raiiString = RaiiStringCreateFromCString(pCString);
    TEST_ASSERT_NOT_NULL(raiiString.pString);
    TEST_ASSERT_EQUAL(1, raiiString.lengthWithTermination);
}

TEST(
    TestRaiiString,
    test_RaiiStringCreateFromCString_SetsExecptedLengthAndNonNullptrIfProvidedNormalString) {
    const char *pCString = "Hello, world!";
    raiiString = RaiiStringCreateFromCString(pCString);
    TEST_ASSERT_NOT_NULL(raiiString.pString);
    TEST_ASSERT_EQUAL(14, raiiString.lengthWithTermination);
}

TEST(TestRaiiString, test_RaiiStringCreateFromCString_SetsExecptedString) {
    const char *pCString = "Hello, world!";
    raiiString = RaiiStringCreateFromCString(pCString);
    TEST_ASSERT_EQUAL_STRING(pCString, raiiString.pString);
}

TEST(
    TestRaiiString,
    test_RaiiStringCreateFromCString_DoesNotUseProvidedPointerInReturnedObject) {
    const char *pCString = "";
    raiiString = RaiiStringCreateFromCString(pCString);
    TEST_ASSERT_NOT_EQUAL(pCString, raiiString.pString);
}

TEST(
    TestRaiiString,
    test_RaiiStringCreateFromCString_SetsZeroLengthAndNullptrIfLengthIsMaxLength) {
    const size_t lengthWithNullTermination =
        MAX_CSTRING_INCLUDING_TERMINATION_LENGTH + 1;
    char *pCString = (char *)malloc(lengthWithNullTermination * sizeof(char));
    assert(pCString != NULL && "Failed to allocate memory for pCString");
    memset(pCString, 'a', lengthWithNullTermination - 1);
    pCString[MAX_CSTRING_INCLUDING_TERMINATION_LENGTH] = '\0';

    raiiString = RaiiStringCreateFromCString(pCString);
    TEST_ASSERT_NULL(raiiString.pString);
    TEST_ASSERT_EQUAL(0, raiiString.lengthWithTermination);
}

TEST(
    TestRaiiString,
    test_RaiiStringCreateFromCString_SetsZeroLengthAndNullptrIfLengthIsGreaterThanMaxLength) {
    const size_t lengthWithNullTermination =
        MAX_CSTRING_INCLUDING_TERMINATION_LENGTH + 2;
    char *pCString = (char *)malloc(lengthWithNullTermination * sizeof(char));
    assert(pCString != NULL && "Failed to allocate memory for pCString");
    memset(pCString, 'a', lengthWithNullTermination - 1);
    pCString[lengthWithNullTermination - 1] = '\0';

    raiiString = RaiiStringCreateFromCString(pCString);
    TEST_ASSERT_NULL(raiiString.pString);
    TEST_ASSERT_EQUAL(0, raiiString.lengthWithTermination);
}

//==============================
// RaiiStringClean()
//==============================

TEST(TestRaiiString, test_RaiiStringClean_SetsNullptrInObject) {
    raiiString = RaiiStringCreate(10);
    RaiiStringClean(&raiiString);
    TEST_ASSERT_NULL(raiiString.pString);
}

TEST(TestRaiiString, test_RaiiStringClean_SetsLengthZeroInObject) {
    raiiString = RaiiStringCreate(10);
    RaiiStringClean(&raiiString);
    TEST_ASSERT_EQUAL(0, raiiString.lengthWithTermination);
}

TEST_GROUP_RUNNER(TestRaiiString) {
    // RaiiStringCreate()
    RUN_TEST_CASE(TestRaiiString,
                  test_RaiiStringCreate_SetsCorrectsLengthInReturnedObject);
    RUN_TEST_CASE(TestRaiiString,
                  test_RaiiStringCreate_SetsNonNullPointerInReturnedObject);
    RUN_TEST_CASE(
        TestRaiiString,
        test_RaiiStringCreate_SetsNullptrAndLengthZeroIfGivenLengthIsZero);
    RUN_TEST_CASE(
        TestRaiiString,
        test_RaiiStringCreate_SetsZeroLengthAndNullptrIfLengthIsMaxLength);
    RUN_TEST_CASE(
        TestRaiiString,
        test_RaiiStringCreate_SetsZeroLengthAndNullptrIfLengthIsGreaterThanMaxLength);

    // RaiiStringCreateFromCString()
    RUN_TEST_CASE(
        TestRaiiString,
        test_RaiiStringCreateFromCString_SetsLengthOfOneAndNonNullptrIfProvidedEmptyString);
    RUN_TEST_CASE(
        TestRaiiString,
        test_RaiiStringCreateFromCString_SetsExecptedLengthAndNonNullptrIfProvidedNormalString);
    RUN_TEST_CASE(TestRaiiString,
                  test_RaiiStringCreateFromCString_SetsExecptedString);
    RUN_TEST_CASE(
        TestRaiiString,
        test_RaiiStringCreateFromCString_DoesNotUseProvidedPointerInReturnedObject);
    RUN_TEST_CASE(
        TestRaiiString,
        test_RaiiStringCreateFromCString_SetsZeroLengthAndNullptrIfLengthIsMaxLength);
    RUN_TEST_CASE(
        TestRaiiString,
        test_RaiiStringCreateFromCString_SetsZeroLengthAndNullptrIfLengthIsGreaterThanMaxLength);

    // RaiiStringClean()
    RUN_TEST_CASE(TestRaiiString, test_RaiiStringClean_SetsNullptrInObject);
    RUN_TEST_CASE(TestRaiiString, test_RaiiStringClean_SetsLengthZeroInObject);
}
