// https://mathworld.wolfram.com/196-Algorithm.html

// Computing 196 algorithm

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <stdnoreturn.h>
#include <stdlib.h>
#include <ctype.h>

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <CUnit/Console.h>
#include <CUnit/Automated.h>

#define NDEBUG // test or run

#define MAX_DIG 100 // 5000
#define TRIES 200 // 10000

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

static int len;

void fill(char*str, char *num)
{
    for(int k=0; k<MAX_DIG; k++)
        num[k]=' ';
    num[MAX_DIG-1]='\0';
    int str_len=strlen(str);
    int i,j;
    for(i=str_len-1,j=0; i>=0; i--,j++)
    {
        char c=str[i];
        num[MAX_DIG-j-2] = c ;
    }
    len=strlen(str);
}

bool all_digits(char* num)
{
    num[MAX_DIG-1]='\0';
    int i;
    for(i=strlen(num)-1; i>0 && num[i]!=' '; i--)
    {
        
        if(!isdigit(num[i]))  
        {
            return false;
        }
    }
    
    return true;
}

void reverse(char* num,char* rev)
{
    
    for(int cnt=0;cnt<MAX_DIG;cnt++)
        rev[cnt]=' ';
    
    rev[MAX_DIG-1]='\0';
    num[MAX_DIG-1]='\0';
    int i; 
    
    i=MAX_DIG-len-1;
    
    assert(isdigit(num[i]));
    
    int k=MAX_DIG-2;
    int j;
    for( j=i; j<MAX_DIG-1; j++,k--)
    {
        char c=num[k];
        assert(isdigit(c));
        
        rev[j]=c;
        
    }
    
    assert(all_digits(rev));
    
}

bool is_palindrome(char* num) // TODO digit by digit
{
    char * rev = malloc(MAX_DIG*sizeof(char));
    memset(rev,' ', MAX_DIG*sizeof(char));
    
    rev[MAX_DIG-1]='\0';
    num[MAX_DIG-1]='\0';
    reverse(num,rev);
    
    int i;
    for(i=MAX_DIG-2; i>=MAX_DIG-len-1; i--)
    {
        if(num[i]!= rev[i])
            return false;
    }
    
    free(rev);
    return true;
}

void reverse_then_add(char* num)
{
    assert(strlen(num)<=MAX_DIG);
    assert(all_digits(num));
    
    char * rev = malloc(MAX_DIG*sizeof(char));
    
    reverse(num,rev);
    
    int carry=0;
    
    int i=MAX_DIG-2;
    int sum=0;
    do
    {
        if(!isdigit(rev[i]))
            break;
        
        assert(isdigit(num[i]));
        assert(isdigit(rev[i]));
        sum = (num[i]-'0')+(rev[i]-'0');
        assert(sum>=0);
        assert(sum<=18);
        num[i] =(sum+carry)%10+'0';
        
        assert(isdigit(num[i]));
        
        if(sum+carry>= 10)
            carry=1;
        else
            carry=0;
        i--;
    }while(rev[i]!=' ');
    
    assert(all_digits(num));
    
    if(carry)
    {
        assert(carry==1);
        num[i]='1';
        len++;
        
    } 
    
    free(rev);
    assert(all_digits(num));
    
    if(len > MAX_DIG-3)
        simple_err("len limit exceeded\n");
    return;
}

char* odds(char*num)
{
    
    int count[10]={0};
    
    char *odds_dig=calloc(11,sizeof(char) );
    size_t i;
    for(i=MAX_DIG-2;isdigit(num[i]);i--)
    {
        count[num[i]-'0']++;
    }	
    
    int k=0;
    for(int j=0; j<=9; j++)
    {
        
        if(count[j]%2==1)
        {
            odds_dig[k]=j+'0';
            k++;
            
        }	
    }
    
    assert(all_digits(odds_dig));
    return odds_dig;  	
}

bool fd_ld_equal(char* num)
{
    char fd = num[MAX_DIG-2];
    char ld= num[MAX_DIG - len-1];
    if(fd==ld)
        return true;
    else
        return false;
}

bool cld_crd_equal(char * num)
{	
    char cld; 
    char crd;
    
    if(len%2==0)
    {
        crd = num[MAX_DIG - len/2-1];
        cld = num[MAX_DIG - len/2 -2];
    }else{
        crd = num[MAX_DIG - len/2-1];
        cld = num[MAX_DIG - len/2 -3];
    }
    
    assert(isdigit(crd));
    assert(isdigit(cld));
    
    if(crd==cld)
        return true;
    else
        return false;
}

