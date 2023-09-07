#include "unity.h"
#include "help_functions.h"

void test_multiply(void) {
    TEST_ASSERT_EQUAL(10, multiply(5, 2));
}

void test_add(void) {
    TEST_ASSERT_EQUAL(8, add(5, 3));
}
