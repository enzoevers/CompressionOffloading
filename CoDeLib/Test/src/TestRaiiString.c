#include "unity_fixture.h"
#include <CoDeLib/RaiiString/RaiiString.h>
#include <assert.h>
#include <string.h>

TEST_GROUP(TestRaiiString);

static RaiiString raiiString;

TEST_SETUP(TestRaiiString) {}

TEST_TEAR_DOWN(TestRaiiString) { RaiiStringClean(&raiiString); }

//==============================
// RaiiStringCreateFromCString(...)
//==============================

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
    const char *pCString = "a";
    raiiString = RaiiStringCreateFromCString(pCString);
    TEST_ASSERT_NOT_EQUAL(pCString, raiiString.pString);
}

TEST(
    TestRaiiString,
    test_RaiiStringCreateFromCString_SetsExpectedLengthAndPtrIfLengthTotalIsMaxTotalLength) {
    const size_t lengthWithNullTermination =
        MAX_CSTRING_INCLUDING_TERMINATION_LENGTH;
    char *pCString = (char *)malloc(lengthWithNullTermination * sizeof(char));
    assert(pCString != NULL && "Failed to allocate memory for pCString");
    memset(pCString, 'a', lengthWithNullTermination - 1);
    pCString[lengthWithNullTermination - 1] = '\0';

    raiiString = RaiiStringCreateFromCString(pCString);
    TEST_ASSERT_NOT_NULL(raiiString.pString);
    TEST_ASSERT_EQUAL_STRING(pCString, raiiString.pString);
    TEST_ASSERT_EQUAL(lengthWithNullTermination,
                      raiiString.lengthWithTermination);

    free(pCString);
}

TEST(
    TestRaiiString,
    test_RaiiStringCreateFromCString_SetsZeroLengthAndNullptrIfStringLengthIsMaxTotalLength) {
    const size_t lengthWithNullTermination =
        MAX_CSTRING_INCLUDING_TERMINATION_LENGTH + 1;
    char *pCString = (char *)malloc(lengthWithNullTermination * sizeof(char));
    assert(pCString != NULL && "Failed to allocate memory for pCString");
    memset(pCString, 'a', lengthWithNullTermination - 1);
    pCString[lengthWithNullTermination - 1] = '\0';

    raiiString = RaiiStringCreateFromCString(pCString);
    TEST_ASSERT_NULL(raiiString.pString);
    TEST_ASSERT_EQUAL(0, raiiString.lengthWithTermination);

    free(pCString);
}

TEST(
    TestRaiiString,
    test_RaiiStringCreateFromCString_SetsZeroLengthAndNullptrIfStringLengthIsGreaterThanMaxTotalLength) {
    const size_t lengthWithNullTermination =
        MAX_CSTRING_INCLUDING_TERMINATION_LENGTH + 2;
    char *pCString = (char *)malloc(lengthWithNullTermination * sizeof(char));
    assert(pCString != NULL && "Failed to allocate memory for pCString");
    memset(pCString, 'a', lengthWithNullTermination - 1);
    pCString[lengthWithNullTermination - 1] = '\0';

    raiiString = RaiiStringCreateFromCString(pCString);
    TEST_ASSERT_NULL(raiiString.pString);
    TEST_ASSERT_EQUAL(0, raiiString.lengthWithTermination);

    free(pCString);
}

TEST(
    TestRaiiString,
    test_RaiiStringCreateFromCString_SetsExpectedLengthAndPtrIfProvidedEmptyString) {
    const char *pCString = "";
    raiiString = RaiiStringCreateFromCString(pCString);
    TEST_ASSERT_NOT_NULL(raiiString.pString);
    TEST_ASSERT_EQUAL(1, raiiString.lengthWithTermination);
}

//==============================
// RaiiStringClean(...)
//==============================

TEST(TestRaiiString, test_RaiiStringClean_DoesNotCrashIfObjectIsNull) {
    RaiiStringClean(NULL);
}

TEST(TestRaiiString, test_RaiiStringClean_SetsNullptrInObject) {
    raiiString = RaiiStringCreateFromCString("Hello, world!");
    RaiiStringClean(&raiiString);
    TEST_ASSERT_NULL(raiiString.pString);
}

TEST(TestRaiiString, test_RaiiStringClean_SetsLengthZeroInObject) {
    raiiString = RaiiStringCreateFromCString("Hello, world!");
    RaiiStringClean(&raiiString);
    TEST_ASSERT_EQUAL(0, raiiString.lengthWithTermination);
}

//==============================
// RAII_STRING
//==============================

