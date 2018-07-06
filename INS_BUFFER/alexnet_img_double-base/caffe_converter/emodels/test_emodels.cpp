/*
 *  Unit Testing for Error models
 *
 *  Michael Sullivan, University of Texas
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "minunit.h"
#include "emodels.h"

int tests_run = 0;
static const int num_trials = 10;

// ignore annoying warnings
#pragma GCC diagnostic ignored "-Wwrite-strings"

/*
 * Helper functions
 */

typedef UINT64 (*TESTFUN)(UINT64);

// number of bits differing between two inputs
template <class intType>
intType bitsDiff (intType x1, intType x2)
{
    return bitsSet(x1 ^ x2);
}

/* Print n as a binary number */
template <class intType>
static inline void printbits(intType n)
{
    intType i;
    i = (intType)0x1 << ((sizeof(n) << 3) - 1);

    while (i > 0)
    {
        if (n & i)
            printf("1");
        else
            printf("0");
        i >>= 1;
    }
}

/* Print n as a binary number */
template <class intType>
static inline void printbits_fp32(intType n)
{
    intType i;
    i = (intType)0x1 << ((sizeof(n) << 3) - 1);

    intType sign_bit = (intType)0x1 << ((sizeof(n) << 3) - 2);
    intType exponent_bit = (intType)0x1 << ((sizeof(n) << 3) - 10);

    while (i > 0)
    {
        if (n & i)
            printf("1");
        else
            printf("0");
        i >>= 1;

        if (i == sign_bit || i == exponent_bit)
            printf(" ");
    }
}

/* Print n as a binary number */
template <class intType>
static inline void printbits_fp64(intType n)
{
    intType i;
    i = (intType)0x1 << ((sizeof(n) << 3) - 1);

    intType sign_bit = (intType)0x1 << ((sizeof(n) << 3) - 2);
    intType exponent_bit = (intType)0x1 << ((sizeof(n) << 3) - 13);

    while (i > 0)
    {
        if (n & i)
            printf("1");
        else
            printf("0");
        i >>= 1;

        if (i == sign_bit || i == exponent_bit)
            printf(" ");
    }
}

static inline char* test_template(char* test_name, TESTFUN tf){
    // get the proper error model (see emodels.h for structures)
    EMODEL_FUNCS efs = emodel_map[test_name]; // struct of function pointers

    // UINT8
    printf("%s_8 syndromes:\n", test_name);
    for (UINT8 loop = 0; loop < num_trials; loop++) {
        UINT8 res = efs.f8(loop);
        printbits <UINT8> (loop ^ res);
        printf("\n");
        mu_assert("uint8 error", tf((UINT64)bitsDiff <UINT8> (loop, res)));
    }
    printf("");

    // UINT8
    printf("%s_16 syndromes:\n", test_name);
    for (UINT16 loop = 0; loop < num_trials; loop++) {
        UINT16 res = efs.f16(loop);
        printbits <UINT16> (loop ^ res);
        printf("\n");
        mu_assert("uint16 error", tf((UINT64)bitsDiff <UINT16> (loop, res)));
    }
    printf("");

    // UINT32
    printf("%s_32 syndromes:\n", test_name);
    for (UINT32 loop = 0; loop < num_trials; loop++) {
        UINT32 res = efs.f32(loop);
        printbits <UINT32> (loop ^ res);
        printf("\n");
        mu_assert("uint32 error", tf((UINT64)bitsDiff <UINT32> (loop, res)));
    }
    printf("");

    // UINT64
    printf("%s_64 syndromes:\n", test_name);
    for (UINT64 loop = 0; loop < num_trials; loop++) {
        UINT64 res = efs.f64(loop);
        printbits <UINT64> (loop ^ res);
        printf("\n");
        mu_assert("uint64 error", tf((UINT64)bitsDiff <UINT64> (loop, res)));
    }
    printf("\n");
    return 0;
}

static inline char* test_template_fp(char* test_name, TESTFUN tf){
    // get the proper error model (see emodels.h for structures)
    EMODEL_FUNCS efs = emodel_map[test_name]; // struct of function pointers

    // UINT32
    printf("%s_32 syndromes:\n", test_name);
    for (UINT32 loop = 0; loop < num_trials; loop++) {
        UINT32 res = efs.f32(loop);
        printbits_fp32 (loop ^ res);
        printf("\n");
        mu_assert("fp32 error", tf((UINT64)bitsDiff <UINT32> (loop, res)));
    }
    printf("");

    // UINT64
    printf("%s_64 syndromes:\n", test_name);
    for (UINT64 loop = 0; loop < num_trials; loop++) {
        UINT64 res = efs.f64(loop);
        printbits_fp64 (loop ^ res);
        printf("\n");
        mu_assert("fp64 error", tf((UINT64)bitsDiff <UINT64> (loop, res)));
    }
    printf("\n");
    return 0;
}

/*
 * Test functions
 */

UINT64 none_assert (UINT64 bits_diff)
{
    return bits_diff == 0;
}
static char * test_none() {
    TESTFUN tf = &none_assert;
    return test_template("NONE", tf);
}

