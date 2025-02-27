/* AUTOGENERATED FILE. DO NOT EDIT. */

/*=======Test Runner Used To Run Each Test Below=====*/
#define RUN_TEST(TestFunc, TestLineNum) \
{ \
  Unity.CurrentTestName = #TestFunc; \
  Unity.CurrentTestLineNumber = TestLineNum; \
  Unity.NumberOfTests++; \
  if (TEST_PROTECT()) \
  { \
      setUp(); \
      TestFunc(); \
  } \
  if (TEST_PROTECT()) \
  { \
    tearDown(); \
  } \
  UnityConcludeTest(); \
}

/*=======Automagically Detected Files To Include=====*/
#include "unity.h"
#include <setjmp.h>
#include <stdio.h>
#include "systrace.h"

/*=======External Functions This Runner Calls=====*/
extern void setUp(void);
extern void tearDown(void);
extern void test__print_summary__pass(void);
extern void test__read_parameters__fail_insufficient_arguments(void);
extern void test__read_parameters__fail_process_name_not_found(void);
extern void test__read_parameters__pass(void);
extern void test__start_tracing__pass(void);
extern void test__start_tracing__fail(void);

/*=======Test Reset Option=====*/
void resetTest(void);
void resetTest(void)
{
  tearDown();
  setUp();
}


/*=======MAIN=====*/
int main(void)
{
  UnityBegin("test/unittest.c");
  RUN_TEST(test__print_summary__pass, 0);
  RUN_TEST(test__read_parameters__fail_insufficient_arguments, 1);
  RUN_TEST(test__read_parameters__fail_process_name_not_found, 2);
  RUN_TEST(test__read_parameters__pass, 3);
  RUN_TEST(test__start_tracing__pass, 4);
  RUN_TEST(test__start_tracing__fail, 5);
  return (UnityEnd());
}