// Do no optimize to avoid -Werror=dangling-pointer=
#if defined(__GNUC__) && !defined(__clang__)
__attribute__((optimize("O0")))
#elif defined(__clang__)
__attribute__((optnone))
#endif
TEST(TestRaiiString, test_RAII_STRING_MacroWorksInScope) {
    char **pointerToScopedRaiiStringString = NULL;
    {
        RAII_STRING myString = RaiiStringCreateFromCString("Hello, world!");
        pointerToScopedRaiiStringString = &myString.pString;

        TEST_ASSERT_NOT_NULL(pointerToScopedRaiiStringString);
        TEST_ASSERT_NOT_NULL(*pointerToScopedRaiiStringString);
        TEST_ASSERT_EQUAL_STRING("Hello, world!",
                                 *pointerToScopedRaiiStringString);
    }

    if (pointerToScopedRaiiStringString != NULL) {
        TEST_ASSERT_NULL(*pointerToScopedRaiiStringString);
    } else {
        TEST_FAIL_MESSAGE("pointerToScopedRaiiStringString is NULL");
    }
}

//==============================
// RaiiStringAppend_RaiiString(...)
//==============================

TEST(TestRaiiString,
     test_RaiiStringAppend_RaiiString_ReturnsFalseIfDstStringIsNull) {
    raiiString = RaiiStringCreateFromCString("Hello, world!");
    bool appended = RaiiStringAppend_RaiiString(NULL, &raiiString);
    TEST_ASSERT_FALSE(appended);
}

TEST(TestRaiiString,
     test_RaiiStringAppend_RaiiString_ReturnsFalseIfInternalStringOfDstIsNull) {
    raiiString = RaiiStringCreateFromCString("Hello, ");
    RAII_STRING stringToAppend = RaiiStringCreateFromCString("world!");
    raiiString.pString = NULL;

    bool appended = RaiiStringAppend_RaiiString(&raiiString, &stringToAppend);

    TEST_ASSERT_FALSE(appended);
}

TEST(
    TestRaiiString,
    test_RaiiStringAppend_RaiiString_ReturnsFalseIfInternalStringOfStringToAppendIsNull) {
    raiiString = RaiiStringCreateFromCString("Hello, ");
    RAII_STRING stringToAppend = RaiiStringCreateFromCString("world!");
    stringToAppend.pString = NULL;

    bool appended = RaiiStringAppend_RaiiString(&raiiString, &stringToAppend);

    TEST_ASSERT_FALSE(appended);
}

TEST(TestRaiiString,
     test_RaiiStringAppend_RaiiString_ReturnsFalseIfStringToAppendIsNull) {
    raiiString = RaiiStringCreateFromCString("Hello, world!");
    bool appended = RaiiStringAppend_RaiiString(&raiiString, NULL);
    TEST_ASSERT_FALSE(appended);
}

TEST(TestRaiiString, test_RaiiStringAppend_RaiiString_AppendsStringToDst) {
    raiiString = RaiiStringCreateFromCString("Hello, ");
    RAII_STRING stringToAppend = RaiiStringCreateFromCString("world!");

    bool appended = RaiiStringAppend_RaiiString(&raiiString, &stringToAppend);

    TEST_ASSERT_TRUE(appended);
    TEST_ASSERT_EQUAL_STRING("Hello, world!", raiiString.pString);
    TEST_ASSERT_EQUAL(14, raiiString.lengthWithTermination);
}

TEST(TestRaiiString,
     test_RaiiStringAppend_RaiiString_HandlesAppendingToSelfCorrectly) {
    raiiString = RaiiStringCreateFromCString("Hello");

    bool appended = RaiiStringAppend_RaiiString(&raiiString, &raiiString);

    TEST_ASSERT_TRUE(appended);
    TEST_ASSERT_EQUAL_STRING("HelloHello", raiiString.pString);
    TEST_ASSERT_EQUAL(11, raiiString.lengthWithTermination);
}

TEST(TestRaiiString,
     test_RaiiStringAppend_RaiiString_StringToAppendIsUnchanged) {
    raiiString = RaiiStringCreateFromCString("Hello, ");
    RAII_STRING stringToAppend = RaiiStringCreateFromCString("world!");

    RaiiStringAppend_RaiiString(&raiiString, &stringToAppend);

    TEST_ASSERT_EQUAL_STRING("world!", stringToAppend.pString);
    TEST_ASSERT_EQUAL(7, stringToAppend.lengthWithTermination);
}

