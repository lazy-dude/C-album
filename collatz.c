// https://mathworld.wolfram.com/CollatzProblem.html

// Collatz conjecture

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <stdnoreturn.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <CUnit/Console.h>
#include <CUnit/Automated.h>

#define NDEBUG // test or run

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

struct info
{
    uintmax_t max;
    uintmax_t steps;
};
struct info make_info(uintmax_t max,uintmax_t steps)
{
    const struct info info ={.max=max , .steps=steps};
    return info;
}

#define TRIES 100000// 1000000
uintmax_t evens=0, odds=0;
bool collatz(uintmax_t num, struct info *info) 
{
    
    if(num == 1)
    {
        
        if(info->steps >1 ) 
            info->steps++;
            
        return true;
    }	
    
    info->steps++;
        
    if(num % 2 == 0)
    {
        
        uintmax_t en=num/2;
        if(num > info->max)
            info->max=num; 
        evens++;
        return collatz(en,info);
    }	
    else 
    {
        assert(num %2 ==1);
        uintmax_t on=num*3+1;
        if(on > info->max)
            info->max=on;
        odds++;
        return collatz(on,info);
    }
    return false;
}

void test_collatz(void);

int main(void)
{
#ifndef NDEBUG
    int cui;

    cui = CU_initialize_registry();
    
    if (cui != CUE_SUCCESS)
        simple_err("Initialization failed.");
    
    CU_pSuite suite1 = CU_add_suite("suite1", NULL, NULL);
    if (suite1 == NULL)
        simple_err("suite1 creation failed.");
        
    CU_pTest test1 = CU_add_test(suite1, "test_collatz", test_collatz);
    if(test1 == NULL)
        simple_err("test1 creation failed.");

    CU_basic_set_mode(CU_BRM_VERBOSE); 
    CU_basic_run_tests();
    CU_cleanup_registry();

#else
    uintmax_t i;
    struct info info={1,0};
    for(i=1; i<TRIES; i++)
    {
        info=make_info(1,0);
        collatz(i,&info);
    }
    double sum = evens+odds;
    printf("$$ evens:%f odds:%f\n",(double)evens/sum,(double)odds/sum );
#endif // NDEBUG
    printf("Finished.\n");
    return 0;

}


void test_collatz(void)
{
    struct info info={.max=1,.steps=0};

    CU_ASSERT(collatz(1,&info));
    CU_ASSERT_EQUAL(info.max,1);
    CU_ASSERT_EQUAL(info.steps,0);
    
    info=make_info(1,0);
    CU_ASSERT(collatz(2,&info));
    CU_ASSERT_EQUAL(info.max,2);
    CU_ASSERT_EQUAL(info.steps,1);
    
    info=make_info(1,0);
    CU_ASSERT(collatz(3,&info));
    CU_ASSERT_EQUAL(info.max,16);
    CU_ASSERT_EQUAL(info.steps,8);
    
    info=make_info(1,0);
    CU_ASSERT(collatz(4,&info));
    CU_ASSERT_EQUAL(info.max,4);
    CU_ASSERT_EQUAL(info.steps,3);
    
    info=make_info(1,0);
    CU_ASSERT(collatz(5,&info));
    CU_ASSERT_EQUAL(info.max,16);
    CU_ASSERT_EQUAL(info.steps,6);
    
    info=make_info(1,0);
    CU_ASSERT(collatz(10,&info));
    CU_ASSERT_EQUAL(info.max,16);
    CU_ASSERT_EQUAL(info.steps,7);
    
    info=make_info(1,0);
    CU_ASSERT(collatz(27,&info));
    CU_ASSERT_EQUAL(info.max,9232);
    CU_ASSERT_EQUAL(info.steps,112);
    
    info=make_info(1,0);
    CU_ASSERT(collatz(31,&info));
    CU_ASSERT_EQUAL(info.max,9232);
    CU_ASSERT_EQUAL(info.steps,107);
    
    info=make_info(1,0);
    CU_ASSERT(collatz(83,&info));
    CU_ASSERT_EQUAL(info.max,9232);
    CU_ASSERT_EQUAL(info.steps,111);
    
    info=make_info(1,0);
    CU_ASSERT(collatz(602,&info));
    CU_ASSERT_EQUAL(info.max,904);
    CU_ASSERT_EQUAL(info.steps,18);
    
    info=make_info(1,0);
    CU_ASSERT(collatz(999999,&info));
    CU_ASSERT_EQUAL(info.max,22781248);
    CU_ASSERT_EQUAL(info.steps,259);
    
    info=make_info(1,0);
    CU_ASSERT(collatz(1000000,&info));
    CU_ASSERT_EQUAL(info.max,1000000);
    CU_ASSERT_EQUAL(info.steps,153);
    
}

