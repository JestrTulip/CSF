#include <stdio.h>
#include <stdlib.h>
#include "fixedpoint.h"
#include "tctest.h"

// Test fixture object, has some useful values for testing
typedef struct {
  Fixedpoint zero;
  Fixedpoint one;
  Fixedpoint one_half;
  Fixedpoint one_fourth;
  Fixedpoint large1;
  Fixedpoint large2;
  Fixedpoint max;
  Fixedpoint min; 
  Fixedpoint test_1; 
  Fixedpoint test_2; 
  Fixedpoint test_3; 
  Fixedpoint test_4; 
  Fixedpoint test_5; 

} TestObjs;

// functions to create and destroy the test fixture
TestObjs *setup(void);
void cleanup(TestObjs *objs);

// test functions
void test_whole_part(TestObjs *objs);
void test_frac_part(TestObjs *objs);
void test_create_from_hex(TestObjs *objs);
void test_format_as_hex(TestObjs *objs);
void test_negate(TestObjs *objs);
void test_add(TestObjs *objs);
void test_sub(TestObjs *objs);
void test_halve(TestObjs *objs); 
void test_double(TestObjs *objs); 
void test_compare(TestObjs *objs); 
void test_is_overflow_pos(TestObjs *objs);
void test_is_overflow_neg(TestObjs *objs); 
void test_is_underflow_pos(TestObjs *objs); 
void test_is_underflow_neg(TestObjs *objs); 
void test_is_err(TestObjs *objs);

int main(int argc, char **argv) {
  // if a testname was specified on the command line, only that
  // test function will be executed
  if (argc > 1) {
    tctest_testname_to_execute = argv[1];
  }

  TEST_INIT();

  TEST(test_whole_part);
  TEST(test_frac_part);
  TEST(test_create_from_hex);
  TEST(test_format_as_hex);
  TEST(test_negate);
  TEST(test_add);
  TEST(test_sub);
  TEST(test_halve); 
  TEST(test_double); 
  TEST(test_compare); 
  TEST(test_is_overflow_pos);
  TEST(test_is_overflow_neg);
  TEST(test_is_underflow_pos); 
  TEST(test_is_underflow_neg);
  TEST(test_is_err);

  // IMPORTANT: if you add additional test functions (which you should!),
  // make sure they are included here.  E.g., if you add a test function
  // "my_awesome_tests", you should add
  //
  //   TEST(my_awesome_tests);
  //
  // here. This ensures that your test function will actually be executed.

  TEST_FINI();
}

TestObjs *setup(void) {
  TestObjs *objs = malloc(sizeof(TestObjs));

  objs->zero = fixedpoint_create(0UL);
  objs->one = fixedpoint_create(1UL);
  objs->one_half = fixedpoint_create2(0UL, 0x8000000000000000UL);
  objs->one_fourth = fixedpoint_create2(0UL, 0x4000000000000000UL);
  objs->large1 = fixedpoint_create2(0x4b19efceaUL, 0xec9a1e2418UL);
  objs->large2 = fixedpoint_create2(0xfcbf3d5UL, 0x4d1a23c24fafUL);
  objs->max = fixedpoint_create2(0xFFFFFFFFFFFFFFFFUL, 0xFFFFFFFFFFFFFFFFUL);
  objs->min = fixedpoint_create2(0x0UL, 0x0UL);
  objs->test_1 = fixedpoint_create2(0xa1af195570d5e446UL, 0x14b9138a43a3a712UL); 
  objs->test_2 = fixedpoint_create2(0x7e9d8914UL, 0x4819b2cbUL); 
  objs->test_3 = fixedpoint_create2(0x2ae0UL, 0x2ae0UL); 
  objs->test_4 = fixedpoint_create2(0x1UL, 0x1UL); 
  objs->test_5 = fixedpoint_create2(0x4b19efceaUL, 0x0UL); 


  return objs;
}

void cleanup(TestObjs *objs) {
  free(objs);
}

