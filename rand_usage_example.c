// Info:
// rand() usage example version 2
// https://www.eg.bucknell.edu/~xmeng/Course/CS6337/Note/master/node37.html
// uniformity, independence

// Includes:
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <inttypes.h>
#include <limits.h>
#include <math.h>
#include <stdalign.h>
#include <stdnoreturn.h>

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <CUnit/Console.h>

#define NDEBUG // test or run

// Typedef:
typedef uint64_t ctr_type; // Couner type
typedef long double gf_t; // greatest float type

// Consts:
#define N 6 // Number of cards or dice or coin for example 52 6 4 2
static const long TRIES = 20001; // Number of tries like 30001 10001

static const long OFFSET = 1;
static const gf_t GF_ZERO=(gf_t)0.0;
static const gf_t GF_ONE=(gf_t)1.0;
static const gf_t GF_TWO=(gf_t)2.0;
static const gf_t ONE_HUNDRED = (gf_t)100.0;
static const gf_t HALF = (gf_t)0.5;
static const gf_t ONE_PERCENT = (gf_t)0.01;
static const gf_t MILLI = (gf_t)0.001;
static const size_t ST_ZERO = (size_t)0;
static const long MAX = (RAND_MAX - (RAND_MAX%N + OFFSET));
static const gf_t AV_PER = (gf_t)ONE_HUNDRED/(gf_t)N;
static const gf_t EXPECTED_VARIANCE = (gf_t)1.0/(gf_t)12.0;

// Struct:
static struct data
{   ctr_type ctr[N];
    gf_t mean;
    gf_t variance;
    gf_t * probability_ptr;
    // inserted by compiler...
	const uint64_t padding;// 8-byte alignment, 8-byte data

} my_data;

// Declarations:
void init_data( struct data* const);
noreturn void simple_err(const char * const);
void *my_alloc(size_t );
intmax_t gen_rands(int *const, int *const, gf_t *const, ctr_type *const);
gf_t compute_mean(intmax_t);
gf_t compute_v_up(gf_t const * const, gf_t);
gf_t compute_variance(gf_t const * const, gf_t);
void print_array(ctr_type const* const , size_t );
void print_all(ctr_type const *const, int , int , intmax_t , gf_t , gf_t );

// Test declarations:
void test_gen_rands(void);
void test_mean(void);
void test_variance(void);

// Main:
int main(void)
{

    int hi = 0;
    int lo = INT_MAX;
    intmax_t sum = 0;

    if(!(N >= 2 && N <= 52))
        simple_err("Unacceptable value of N.");
    if(!(TRIES >= 1 && TRIES <= 1000000))
        simple_err("Unacceptable value of TRIES.");
    init_data(&my_data);

    my_data.probability_ptr = my_alloc(TRIES * sizeof(gf_t));

    sum = gen_rands(&hi, &lo, my_data.probability_ptr, my_data.ctr);

    my_data.mean = compute_mean(sum);

    my_data.variance = compute_variance(my_data.probability_ptr, my_data.mean);
#ifdef NDEBUG
    print_all(my_data.ctr, lo, hi, sum, my_data.mean, my_data.variance);

	
#else // NDEBUG
	int init_reg = CUE_NOMEMORY; // CUnit

    init_reg = CU_initialize_registry();
    if (init_reg != CUE_SUCCESS)
        simple_err("CUnit initialization failed.");

    // CU_pSuite CU_add_suite(const char* strName, CU_InitializeFunc pInit, CU_CleanupFunc pClean)
    CU_pSuite suite1 = CU_add_suite("rand_test", NULL, NULL);
    if (suite1 == NULL)
        simple_err("suite1 creation failed.");

    // CU_pTest CU_add_test(CU_pSuite pSuite, const char* strName, CU_TestFunc pTestFunc)
    CU_pTest test1 = CU_add_test(suite1, "gen_rands_function", test_gen_rands);
    if (test1 == NULL)
        simple_err("test1 creation failed.");
    CU_pTest test2 = CU_add_test(suite1, "mean_variable", test_mean);
    if (test2 == NULL)
        simple_err("test2 creation failed.");
    CU_pTest test3 = CU_add_test(suite1, "variance_variable", test_variance);
    if (test3 == NULL)
        simple_err("test3 creation failed.");

    CU_basic_set_mode(CU_BRM_VERBOSE);

    //CU_console_run_tests();
    //CU_automated_run_tests();
    CU_basic_run_tests();

    CU_cleanup_registry();
    // finish tests.
#endif // NDEBUG
	free(my_data.probability_ptr);
    return EXIT_SUCCESS;
}

