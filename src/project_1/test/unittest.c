#include <stdlib.h>
#include "../../Unity/src/unity.h"
#include "./../src/scheduler.h"
#include "./../src/ui.h"

void setUp (void) {}
void tearDown (void) {}

void test__get_random_number__unique_value(void) {
    const int value = 2;
    const int random = get_random_number(value, value);
    TEST_ASSERT_EQUAL_INT(value, random);
}

void test__get_random_number__pass(void) {
    const int min = 2;
    const int max = 80;
    const int random = get_random_number(min, max);
    printf("Random value = %d\n", random);
    TEST_ASSERT_LESS_OR_EQUAL_INT(max, random);
    TEST_ASSERT_GREATER_OR_EQUAL_INT(min, random);
}

void test__get_next_thread__pass(void) {
    int result;
    get_next_thread(&result);
    TEST_ASSERT_NULL(result);
}