void test_whole_part(TestObjs *objs) {
  ASSERT(0UL == fixedpoint_whole_part(objs->zero));
  ASSERT(1UL == fixedpoint_whole_part(objs->one));
  ASSERT(0UL == fixedpoint_whole_part(objs->one_half));
  ASSERT(0UL == fixedpoint_whole_part(objs->one_fourth));
  ASSERT(0x4b19efceaUL == fixedpoint_whole_part(objs->large1));
  ASSERT(0xfcbf3d5UL == fixedpoint_whole_part(objs->large2));
  ASSERT(0xFFFFFFFFFFFFFFFFUL == fixedpoint_whole_part(objs->max)); 
  ASSERT(0x0UL == fixedpoint_whole_part(objs->min)); 
  ASSERT(0xa1af195570d5e446UL == fixedpoint_whole_part(objs->test_1));
  ASSERT(0x7e9d8914UL == fixedpoint_whole_part(objs->test_2));
  ASSERT(0x2ae0UL == fixedpoint_whole_part(objs->test_3));
  ASSERT(0x1UL == fixedpoint_whole_part(objs->test_4));
  ASSERT(0x4b19efceaUL == fixedpoint_whole_part(objs->test_5));
}

void test_frac_part(TestObjs *objs) {
  ASSERT(0UL == fixedpoint_frac_part(objs->zero));
  ASSERT(0UL == fixedpoint_frac_part(objs->one));
  ASSERT(0x8000000000000000UL == fixedpoint_frac_part(objs->one_half));
  ASSERT(0x4000000000000000UL == fixedpoint_frac_part(objs->one_fourth));
  ASSERT(0xec9a1e2418UL == fixedpoint_frac_part(objs->large1));
  ASSERT(0x4d1a23c24fafUL == fixedpoint_frac_part(objs->large2));
  ASSERT(0xFFFFFFFFFFFFFFFFUL == fixedpoint_frac_part(objs->max)); 
  ASSERT(0x0000000000000000UL == fixedpoint_frac_part(objs->min)); 
  ASSERT(0x14b9138a43a3a712UL == fixedpoint_frac_part(objs->test_1)); 
  ASSERT(0x4819b2cbUL == fixedpoint_frac_part(objs->test_2));
  ASSERT(0x2ae0UL == fixedpoint_frac_part(objs->test_3)); 
  ASSERT(0x1UL == fixedpoint_frac_part(objs->test_4)); 
  ASSERT(0x0UL == fixedpoint_frac_part(objs->test_5));

}

