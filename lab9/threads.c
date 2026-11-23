/**
 * Write a C program consisting of a single process in which 6 threads 
 * of execution (including the initial one) run. 
 * 
 * The process creates an array of 5 characters, 
 * whose elements are initialized with '#'. 
 * 
 * There will be 5 "producer" threads in the process that 
 * will continuously fill in a random location in the array with one character each, as follows: 
 *  - the first thread with the character 'A', 
 *  - the second with 'B' and so on up to 'E'. 
 * 
 * Each "producer" thread will count how many characters it has entered in the array; 
 * the first thread will terminate when it has entered 100,000 characters, the second 200,000 and so on.
 *  
 * The main thread will continuously display the contents of the array until all the "producer" threads terminate, 
 * at which point it will take over their state and terminate the process.
 * 
 * Hint: to announce the termination of the "producer" threads, each of them will set a distinct global variable, the main thread testing all these variables.
 */


#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <time.h>

#define NUM_THREADS 5

char array[NUM_THREADS] = {'#', '#', '#', '#', '#'};
int finished[NUM_THREADS] = {0};

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *printHello(void *threadid)
{
    long thread_id = (long)(intptr_t)threadid;
    char c = 'A' + (char)thread_id;

    unsigned int seed = (unsigned int)time(NULL)
                      ^ (unsigned int)(thread_id + 1) * 12345U;

    for (int i = 0; i < 100000 * (thread_id + 1); i++)
    {
        int position = rand_r(&seed) % NUM_THREADS;

        pthread_mutex_lock(&mutex);
        array[position] = c;
        pthread_mutex_unlock(&mutex);
    }

    pthread_mutex_lock(&mutex);
    finished[thread_id] = 1;
    pthread_mutex_unlock(&mutex);

    pthread_exit(threadid);
}

int main(void)
{
    pthread_t threads[NUM_THREADS];
    int return_code;
    long thread_index;

    // Create 5 producer threads
    for (thread_index = 0; thread_index < NUM_THREADS; thread_index++)
    {
        printf("In main: creating thread %ld\n", thread_index);

        return_code = pthread_create(
            &threads[thread_index],
            NULL,
            printHello,
            (void *)(intptr_t)thread_index
        );

        if (return_code != 0)
        {
            printf("Error creating thread: %d\n", return_code);
            exit(-1);
        }
    }

    // Main thread displays the array
    int all_finished;

    do
    {
        char snapshot[NUM_THREADS];
        all_finished = 1;

        pthread_mutex_lock(&mutex);

        for (int i = 0; i < NUM_THREADS; i++)
        {
            snapshot[i] = array[i];

            if (finished[i] == 0)
                all_finished = 0;
        }

        pthread_mutex_unlock(&mutex);

        printf("Array: ");

        for (int i = 0; i < NUM_THREADS; i++)
            printf("%c", snapshot[i]);

        printf("\n");

        if (!all_finished)
        {
            struct timespec pause = {0, 10000000L};
            nanosleep(&pause, NULL);
        }

    } while (!all_finished);

    // Wait for threads and retrieve their statuses
    void *status[NUM_THREADS];

    for (thread_index = 0; thread_index < NUM_THREADS; thread_index++)
    {
        return_code = pthread_join(
            threads[thread_index],
            &status[thread_index]
        );

        if (return_code != 0)
        {
            printf("Error joining thread: %d\n", return_code);
            exit(-1);
        }

        printf("Thread %ld finished! Status %ld\n",
               thread_index,
               (long)(intptr_t)status[thread_index]);
    }

    pthread_mutex_destroy(&mutex);

    return 0;
}