double average(char* num)
{
    double av;
    int sum=0;
    int i;
    for(i=MAX_DIG-len-1; i<=MAX_DIG-2; i++)
        sum += (num[i]-'0');
    
    av = (double)sum / (double)len;
    
    return av; 
}

// test prototypes

void test_fill(void);
void test_all_digits(void);
void test_reverse(void);
void test_is_palindrome(void);
void test_reverse_then_add(void);
void test_reverse_then_add2(void);
void test_reverse_then_add3(void);
void test_odds(void);
void test_fd_ld_equal(void);
void test_cld_crd_equal(void);
void test_average(void);

//////////////////////

int main(void) // first/last digit equality, center_left/center_right digit eq, average of digits , oddly occurred digits
{

#ifndef NDEBUG
    int ir;

    ir = CU_initialize_registry();
    
    if (ir != CUE_SUCCESS)
        simple_err("Initialization failed.");
    
    // CU_pSuite CU_add_suite(const char* strName, CU_InitializeFunc pInit, CU_CleanupFunc pClean)
    CU_pSuite suite1 = CU_add_suite("196_test_suite", NULL, NULL);
    if (suite1 == NULL)
        simple_err("suite1 creation failed.");

    CU_pTest test0 = CU_add_test(suite1, "test_fill", test_fill);
    if(test0 == NULL)
        simple_err("test0 creation failed.");

    // CU_pTest CU_add_test(CU_pSuite pSuite, const char* strName, CU_TestFunc pTestFunc)
    CU_pTest test1 = CU_add_test(suite1, "test_reverse", test_reverse);
    if(test1 == NULL)
        simple_err("test1 creation failed.");
        
    CU_pTest test2 = CU_add_test(suite1, "test_all_digits", test_all_digits);
    if(test2 == NULL)
        simple_err("test2 creation failed.");
    
    CU_pTest test3 = CU_add_test(suite1, "test_is_palindrome", test_is_palindrome);
    if(test3 == NULL)
        simple_err("test3 creation failed.");
    
    CU_pTest test4 = CU_add_test(suite1, "test_reverse_then_add", test_reverse_then_add);
    if(test4 == NULL)
        simple_err("test4 creation failed.");
    
    CU_pTest test5 = CU_add_test(suite1, "test_reverse_then_add2", test_reverse_then_add2);
    if(test5 == NULL)
        simple_err("test5 creation failed.");
    
    CU_pTest test6 = CU_add_test(suite1, "test_reverse_then_add3", test_reverse_then_add3);
    if(test6 == NULL)
        simple_err("test6 creation failed.");
    
    CU_pTest test7 = CU_add_test(suite1, "test_odds", test_odds);
    if(test7 == NULL)
        simple_err("test7 creation failed.");
    CU_pTest test8 = CU_add_test(suite1, "test_fd_ld_equal", test_fd_ld_equal);
    if(test8 == NULL)
        simple_err("test8 creation failed.");
    CU_pTest test9 = CU_add_test(suite1,"test_cld_crd_equal", test_cld_crd_equal); //
    if(test9 == NULL)
        simple_err("test9 creation failed.");
    CU_pTest test10 = CU_add_test(suite1, "test_average", test_average);
    if(test10 == NULL)
        simple_err("test10 creation failed.");
    
    
    CU_basic_set_mode(CU_BRM_VERBOSE); 

    //CU_automated_run_tests();
    CU_basic_run_tests();

    CU_cleanup_registry();

#else
    printf("196 algorithm:\n");
    
    char *num=malloc(MAX_DIG*sizeof(char));
    fill("196",num);
    char* odds_arr;
    int i=0;
    bool fl,cl_cr;
    char pm,pm2;
    double av;
    
    do{
        
        odds_arr=odds(num);
        fl = fd_ld_equal(num);
        cl_cr = cld_crd_equal(num);
        pm=(fl == true ? '+' : '-');
        pm2=(cl_cr==true ? '+' : '-');
        av = average(num);
        printf("num:%s fd_ld:%c cl_cr:%c av:%.2f odds:%s\n",num, pm,pm2, av, odds_arr);
        reverse_then_add(num);
        free(odds_arr);
        i++;
    }while(i<TRIES);
    free(num);
    printf("Finished.\n");
    
#endif // NDEBUG
    return 0;
}