void test_create_from_hex(TestObjs *objs) {
  (void) objs;

  Fixedpoint val1 = fixedpoint_create_from_hex("f6a5865.00f2");


  ASSERT(fixedpoint_is_valid(val1));

  ASSERT(0xf6a5865UL == fixedpoint_whole_part(val1));

  ASSERT(0x00f2000000000000UL == fixedpoint_frac_part(val1));

  Fixedpoint val2 = fixedpoint_create_from_hex("123"); 

  ASSERT(fixedpoint_is_valid(val2));
  
  ASSERT(0x123UL == fixedpoint_whole_part(val2));

  ASSERT(0x0UL == fixedpoint_frac_part(val2));

  Fixedpoint val3 = fixedpoint_create_from_hex("-123"); 

  ASSERT(fixedpoint_is_neg(val3)); 

  ASSERT(0x123UL == fixedpoint_whole_part(val3));

  ASSERT(0x0L == fixedpoint_frac_part(val3));

  Fixedpoint val4 = fixedpoint_create_from_hex("ABACAB"); 

  ASSERT(0xABACABUL == fixedpoint_whole_part(val4));

  ASSERT(0x0L == fixedpoint_frac_part(val4));

  Fixedpoint val5 = fixedpoint_create_from_hex("8573AB."); 

  ASSERT(0x8573ABUL == fixedpoint_whole_part(val5));

  ASSERT(0x0L == fixedpoint_frac_part(val5));

  Fixedpoint val6 = fixedpoint_create_from_hex("-.8573AB"); 
  
  ASSERT(fixedpoint_is_neg(val6)); 
  
  ASSERT(0x0UL == fixedpoint_whole_part(val6));

  ASSERT(0x8573AB0000000000UL == fixedpoint_frac_part(val6));

  Fixedpoint val7 = fixedpoint_create_from_hex(".8573AB"); 
  
  ASSERT(0x0UL == fixedpoint_whole_part(val7));

  ASSERT(0x8573AB0000000000UL == fixedpoint_frac_part(val7));

  Fixedpoint val8 = fixedpoint_create_from_hex("AAaaaaaaAAAAAAAAAAAAAAAA.BBBBBBBBBBBBBBBBBB");

  ASSERT(fixedpoint_is_err(val8)); 

  Fixedpoint val9 = fixedpoint_create_from_hex("AAaaaaaaAAAAAAAAAAAAAApA.BBBBBBBBBBBBBBBBBB");

  ASSERT(fixedpoint_is_err(val9)); 

  Fixedpoint val10 = fixedpoint_create_from_hex("AAaaaaaaAAAAAAAAAAAAAA^A.BBBBBBBBBBBBBB*BBB");

  ASSERT(fixedpoint_is_err(val10)); 

  Fixedpoint val11 = fixedpoint_create_from_hex("AAaaaaaaAAAAAAAAAAAAAA%A.BBBBBBBBBBBB%BBBBB");

  ASSERT(fixedpoint_is_err(val11)); 

  Fixedpoint val12 = fixedpoint_create_from_hex("AAaaaaaaAAAAAAAAAAAAAApAAaaaahcashvvcbvcahvkBHJBVKSAHBKDBVUYSBUYSBVYHbiuyuiYUIBAIUYBIDSUYUDVBUYBVUbiubyuiybiubUIYBUBSUYSBUIASIUBVAUYSBUKYABSUKBVKUbuybvsuybusdbuyvsuj.BBBBBBBBBBBBBBBBBB");

  ASSERT(fixedpoint_is_err(val12)); 

}

void test_format_as_hex(TestObjs *objs) {
  char *s;

  s = fixedpoint_format_as_hex(objs->zero);
  ASSERT(0 == strcmp(s, "0"));
  free(s);

  s = fixedpoint_format_as_hex(objs->one);
  ASSERT(0 == strcmp(s, "1"));
  free(s);

  s = fixedpoint_format_as_hex(objs->one_half);
  ASSERT(0 == strcmp(s, "0.8"));
  free(s);

  s = fixedpoint_format_as_hex(objs->one_fourth);
  ASSERT(0 == strcmp(s, "0.4"));
  free(s);

  s = fixedpoint_format_as_hex(objs->large1);
  ASSERT(0 == strcmp(s, "4b19efcea.000000ec9a1e2418"));
  free(s);

  s = fixedpoint_format_as_hex(objs->large2);
  ASSERT(0 == strcmp(s, "fcbf3d5.00004d1a23c24faf"));
  free(s);

  s = fixedpoint_format_as_hex(objs->test_1);
  ASSERT(0 == strcmp(s, "a1af195570d5e446.14b9138a43a3a712"));
  free(s);


  
}

