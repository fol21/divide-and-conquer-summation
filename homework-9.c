#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>

#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))
  
sem_t mutex;

typedef struct divide_and_conquer_args { int length; int* parts;} divide_and_conquer_args;

int* copy(int* arr, int start, int end)
{
    int _l = end - start + 1;
    int* _arr = (int*) malloc(sizeof(*arr) * _l);
    memcpy(_arr, &arr[start], _l * sizeof(*arr));
    return _arr;
    
}

int divide_and_conquer_sum(int length, int* parts)
{
    if(length == 2)
    {
        return (parts[0] + parts[1]);
    }
    else if(length == 1)
    {
         return  parts[0];
    }
    else
    {
        int _l = length / 2;
        int _lp = length / 2 + length % 2;

        // Left
        int* _div = copy(parts, 0, _l - 1); 
        int left = divide_and_conquer_sum(_l, _div);
        
        // Right
        int* _remainder = copy(parts, _l, _l + _lp - 1); 
        int right = divide_and_conquer_sum(_lp, _remainder);

        return left + right; 
    }
}

void* divide_and_conquer_sum_thread(void* arg)
{
    divide_and_conquer_args* args = (divide_and_conquer_args*) arg;
    int* data = (int*) malloc(sizeof(int));
    // //wait
    // sem_wait(&mutex);
    //  *data = args[0] + args[1];
    // //signal
    // sem_post(&mutex);   
    // return data;

    if(args->length == 2)
    {
        *data = (args->parts[0] + args->parts[1]);
        return data;
    }
    else if(args->length == 1)
    {
        *data = (args->parts[0] + args->parts[1]);
         return  data;
    }
    else
    {
        int _l = args->length / 2;
        int _lp = args->length / 2 + args->length % 2;

        // Left
        int* _div = copy(args->parts, 0, _l - 1); 
        pthread_t left_thd;
        divide_and_conquer_args largs = {_l, _div};
        pthread_create(&left_thd, NULL, divide_and_conquer_sum_thread, &largs);
        int* left;

        // Right
        int* _remainder = copy(args->parts, _l, _l + _lp - 1); 
        pthread_t right_thd;
        divide_and_conquer_args rargs = {_lp, _remainder};
        pthread_create(&right_thd, NULL, divide_and_conquer_sum_thread, &rargs);
        int* right;

        pthread_join(left_thd, &left);
        pthread_join(right_thd, &right);

        *data = *left + *right;
        return data; 
    }
}

  
int main(int argc, char *argv[])
{
    sem_init(&mutex, 0, 1);

    // pthread_t prod_thds;
    // int arg[2]  = {1,2};
    // int* result;
    // pthread_create(&prod_thds, NULL, divide_and_conquer_sum_thread, arg);
    // int arr[5] = {10,-10,10,-10,2};
    int arr[5] = {1,2,3,4,5};
    int sum = 0;
    sum = divide_and_conquer_sum( NELEMS(arr), arr);
    printf("%d\n", sum);
    // pthread_join(prod_thds, &result);

    pthread_t divide_and_conquer_thd;
    int* result;
    divide_and_conquer_args args = {NELEMS(arr), arr};
    pthread_create(&divide_and_conquer_thd, NULL, divide_and_conquer_sum_thread, &args);

    pthread_join(divide_and_conquer_thd, &result);
    printf("%d\n", *result);
    return 0;
}