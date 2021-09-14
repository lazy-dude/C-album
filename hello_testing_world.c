// This is a demonstration of how to use CUnit testing library.

// http://cunit.sourceforge.net/doc/index.html
// http://cunit.sourceforge.net/doc/writing_tests.html

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdnoreturn.h>

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <CUnit/Console.h>

noreturn void simple_err(const char*);
int maxi(int , int );
void test_maxi(void);

noreturn void simple_err(const char *s)
{
    if( (s==NULL) || ( *s=='\0') )
    {
        fprintf(stderr,"Please provide a proper message for simple_err function\n");
        exit(EXIT_FAILURE);
    }
    fprintf(stderr,"%s\n",s);
    exit(EXIT_FAILURE);
}

int maxi(int i1, int i2)
{
    return (i1 > i2) ? i1 : i2;
}

void test_maxi(void)
{ 
    CU_ASSERT_EQUAL(maxi(0,2) , 2);
    CU_ASSERT_EQUAL(maxi(0,-2) , 0);
    CU_ASSERT_EQUAL(maxi(2,2) , 2);
}

int main(void)
{
    int ir;

    ir = CU_initialize_registry();
    
    if (ir != CUE_SUCCESS)
        simple_err("Initialization failed.");

    // CU_pSuite CU_add_suite(const char* strName, CU_InitializeFunc pInit, CU_CleanupFunc pClean)
    CU_pSuite suite1 = CU_add_suite("maxi_test", NULL, NULL);
    if (suite1 == NULL)
        simple_err("suite1 creation failed.");

    // CU_pTest CU_add_test(CU_pSuite pSuite, const char* strName, CU_TestFunc pTestFunc)
    CU_pTest test1 = CU_add_test(suite1, "maxi_fun", test_maxi);
    if (test1 == NULL)
        simple_err("test1 creation failed.");

    CU_basic_set_mode(CU_BRM_VERBOSE); 

    //CU_console_run_tests();
    //CU_automated_run_tests();
    CU_basic_run_tests();

    CU_cleanup_registry();

    return EXIT_SUCCESS;
}