///////////////////////////
//       tests
//////////////////////////
void test_fill(void)
{
    char * num = malloc(MAX_DIG*sizeof(char));
    memset(num,' ', MAX_DIG*sizeof(char));
    fill("123",num);
    CU_ASSERT(num[MAX_DIG-1]=='\0');
    CU_ASSERT(num[MAX_DIG-2]=='3');
    CU_ASSERT(num[MAX_DIG-3]=='2');
    CU_ASSERT(num[MAX_DIG-4]=='1');
    CU_ASSERT(num[MAX_DIG-5]==' ');
    free(num);
    
    char * num2 = malloc(MAX_DIG*sizeof(char));
    memset(num2,' ', MAX_DIG*sizeof(char));
    fill("89",num2);
    CU_ASSERT(num2[MAX_DIG-1]=='\0');
    CU_ASSERT(num2[MAX_DIG-2]=='9');
    CU_ASSERT(num2[MAX_DIG-3]=='8');
    CU_ASSERT(num2[MAX_DIG-4]==' ');
    
    free(num2);
}

void test_all_digits(void)
{
    
    char * num = malloc(MAX_DIG*sizeof(char));
    memset(num,' ', MAX_DIG*sizeof(char));
    fill("165944604392018",num);
    CU_ASSERT(all_digits(num));
    
    fill("12a3",num);
    
    CU_ASSERT_FALSE(all_digits(num));
    
    char * num2 = malloc(MAX_DIG*sizeof(char));
    memset(num2,' ', MAX_DIG*sizeof(char));
    fill("196",num);
    CU_ASSERT(all_digits(num2));
    
    free(num);
    free(num2);
}

void test_reverse(void)
{
    
    char * num = malloc(MAX_DIG*sizeof(char));
    memset(num,' ', MAX_DIG*sizeof(char));
    
    char * rev = malloc(MAX_DIG*sizeof(char));
    memset(rev,' ', MAX_DIG*sizeof(char));
    
    fill("12321",num);
    
    reverse(num,rev);
    
    CU_ASSERT_STRING_EQUAL(num,rev);
    
    fill("123",num);
    reverse(num,rev);
    
    fill("321",num);
    CU_ASSERT_STRING_EQUAL(rev,num);
    
    fill("1234567890",num);
    reverse(num,rev);
    fill("0987654321",num);
    CU_ASSERT_STRING_EQUAL(rev,num);
    
    free(num);
    free(rev);
}

void test_is_palindrome(void)
{
    
    char * num = malloc(MAX_DIG*sizeof(char));
    
    fill("12321",num);
    CU_ASSERT_EQUAL(is_palindrome(num),true);
    
    fill("196",num);
    CU_ASSERT_EQUAL(is_palindrome(num),false);
    fill("12",num);
    CU_ASSERT_EQUAL(is_palindrome(num),false);
    fill("11",num);
    CU_ASSERT_EQUAL(is_palindrome(num),true);
    fill("99",num);
    CU_ASSERT_EQUAL(is_palindrome(num),true);
    fill("888",num);
    CU_ASSERT_EQUAL(is_palindrome(num),true);
    fill("887",num);
    CU_ASSERT_EQUAL(is_palindrome(num),false);
    fill("550",num);
    CU_ASSERT_EQUAL(is_palindrome(num),false);
    
    free(num);
}

void test_reverse_then_add(void)
{
    
    char * num = malloc(MAX_DIG*sizeof(char));
    memset(num,' ', MAX_DIG*sizeof(char));
    
    char * rta = malloc(MAX_DIG*sizeof(char));
    memset(rta,' ', MAX_DIG*sizeof(char));
    
    fill("10",num);
    reverse_then_add(num);
    fill("11",rta);
    CU_ASSERT_STRING_EQUAL(num,rta);
    
    fill("196",num);
    reverse_then_add(num);
    fill("887",rta);
    CU_ASSERT_STRING_EQUAL(num,rta);
    
    fill("887",num);
    reverse_then_add(num);
    fill("1675",rta);
    CU_ASSERT_STRING_EQUAL(num,rta);
    
    fill("1675",num);
    reverse_then_add(num);
    fill("7436",rta);
    CU_ASSERT_STRING_EQUAL(num,rta);
    
    free(num);
    free(rta);
}