UINT64 randbit1_assert (UINT64 bits_diff)
{
    return bits_diff == 1;
}
UINT64 randbit2_assert (UINT64 bits_diff)
{
    return bits_diff == 2;
}
UINT64 randbit4_assert (UINT64 bits_diff)
{
    return bits_diff == 4;
}
UINT64 randbit8_assert (UINT64 bits_diff)
{
    return bits_diff == 8;
}
static char * test_randbit() {
    char * running_result;
    TESTFUN tf1 = &randbit1_assert;
    TESTFUN tf2 = &randbit2_assert;
    TESTFUN tf4 = &randbit4_assert;
    TESTFUN tf8 = &randbit8_assert;

    running_result = test_template("RANDBIT1", randbit1_assert);
    if (running_result != 0)
        return running_result;

    running_result = test_template("RANDBIT2", randbit2_assert);
    if (running_result != 0)
        return running_result;

    running_result = test_template("RANDBIT4", randbit4_assert);
    if (running_result != 0)
        return running_result;

    running_result = test_template("RANDBIT8", randbit8_assert);
    if (running_result != 0)
        return running_result;

    return 0;
}

static char * test_burst() {
    char * running_result;
    // visually verify burstiness
    TESTFUN tf1 = &randbit1_assert;
    TESTFUN tf2 = &randbit2_assert;
    TESTFUN tf4 = &randbit4_assert;
    TESTFUN tf8 = &randbit8_assert;

    running_result = test_template("BURST1", randbit1_assert);
    if (running_result != 0)
        return running_result;

    running_result = test_template("BURST2", randbit2_assert);
    if (running_result != 0)
        return running_result;

    running_result = test_template("BURST4", randbit4_assert);
    if (running_result != 0)
        return running_result;

    running_result = test_template("BURST8", randbit8_assert);
    if (running_result != 0)
        return running_result;

    return 0;
}

// using visual verification
UINT64 random_assert (UINT64 bits_diff)
{
    return 1;
}
static char * test_random() {
    char * running_result;
    TESTFUN tf1 = &random_assert;

    running_result = test_template("RANDOM", random_assert);
    if (running_result != 0)
        return running_result;

    running_result = test_template("RANDOM_LH", random_assert);
    if (running_result != 0)
        return running_result;

    return 0;
}

static char * test_mantb() {
    char * running_result;
    // visually verify burstiness
    TESTFUN tf1 = &randbit1_assert;
    TESTFUN tf2 = &randbit2_assert;
    TESTFUN tf4 = &randbit4_assert;
    TESTFUN tf8 = &randbit8_assert;

    running_result = test_template_fp("MANTB1", randbit1_assert);
    if (running_result != 0)
        return running_result;

    running_result = test_template_fp("MANTB2", randbit2_assert);
    if (running_result != 0)
        return running_result;

    running_result = test_template_fp("MANTB4", randbit4_assert);
    if (running_result != 0)
        return running_result;

    running_result = test_template_fp("MANTB8", randbit8_assert);
    if (running_result != 0)
        return running_result;

    return 0;
}

static char * test_mantr() {
    char * running_result;
    // visually verify limits
    TESTFUN tf1 = &randbit1_assert;
    TESTFUN tf2 = &randbit2_assert;
    TESTFUN tf4 = &randbit4_assert;
    TESTFUN tf8 = &randbit8_assert;

    running_result = test_template_fp("MANTR1", randbit1_assert);
    if (running_result != 0)
        return running_result;

    running_result = test_template_fp("MANTR2", randbit2_assert);
    if (running_result != 0)
        return running_result;

    running_result = test_template_fp("MANTR4", randbit4_assert);
    if (running_result != 0)
        return running_result;

    running_result = test_template_fp("MANTR8", randbit8_assert);
    if (running_result != 0)
        return running_result;

    return 0;
}

static char * test_mantrandi() {
    char * running_result;
    // visually verify limits
    TESTFUN tf1 = &random_assert;
    TESTFUN tf2 = &random_assert;
    TESTFUN tf4 = &random_assert;
    TESTFUN tf8 = &random_assert;

    running_result = test_template_fp("MANTRAND", random_assert);
    if (running_result != 0)
        return running_result;

    running_result = test_template_fp("MANTRAND16I", random_assert);
    if (running_result != 0)
        return running_result;

    running_result = test_template_fp("MANTRAND8I", random_assert);
    if (running_result != 0)
        return running_result;

    running_result = test_template_fp("MANTRAND4I", random_assert);
    if (running_result != 0)
        return running_result;

    running_result = test_template_fp("MANTRAND2I", random_assert);
    if (running_result != 0)
        return running_result;

    return 0;
}

static char * test_mantrand() {
    char * running_result;
    // visually verify limits
    TESTFUN tf1 = &random_assert;
    TESTFUN tf2 = &random_assert;
    TESTFUN tf4 = &random_assert;
    TESTFUN tf8 = &random_assert;

    running_result = test_template_fp("MANTRAND", random_assert);
    if (running_result != 0)
        return running_result;

    running_result = test_template_fp("MANTRAND16", random_assert);
    if (running_result != 0)
        return running_result;

    running_result = test_template_fp("MANTRAND8", random_assert);
    if (running_result != 0)
        return running_result;

    running_result = test_template_fp("MANTRAND4", random_assert);
    if (running_result != 0)
        return running_result;

    running_result = test_template_fp("MANTRAND2", random_assert);
    if (running_result != 0)
        return running_result;

    return 0;
}

static char * all_tests() {
    mu_run_test(test_none);
    mu_run_test(test_randbit);
    mu_run_test(test_burst);
    mu_run_test(test_random);
    mu_run_test(test_mantb);
    mu_run_test(test_mantr);
    mu_run_test(test_mantrandi);
    mu_run_test(test_mantrand);
    return 0;
}

int main(int argc, char **argv) {

    srand(time(NULL));

    char *result = all_tests();
    if (result != 0) {
        printf("%s\n", result);
    }
    else {
        printf("ALL TESTS PASSED\n");
    }
    printf("Tests run: %d\n", tests_run);

    return result != 0;
}
