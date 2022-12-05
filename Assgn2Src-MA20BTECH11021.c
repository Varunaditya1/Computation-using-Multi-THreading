#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
// Data structure to store the start value and the no of values in that thread
struct ThreadParams_s
{
    int start;
    int count;
};
// Data structure to store the final count of Perfect No in that thread and their respective values.
struct ThreadResult_s
{
    int *data;
    int count;
};

void *thread_runner(void *args)
{
    struct ThreadParams_s *params = (struct ThreadParams_s *)args; 
    struct ThreadResult_s *result = calloc(1, sizeof(struct ThreadResult_s));  //Assigning the memory to store the result of that thread.
    result->data = calloc(params->count, sizeof(int));
    result->count = 0;
    FILE *fp;
    char filename[200];
    sprintf(filename, "OutFile_%d", params->start); // Since the no of threads vary , hence no of output files vary so we use sprintf to assign names to the files in terms of that thread no.
    fp = fopen(filename, "w");
    for (int i = params->start; params->count > 0; i++, params->count--)
    {
        // Running the algoritm to find whether that no is a Perfect No or not.
        int j = 1, sum = 0;
        while (j < i)
        {
            if (i % j == 0)
                sum += j;
            j++;
        }
        if (sum == i)
        {
            result->data[result->count] = i;
            result->count++;
            fprintf(fp, "%d : Is a Perfect Number.\n", i);
        }
        else
            fprintf(fp, "%d : Not a Perfect Number.\n", i);
    }
    fclose(fp);         //Freeing up the memory.
    pthread_exit(result);
}

int main()
{
    int n, k;
    FILE *inp;
    inp = fopen("input.txt", "r");
    fscanf(inp, "%d %d", &n, &k); // main thread taking input and later freeing that pointer to the input file.
    fclose(inp);
    if(n<0 || k<0){
        printf("The input file contains negative integers.");       //If the input is wrong.
        return 1;
    }
    int start = 1, count = n / k;
    if (n % k != 0)
    {
        count++;                // The count of numbers to be added in each thread.
    }
    pthread_t *ptid = calloc(k, sizeof(pthread_t));
    struct ThreadParams_s *params = calloc(k, sizeof(struct ThreadParams_s));       //Assigning memory and values to the Threads parameters.
    int threadCount = 0;
    for (int i = 0; i < k && start <= n; i++)
    {
        params[i].start = start;
        params[i].count = count;
        if (start + count > n)
            params[i].count = n - start + 1;
        start += count;
        pthread_create(&ptid[i], NULL, &thread_runner, &params[i]);

        threadCount++;
    }
    FILE *output;
    output = fopen("OutMain", "w");             //Creating final output file and storing the data using the result data-structure made above.
    for (int i = 0; i < threadCount; i++)
    {
        void *res = NULL;
        pthread_join(ptid[i], &res);
        struct ThreadResult_s *result = res;
        for (int j = 0; j < result->count; j++)
        {
            fprintf(output, "Thread%d : %d ", i + 1, result->data[j]);
        }
        if (result->count == 0)
        {
            fprintf(output, "Thread%d :  \n", i + 1);
        }
        else
            fprintf(output, "\n");
        free(result->data);
        free(result);
    }
    fclose(output);
    free(ptid);             //Freeing up all the pointers.
    free(params);
    return 0;
}