void test_negate(TestObjs *objs) {
  // none of the test fixture objects are negative
  ASSERT(!fixedpoint_is_neg(objs->zero));
  ASSERT(!fixedpoint_is_neg(objs->one));
  ASSERT(!fixedpoint_is_neg(objs->one_half));
  ASSERT(!fixedpoint_is_neg(objs->one_fourth));
  ASSERT(!fixedpoint_is_neg(objs->large1));
  ASSERT(!fixedpoint_is_neg(objs->large2));

  // negate the test fixture values
  Fixedpoint zero_neg = fixedpoint_negate(objs->zero);
  Fixedpoint one_neg = fixedpoint_negate(objs->one);
  Fixedpoint one_half_neg = fixedpoint_negate(objs->one_half);
  Fixedpoint one_fourth_neg = fixedpoint_negate(objs->one_fourth);
  Fixedpoint large1_neg = fixedpoint_negate(objs->large1);
  Fixedpoint large2_neg = fixedpoint_negate(objs->large2);

  // zero does not become negative when negated
  ASSERT(!fixedpoint_is_neg(zero_neg));

  // all of the other values should have become negative when negated
  ASSERT(fixedpoint_is_neg(one_neg));
  ASSERT(fixedpoint_is_neg(one_half_neg));
  ASSERT(fixedpoint_is_neg(one_fourth_neg));
  ASSERT(fixedpoint_is_neg(large1_neg));
  ASSERT(fixedpoint_is_neg(large2_neg));

  // magnitudes should stay the same
  ASSERT(0UL == fixedpoint_whole_part(objs->zero));
  ASSERT(1UL == fixedpoint_whole_part(objs->one));
  ASSERT(0UL == fixedpoint_whole_part(objs->one_half));
  ASSERT(0UL == fixedpoint_whole_part(objs->one_fourth));
  ASSERT(0x4b19efceaUL == fixedpoint_whole_part(objs->large1));
  ASSERT(0xfcbf3d5UL == fixedpoint_whole_part(objs->large2));
  ASSERT(0UL == fixedpoint_frac_part(objs->zero));
  ASSERT(0UL == fixedpoint_frac_part(objs->one));
  ASSERT(0x8000000000000000UL == fixedpoint_frac_part(objs->one_half));
  ASSERT(0x4000000000000000UL == fixedpoint_frac_part(objs->one_fourth));
  ASSERT(0xec9a1e2418UL == fixedpoint_frac_part(objs->large1));
  ASSERT(0x4d1a23c24fafUL == fixedpoint_frac_part(objs->large2));
}

void test_add(TestObjs *objs) {
  (void) objs;

  Fixedpoint lhs, rhs, sum, fin, fin2;
  lhs = fixedpoint_create_from_hex("-c7252a193ae07.7a51de9ea0538c5");
  ASSERT(0xc7252a193ae07UL == fixedpoint_whole_part(lhs)); 
  ASSERT(0x7a51de9ea0538c50UL == fixedpoint_frac_part(lhs)); 
  ASSERT(fixedpoint_is_neg(lhs));
  rhs = fixedpoint_create_from_hex("d09079.1e6d601");
  ASSERT(0xd09079UL == fixedpoint_whole_part(rhs)); 
  ASSERT(0x1e6d601000000000UL == fixedpoint_frac_part(rhs)); 
  sum = fixedpoint_add(lhs, rhs);
  ASSERT(fixedpoint_is_neg(sum));
  ASSERT(0xc7252a0c31d8eUL == fixedpoint_whole_part(sum));
  ASSERT(0x5be47e8ea0538c50UL == fixedpoint_frac_part(sum));
  fin = fixedpoint_add(objs->one, objs->one);
  ASSERT(2UL == fixedpoint_whole_part(fin));
  fin2 = fixedpoint_add(objs->test_3, objs->test_5);
  ASSERT(0x4B19F27CAUL == fixedpoint_whole_part(fin2));


}

void test_sub(TestObjs *objs) {
  (void) objs;

  Fixedpoint lhs, rhs, diff, lhs2, rh2, diff2;

  lhs = fixedpoint_create_from_hex("-ccf35aa3a04a3b.b105");
  rhs = fixedpoint_create_from_hex("f676e8.58");
  diff = fixedpoint_sub(lhs, rhs);
  ASSERT(fixedpoint_is_neg(diff));
  ASSERT(0xccf35aa496c124UL == fixedpoint_whole_part(diff));
  ASSERT(0x0905000000000000UL == fixedpoint_frac_part(diff));

  lhs2 = fixedpoint_create_from_hex("-FFFFFFFFFFFFFFFF.FFFFFFFFFFFFFFFF"); 
  rh2 = fixedpoint_create_from_hex("0.8000000000000000");
  diff2 = fixedpoint_sub(lhs2, rh2); 
  ASSERT(fixedpoint_is_overflow_neg(diff2));
}

void test_is_overflow_pos(TestObjs *objs) {
  Fixedpoint sum;

  sum = fixedpoint_add(objs->max, objs->one);
  ASSERT(fixedpoint_is_overflow_pos(sum));

  sum = fixedpoint_add(objs->one, objs->max);
  ASSERT(fixedpoint_is_overflow_pos(sum));

  Fixedpoint negative_one = fixedpoint_negate(objs->one);

  sum = fixedpoint_sub(objs->max, negative_one);
  ASSERT(fixedpoint_is_overflow_pos(sum));
}