TEST(
    TestRaiiString,
    test_RaiiStringAppend_RaiiString_ReturnsFalseAndDoesntChangeStringsIfTotalLengthIsGreaterThanMaxLengthWithTermination) {

    // Base string
    const size_t lengthWithNullTermination_baseString =
        MAX_CSTRING_INCLUDING_TERMINATION_LENGTH - 10;
    char *pCString_baseString =
        (char *)malloc(lengthWithNullTermination_baseString * sizeof(char));
    assert(pCString_baseString != NULL &&
           "Failed to allocate memory for pCString_baseString");
    memset(pCString_baseString, 'a', lengthWithNullTermination_baseString - 1);
    pCString_baseString[lengthWithNullTermination_baseString - 1] = '\0';
    raiiString = RaiiStringCreateFromCString(pCString_baseString);

    // String to append
    const size_t lengthWithNullTermination_appendString = 12;
    char *pCString_appendString =
        (char *)malloc(lengthWithNullTermination_appendString * sizeof(char));
    assert(pCString_appendString != NULL &&
           "Failed to allocate memory for pCString_appendString");
    memset(pCString_appendString, 'a',
           lengthWithNullTermination_appendString - 1);
    pCString_appendString[lengthWithNullTermination_appendString - 1] = '\0';
    RAII_STRING appendRaiiString =
        RaiiStringCreateFromCString(pCString_appendString);

    // The test
    bool appendStatus =
        RaiiStringAppend_RaiiString(&raiiString, &appendRaiiString);

    TEST_ASSERT_FALSE(appendStatus);
    TEST_ASSERT_EQUAL_STRING(pCString_baseString, raiiString.pString);
    TEST_ASSERT_EQUAL(lengthWithNullTermination_baseString,
                      raiiString.lengthWithTermination);
    TEST_ASSERT_EQUAL_STRING(pCString_appendString, appendRaiiString.pString);
    TEST_ASSERT_EQUAL(lengthWithNullTermination_appendString,
                      appendRaiiString.lengthWithTermination);

    free(pCString_baseString);
    free(pCString_appendString);
}

//==============================
// RaiiStringAppend_cString(...)
//==============================

// Currently, this function only uses already tested functions for RaiiString.

//==============================
// TEST_GROUP_RUNNER
//==============================

TEST_GROUP_RUNNER(TestRaiiString) {
    // RaiiStringCreateFromCString()
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
        test_RaiiStringCreateFromCString_SetsExpectedLengthAndPtrIfLengthTotalIsMaxTotalLength);
    RUN_TEST_CASE(
        TestRaiiString,
        test_RaiiStringCreateFromCString_SetsZeroLengthAndNullptrIfStringLengthIsMaxTotalLength);
    RUN_TEST_CASE(
        TestRaiiString,
        test_RaiiStringCreateFromCString_SetsZeroLengthAndNullptrIfStringLengthIsGreaterThanMaxTotalLength);
    RUN_TEST_CASE(
        TestRaiiString,
        test_RaiiStringCreateFromCString_SetsExpectedLengthAndPtrIfProvidedEmptyString);

    // RaiiStringClean()
    RUN_TEST_CASE(TestRaiiString,
                  test_RaiiStringClean_DoesNotCrashIfObjectIsNull);
    RUN_TEST_CASE(TestRaiiString, test_RaiiStringClean_SetsNullptrInObject);
    RUN_TEST_CASE(TestRaiiString, test_RaiiStringClean_SetsLengthZeroInObject);

    // RAII_STRING
    RUN_TEST_CASE(TestRaiiString, test_RAII_STRING_MacroWorksInScope);

    // RaiiStringAppend()
    RUN_TEST_CASE(
        TestRaiiString,
        test_RaiiStringAppend_RaiiString_ReturnsFalseIfDstStringIsNull);
    RUN_TEST_CASE(
        TestRaiiString,
        test_RaiiStringAppend_RaiiString_ReturnsFalseIfStringToAppendIsNull);
    RUN_TEST_CASE(
        TestRaiiString,
        test_RaiiStringAppend_RaiiString_ReturnsFalseIfInternalStringOfDstIsNull);
    RUN_TEST_CASE(
        TestRaiiString,
        test_RaiiStringAppend_RaiiString_ReturnsFalseIfInternalStringOfStringToAppendIsNull);
    RUN_TEST_CASE(TestRaiiString,
                  test_RaiiStringAppend_RaiiString_AppendsStringToDst);
    RUN_TEST_CASE(
        TestRaiiString,
        test_RaiiStringAppend_RaiiString_HandlesAppendingToSelfCorrectly);
    RUN_TEST_CASE(TestRaiiString,
                  test_RaiiStringAppend_RaiiString_StringToAppendIsUnchanged);
    RUN_TEST_CASE(
        TestRaiiString,
        test_RaiiStringAppend_RaiiString_ReturnsFalseAndDoesntChangeStringsIfTotalLengthIsGreaterThanMaxLengthWithTermination);
}