void test_reverse_then_add2(void)
{
    char *sequence[]={"196", "887", "1675", "7436", "13783", "52514", "94039"
        , "187088", "1067869", "10755470", "18211171", "35322452", "60744805", "111589511", "227574622"};
    int seq_len = 15;
    int i;
    for(i=0; i<seq_len-1; i++)
    {
        
        char * nums = malloc(MAX_DIG*sizeof(char));	
        char * next = malloc(MAX_DIG*sizeof(char));
        
        fill(sequence[i],nums);
        reverse_then_add(nums);
        fill(sequence[i+1],next);
        CU_ASSERT_STRING_EQUAL(nums,next);
        
        free(nums);
        free(next);
    }
}

void test_reverse_then_add3(void)
{
    char * num = malloc(MAX_DIG*sizeof(char));
    
    fill("89",num);
    for(int i=0; i<50; i++)
    {
        reverse_then_add(num);
        if(is_palindrome(num))
        {
            char answer[MAX_DIG];
            fill("8813200023188",answer);
            CU_ASSERT_STRING_EQUAL(num,answer);
            break;
        }
            
    }
    
    free(num);
}

void test_odds(void)
{
    char * num = malloc(MAX_DIG*sizeof(char));
    fill("196",num);
    CU_ASSERT_STRING_EQUAL(odds(num),"169");
    fill("121",num);
    CU_ASSERT_STRING_EQUAL(odds(num),"2");
    fill("11",num);
    CU_ASSERT_STRING_EQUAL(odds(num),"");
    fill("887",num);
    CU_ASSERT_STRING_EQUAL(odds(num),"7");
    fill("111899999",num);
    CU_ASSERT_STRING_EQUAL(odds(num),"189");
    fill("8813200023188",num);
    CU_ASSERT_STRING_EQUAL(odds(num),"0");
    fill("0123456789",num);
    CU_ASSERT_STRING_EQUAL(odds(num),"0123456789");
    
    free(num);
}

void test_fd_ld_equal(void)
{
    char * num = malloc(MAX_DIG*sizeof(char));
    
    fill("196",num);
    CU_ASSERT_EQUAL(fd_ld_equal(num),false);
    fill("11",num);
    CU_ASSERT_EQUAL(fd_ld_equal(num),true);
    fill("887",num);
    CU_ASSERT_EQUAL(fd_ld_equal(num),false);
    fill("8813200023188",num);
    CU_ASSERT_EQUAL(fd_ld_equal(num),true);
    fill("89",num);
    CU_ASSERT_EQUAL(fd_ld_equal(num),false);
    
    free(num);
}

void test_cld_crd_equal(void)
{
    char * num = malloc(MAX_DIG*sizeof(char));
    
    fill("196",num);
    CU_ASSERT_EQUAL(cld_crd_equal(num),false);
    fill("11",num);
    CU_ASSERT_EQUAL(cld_crd_equal(num),true);
    fill("887",num);
    CU_ASSERT_EQUAL(cld_crd_equal(num),false);
    fill("8813200023188",num);
    CU_ASSERT_EQUAL(cld_crd_equal(num),true);
    fill("89",num);
    CU_ASSERT_EQUAL(cld_crd_equal(num),false);
    fill("187088",num);
    CU_ASSERT_EQUAL(cld_crd_equal(num),false);
    fill("8801197801088",num);
    CU_ASSERT_EQUAL(cld_crd_equal(num),false);
    fill("897100798",num);
    CU_ASSERT_EQUAL(cld_crd_equal(num),false);
    fill("18408442064004592449047",num);
    CU_ASSERT_EQUAL(cld_crd_equal(num),false);
    fill("12311",num);
    CU_ASSERT_EQUAL(cld_crd_equal(num),false);
    fill("12320",num);
    CU_ASSERT_EQUAL(cld_crd_equal(num),true);
    
    free(num);
}

void test_average(void)
{
    char * num = malloc(MAX_DIG*sizeof(char));
    
    fill("196",num);
    CU_ASSERT_DOUBLE_EQUAL(5.33,average(num),0.1);
    fill("11",num);
    CU_ASSERT_DOUBLE_EQUAL(1.0,average(num),0.1);
    fill("8813200023188",num);
    CU_ASSERT_DOUBLE_EQUAL(3.38,average(num),0.1);
    fill("89",num);
    CU_ASSERT_DOUBLE_EQUAL(8.50,average(num),0.1);
    
    free(num);
}