void test_is_err(TestObjs *objs) {
  (void) objs;

  // too many characters
  Fixedpoint err1 = fixedpoint_create_from_hex("88888888888888889.6666666666666666");
  ASSERT(fixedpoint_is_err(err1));

  // too many characters
  Fixedpoint err2 = fixedpoint_create_from_hex("6666666666666666.88888888888888889");
  ASSERT(fixedpoint_is_err(err2));

  // this one is actually fine
  Fixedpoint err3 = fixedpoint_create_from_hex("-6666666666666666.888888888888888");
  ASSERT(fixedpoint_is_valid(err3));
  ASSERT(!fixedpoint_is_err(err3));

  // whole part is too large
  Fixedpoint err4 = fixedpoint_create_from_hex("88888888888888889");
  ASSERT(fixedpoint_is_err(err4));

  // fractional part is too large
  Fixedpoint err5 = fixedpoint_create_from_hex("7.88888888888888889");
  ASSERT(fixedpoint_is_err(err5));

  // invalid hex digits in whole part
  Fixedpoint err6 = fixedpoint_create_from_hex("123xabc.4");
  ASSERT(fixedpoint_is_err(err6));

  // invalid hex digits in fractional part
  Fixedpoint err7 = fixedpoint_create_from_hex("7.0?4");
  ASSERT(fixedpoint_is_err(err7));
}

void test_halve(TestObjs *objs) { 
  Fixedpoint half1, half2, half3; 

  half1 = fixedpoint_halve(objs->one);
  ASSERT(0UL == fixedpoint_whole_part(half1));

  half2 = fixedpoint_halve(objs->test_5);
  ASSERT(0x258CF7E75UL == fixedpoint_whole_part(half2)); 
  
  half3 = fixedpoint_halve(objs->zero);
  ASSERT(0UL == fixedpoint_whole_part(half3));

}

void test_double(TestObjs *objs) { 
  Fixedpoint half1, half2, half3; 

  half1 = fixedpoint_double(objs->one);
  ASSERT(2UL == fixedpoint_whole_part(half1));

  half2 = fixedpoint_double(objs->test_5);
  ASSERT(0x9633DF9D4UL == fixedpoint_whole_part(half2)); 
  
  half3 = fixedpoint_double(objs->zero);
  ASSERT(0UL == fixedpoint_whole_part(half3));

}

void test_compare(TestObjs *objs) { 
  ASSERT(fixedpoint_compare(objs->one, objs->one) == 0); 
  ASSERT(fixedpoint_compare(objs->test_5, objs->test_3) == 1);
  ASSERT(fixedpoint_compare(objs->test_3, objs->test_5) == -1);
  Fixedpoint neg_test = fixedpoint_negate(objs->one);
  ASSERT(fixedpoint_compare(objs->one, neg_test) == 1);
  ASSERT(fixedpoint_compare(neg_test, objs->one) == -1);
}

void test_is_overflow_neg(TestObjs *objs) { 
  Fixedpoint test1, test2; 

  test1 = fixedpoint_double(fixedpoint_negate(objs->max));
  ASSERT(fixedpoint_is_overflow_neg(test1));

  test2 = fixedpoint_add(fixedpoint_negate(objs->max), fixedpoint_negate(objs->one));
  ASSERT(fixedpoint_is_overflow_neg(test2));

}

void test_is_underflow_neg(TestObjs *objs) { 
  Fixedpoint test1; 
 

  test1 = fixedpoint_halve(fixedpoint_negate(objs->test_4)); 
  ASSERT(fixedpoint_is_underflow_neg(test1));

}

void test_is_underflow_pos(TestObjs *objs) { 
  Fixedpoint test1; 
 

  test1 = fixedpoint_halve((objs->test_4)); 
  ASSERT(fixedpoint_is_underflow_pos(test1));

}