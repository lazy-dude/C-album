// https://mathworld.wolfram.com/SolitaryNumber.html

// Friendly/Solitary/Multi-perfect numbers

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdnoreturn.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <CUnit/Console.h>

//#define NDEBUG // run or test
#define LIMIT 100000//100000// 5000

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

bool is_prime(int num)
{
	int  i, flag = 1;
 
    // Iterate from 2 to sqrt(n)
    for (i = 2; i <= sqrt(num); i++) 
    {
 
        // If n is divisible by any number between
        // 2 and n/2, it is not prime
        if (num % i == 0) 
        {
            flag = 0;
            break;
        }
    }
 
    if (num <= 1)
        flag = 0;
 
    if (flag == 1) 
    {
        //printf("%d is a prime number", n);
        return true;
    }
    else 
    {
       // printf("%d is not a prime number", n);
       return false;
    }
 
    //return 0;
}

/*int divisor_function(int num)
{
	int i;
	int sigma=0;
	
	for(i=1; i<=sqrt(num); i++)
	{
		if(num %i == 0)
		{
			int ndi=num/i;
			if(ndi == i)
				sigma += i;
			else
			{
				sigma += i;
				sigma += (ndi);
			}
		}
	}
	return sigma;
}*/

// https://rosettacode.org/wiki/Sum_of_divisors#C 
// See https://en.wikipedia.org/wiki/Divisor_function
unsigned int divisor_function(unsigned int n) {
    unsigned int total = 1, power = 2;
    unsigned int p;
    // Deal with powers of 2 first
    for (; (n & 1) == 0; power <<= 1, n >>= 1) {
        total += power;
    }
    // Odd prime factors up to the square root
    for (p = 3; p * p <= n; p += 2) {
        unsigned int sum = 1;
        for (power = p; n % p == 0; power *= p, n /= p) {
            sum += power;
        }
        total *= sum;
    }
    // If n > 1 then it's prime
    if (n > 1) {
        total *= n + 1;
    }
    return total;
}


int gcd(int a, int b)
{
    int r;
    while (b != 0)
    {
        r = a % b;

        a = b;
        b = r;
    }
    return a;
}

struct ratio
{ // simplified sigma(n)/n
	int sigma_n;
	int n;
	int gcd;
};
struct ratio ten_ratio(void) // 18/10 or simplified 9/5
{
	struct ratio const rs={.sigma_n=9 , .n=5};
	return rs;
}
struct ratio make_ratio(int sigma_n, int n)
{
	struct ratio rs;
	rs.sigma_n=sigma_n;
	rs.n=n;
	return rs;
}

bool ratio_equal(struct ratio rs1,struct ratio rs2)
{
	assert(gcd(rs1.sigma_n,rs1.n)==1);
	assert(gcd(rs2.sigma_n,rs2.n)==1);
	if(rs1.sigma_n==rs2.sigma_n && rs1.n==rs2.n)
		return true;
	else 
		return false;
}

struct ratio compute_ratio(int num)
{
	struct ratio rs;
	rs.sigma_n=divisor_function(num);
	rs.n=num;
	
	int gcd_num=gcd(rs.sigma_n,rs.n);
	rs.sigma_n /= gcd_num;
	rs.n /= gcd_num;
	rs.gcd=gcd_num;
	
	assert(gcd(rs.sigma_n,rs.n)==1);
	return rs;
}


bool p_multi_p(int num)
{
	bool pmp=false ;
	double s1=sqrt(num);
	int s2=round(s1);
	if(is_prime(s2) && s2*s2==num)
		pmp=true;
	return pmp;
}


void test_gcd(void);
void test_divisor_function(void);
void test_is_prime(void);
void test_ratio_equal(void);
void test_compute_ratio(void);
void test_p_multi_p(void);

