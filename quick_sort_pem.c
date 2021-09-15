// https://github.com/soupi/3-C-Sorting-Algorithms/blob/master/QuickSort.c

// quick sort algorithm , pivot is estimated median.

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>

#define TRIES 100000 // 10000000

void Quicksort(int *array, int length);
void quicksort(int *array, int low, int high);
int place_pivot(int *array, int low, int high);

void display(int arr[], int n)
{
	
    for(int i=0; i<n; i++)
        printf("%d ", arr[i]);
    printf("\n");

}

void swap(int* first, int* second)
{
    int temp = *first;
    *first = *second;
    *second = temp;
}

// min max
int min(int a,int b)
{
    return a < b ? a : b;
}
int max(int a,int b)
{
    return a > b ? a : b;
}
// median of 3
int median3(int a, int b, int c)
{  
    intmax_t tot_v = a + b + c ;
    int max_v = max(a, max(b, c));
    int min_v = min(a, min(b, c));
    return tot_v - max_v - min_v;
}

double ratio(void)
{
    return (double)rand() / (double)RAND_MAX;
}

//  pivot is estimated median
int gen_piv_pem(int arr2[], int low, int high)
{
    int pivot = low;
	
    if (high == low + 1) { // one choice

        return low;
    }
    if (high == low + 2) // two choices
    {
        double r = ratio();
        if (r <= 0.5)
            return low;
        else
            return (low + 1);
    }
    if (high == low + 3) // three choices
    {
        int value = median3(arr2[low], arr2[low + 1], arr2[low + 2]);
        if (value == arr2[low])
            pivot = low;
        else if (value == arr2[low + 1])
            pivot = low + 1;
        else if (value == arr2[low + 2])
            pivot = low + 2;
    }
    if (high > low + 3) // estimate
    {
        int i = (double)(high - (low + 2)) * ratio() + low;
        assert(i >= low);
        assert(i <= high);
        int value = median3(arr2[low], arr2[high - 1], arr2[i]);
        if (value == arr2[i])
            pivot = i;
        else if (value == arr2[low])
            pivot = low;
        else if (value == arr2[high - 1])
            pivot = high - 1;

        assert(pivot >= low);
        assert(pivot <= high);
    }

    assert(pivot >= 0);
    assert(pivot >= low);
    assert(pivot <= high);
    
    return pivot;
}

// sorts the array using quick sort algorithm
void Quicksort(int *array, int length)
{
    int low = 0;
    int high = length-1;
    quicksort(array, low, high);
}


// sorts the array using quick sort algorithm
// gets: pointer to array, low index, high index
void quicksort(int *array, int low, int high)
{
    if(high>low)
    {
        //set pivot location
        int pivot = place_pivot(array, low, high); 

        // sort: smaller part then larger part
        if(pivot-low < high - pivot)
        {
            quicksort(array, low, pivot-1); // sort bottom part
            quicksort(array, pivot+1, high); // sort top part
        }else
        {
            quicksort(array, pivot+1, high);
            quicksort(array, low, pivot-1);
        }
    }
}

// moves all the objects smaller than low to it's left
// and all the objects bigger than low to it's right
// gets: array, low, high
int place_pivot(int *array, int low, int high)
{
    //int pivot = high;
    int pivot = gen_piv_pem(array, low, high);
	
    int i=low-1;
    int j;
    int save_pivot_arr = array[pivot];
    swap(&array[pivot], &array[high]);
    for ( j = low; j < high; j++) 
    {
        if(array[j]<save_pivot_arr)
        {
			
            i++;
            swap(&array[i], &array[j]); 
			
        }
    }
    swap(&array[i + 1], &array[high]);
	
    assert(i+1>=low);
    assert(i+1<=high);
    return i+1; 
}

bool is_sorted(int *arr, int n)
{
    int i;
    for(i=0; i< n-1; i++)
        if(arr[i+1]<arr[i])
            return false;
    return true;
}


int main(void)
{
    int i;
    int *arr = malloc(sizeof(int)*TRIES);
    srand(time(NULL));
    for(i=0; i<TRIES; i++)
    	arr[i]=rand();
    Quicksort(arr, TRIES);
    
    bool is=is_sorted(arr,TRIES);
    printf("is_sorted is %d\n",is);
    
    printf("Finished.\n");
    return 0;
}