// Function definitions:
void init_data(struct data* const data_ptr)
{
    data_ptr->probability_ptr = NULL;
    for(long i = 0; i<N; i++)
        data_ptr->ctr[i] = 0;

    data_ptr->mean = GF_ZERO;
    data_ptr->variance = GF_ZERO;

    return;
}

noreturn void simple_err(const char * const s)
{
    if( (s==NULL) || ( *s=='\0') )
    {
        fprintf(stderr,"Please provide a proper message for simple_err function.\n");
        exit(EXIT_FAILURE);
    }
    fprintf(stderr,"%s\n",s);
    exit(EXIT_FAILURE);
}

void *my_alloc( size_t size )
{
    static void *alloc_ptr = NULL; // added static, see: http://c-faq.com/ptrs/passptrinit.html
    alloc_ptr = malloc(size);

    if( alloc_ptr == NULL )
        simple_err("alloc_ptr memory allocation failed.");

    return alloc_ptr;
}

intmax_t gen_rands(int *const hi, int *const lo, gf_t *const probability_ptr, ctr_type *const p)
{
    intmax_t sum = 0;
    int64_t i;
    int r = -1;

    srand((unsigned int)time(NULL));

    for (i=0; i<TRIES; i++)
    {
        do
        {
            r = rand();
            if(r >= 0 && r <= MAX)
                break;
        }
        while(true);

        if( r > *hi )
            *hi=r;
        if( r < *lo )
            *lo=r;

        *(probability_ptr+i)= (gf_t)r / (gf_t)MAX;
        sum += r;

        p[r%N]++;
    }

    return sum;
}

gf_t compute_mean(intmax_t sum)
{
    gf_t mean = (gf_t)sum / ( (gf_t)TRIES * (gf_t)MAX );
    return mean;
}

gf_t compute_v_up(gf_t const * const probability_ptr, gf_t mean)
{
    int64_t i;
    gf_t variance_up = GF_ZERO;

    for (i=0; i<TRIES; i++)
        variance_up += powl( probability_ptr[i] - mean, GF_TWO );

    return variance_up;
}

gf_t compute_variance(gf_t const * const probability_ptr, gf_t mean)
{
    gf_t variance_up = compute_v_up(probability_ptr, mean);
    gf_t variance = variance_up / (gf_t)TRIES;

    return variance;
}

void print_array(ctr_type const *const p, size_t n)
{
    size_t sub;
    gf_t percent = GF_ZERO;

    for(sub=ST_ZERO; sub<n; sub++)
    {
        percent = ONE_HUNDRED*(gf_t)p[sub]/(gf_t)TRIES ;
        printf("%4zu\t%"PRIuMAX"\t%.2Lf %%\n", sub,(uintmax_t)(*(p+sub)), percent);
    }

    return;
}

void print_all(ctr_type const*const c, int lo, int hi, intmax_t sum, gf_t mean, gf_t variance)
{
    printf("ctr[sub] number percentage\n\n");
    print_array(c, N);
    printf("\nAverage percentage is %.2Lf %%\n",AV_PER);
    printf("Lowest is %d\t Highest is %d\n",lo,hi);
    printf("Sum is %"PRIdMAX"\tMean is %Lf\n",sum,mean);
    printf("Variance is %Lf expected is %Lf\n",variance,EXPECTED_VARIANCE);

    return;
}

// Test definitions:
void test_gen_rands(void) // create a loop (fixture) for all percents
{ 
    gf_t percent = GF_ZERO;
    for(long i=0; i<N; i++)
    {
        percent = ONE_HUNDRED*(gf_t)my_data.ctr[i]/(gf_t)TRIES ;

        CU_ASSERT_DOUBLE_EQUAL(percent, AV_PER ,GF_ONE);
        // experimental granularity: GF_ONE
    }
    return;
}

void test_mean(void)
{
    CU_ASSERT_DOUBLE_EQUAL(my_data.mean, HALF ,ONE_PERCENT);
    // experimental granularity: ONE_PERCENT
    return;
}

void test_variance(void)
{
    CU_ASSERT_DOUBLE_EQUAL(my_data.variance, EXPECTED_VARIANCE, MILLI);
    // experimental granularity: MILLI
    return;
}
// End.