int main(void)
{
#ifndef NDEBUG
    int in_reg;

    in_reg = CU_initialize_registry();
    
    if (in_reg != CUE_SUCCESS)
        simple_err("Initialization failed.");

    CU_pSuite suite1 = CU_add_suite("suite1", NULL, NULL);
    if (suite1 == NULL)
        simple_err("suite1 creation failed.");

    CU_pTest test1 = CU_add_test(suite1, "test_gcd", test_gcd);
    if (test1 == NULL)
        simple_err("test1 creation failed.");
    CU_pTest test2 = CU_add_test(suite1, "test_is_prime", test_is_prime);
    if (test2 == NULL)
        simple_err("test2 creation failed.");
    CU_pTest test3 = CU_add_test(suite1, "test_divisor_function", test_divisor_function);
    if (test3 == NULL)
        simple_err("test3 creation failed.");
    CU_pTest test4 = CU_add_test(suite1, "test_ratio_equal", test_ratio_equal);
    if (test4 == NULL)
        simple_err("test4 creation failed.");
    CU_pTest test5 = CU_add_test(suite1, "test_compute_ratio", test_compute_ratio);
    if (test5 == NULL)
        simple_err("test5 creation failed.");
    CU_pTest test6 = CU_add_test(suite1, "test_p_multi_p", test_p_multi_p);
    if (test6 == NULL)
        simple_err("test6 creation failed.");
    
    CU_basic_set_mode(CU_BRM_VERBOSE); 
    CU_basic_run_tests();
    CU_cleanup_registry();
#else // release

	int i;
	struct ratio rs;
	for(i=1; i<=LIMIT; i++)
	{
		rs=compute_ratio(i);
		if(ratio_equal(rs,ten_ratio() ) && i !=10)
		{
			printf("Friend of 10 found, it is %d\n",i);
			return 0;
		}
		else 
		{
			bool p2=p_multi_p(rs.sigma_n);
			if(  p2 && is_prime(rs.n) )// rs.sigma_n/10==0 &&
			//if(is_prime(rs.sigma_n) && is_prime(rs.n) )
				printf("Num:%6d gcd:%6d ratio: %d/%d \n",i,rs.gcd,rs.sigma_n,rs.n);
		}		
		
	}

#endif // NDEBUG

    return EXIT_SUCCESS;
}

void test_gcd(void)
{
	CU_ASSERT_EQUAL(gcd(1,2),1);
	CU_ASSERT_EQUAL(gcd(2,2),2);
	CU_ASSERT_EQUAL(gcd(6,4),2);
	CU_ASSERT_EQUAL(gcd(8,20),4);
	CU_ASSERT_EQUAL(gcd(18,27),9);
	CU_ASSERT_EQUAL(gcd(1000,101),1);
	
	
}
void test_divisor_function(void)
{
	
	CU_ASSERT_EQUAL(divisor_function(1),1);
	CU_ASSERT_EQUAL(divisor_function(2),3);
	CU_ASSERT_EQUAL(divisor_function(3),4);
	CU_ASSERT_EQUAL(divisor_function(4),7);
	CU_ASSERT_EQUAL(divisor_function(10),18);
	CU_ASSERT_EQUAL(divisor_function(15),24);
	
}

void test_is_prime(void)
{
	CU_ASSERT_EQUAL(is_prime(1),false);
	CU_ASSERT_EQUAL(is_prime(2),true);
	CU_ASSERT_EQUAL(is_prime(5),true);
	CU_ASSERT_EQUAL(is_prime(10),false);
	CU_ASSERT_EQUAL(is_prime(19),true);
	CU_ASSERT_EQUAL(is_prime(25),false);
	CU_ASSERT_EQUAL(is_prime(1000),false);
}

void test_ratio_equal(void)
{
	struct ratio rs1={1,1,1};
	struct ratio rs2={1,1,1};
	CU_ASSERT_EQUAL(ratio_equal(rs1,rs2),true);
	
	rs1.sigma_n=2;
	CU_ASSERT_EQUAL(ratio_equal(rs1,rs2),false);
	
	
}

void test_compute_ratio(void)
{
	struct ratio rs;
	
	rs=make_ratio(1,1);
	CU_ASSERT(ratio_equal(compute_ratio(1),rs));
	
	rs=make_ratio(3,2);
	CU_ASSERT(ratio_equal(compute_ratio(2),rs));
	
	rs=make_ratio(4,3);
	CU_ASSERT(ratio_equal(compute_ratio(3),rs));
	
	rs=make_ratio(9,5);
	CU_ASSERT(ratio_equal(compute_ratio(10),rs));
	
	rs=make_ratio(8,5);//24,15
	CU_ASSERT(ratio_equal(compute_ratio(15),rs));
	
	CU_ASSERT(ratio_equal(compute_ratio(24),compute_ratio(91963648)) );
}

void test_p_multi_p(void)
{
	CU_ASSERT(p_multi_p(4));
	CU_ASSERT(p_multi_p(9));
	CU_ASSERT(p_multi_p(25));
	
	CU_ASSERT_FALSE(p_multi_p(2));
	CU_ASSERT_FALSE(p_multi_p(3));
	CU_ASSERT_FALSE(p_multi_p(6));
	CU_ASSERT_FALSE(p_multi_p(10));	
	
}


