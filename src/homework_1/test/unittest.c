#include "../../Unity/src/unity.h"
#include "./../src/systrace.h"

void setUp (void) {}
void tearDown (void) {}

void test__print_summary__pass(void) {
    int counter = 2;
    TEST_ASSERT_EQUAL_HEX8(0, print_summary(&counter));
}

void test__read_parameters__fail_insufficient_arguments(void) {
    int argc = 0;
    char ** argv;
    int *child_argc;
    char ** child_argv;
    TEST_ASSERT_EQUAL_INT(FAIL, read_parameters(argc, argv, child_argc, child_argv));
}

void test__read_parameters__fail_process_name_not_found(void) {
    int argc = 2;
    char **argv = (char *[]){"systrace", "-V"};
    int value = 0;
    int *child_argc = &value;
    char ** child_argv = (char *[]){};
    TEST_ASSERT_EQUAL_INT(FAIL, read_parameters(argc, argv, child_argc, child_argv));
    TEST_ASSERT_LESS_THAN_INT(1, *child_argc);
}

void test__read_parameters__pass(void) {
    int argc = 4;
    char **argv = (char *[]){"systrace", "-V", "ls", "-la"};
    int value = 0;
    int *child_argc = &value;
    char ** child_argv = (char *[]){};
    TEST_ASSERT_EQUAL_INT(OK, read_parameters(argc, argv, child_argc, child_argv));
    TEST_ASSERT_EQUAL_INT(2, *child_argc);
    TEST_ASSERT_TRUE(strcmp(child_argv[0], "ls") == 0);
    TEST_ASSERT_TRUE(strcmp(child_argv[1], "-la") == 0);
}

void test__start_tracing__pass(void) {
    int child_argc = 2;
    char **child_argv = (char *[]){"ls", "-la"};
    int counter[COUNTER_SIZE];
    pid_t pid;

    TEST_ASSERT_EQUAL_INT(OK, start_tracing(child_argc, child_argv, counter, &pid));
}

void test__start_tracing__fail(void) {
    int child_argc = 2;
    char **child_argv = (char *[]){"invalid_cmd", "-la"};
    int counter[COUNTER_SIZE];
    pid_t pid;

    // This function will create two process, one will fail (the child) and the other one will success
    int ret = start_tracing(child_argc, child_argv, counter, &pid);

    if (pid == 0) {
        TEST_ASSERT_EQUAL_INT(FAIL, ret);
    } else {
        TEST_ASSERT_EQUAL_INT(OK, ret);
    }
}
