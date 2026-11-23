
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>

#define NUM_THREADS 5

void *printHello(void *threadid)
{
    long thread_id = (long)(intptr_t)threadid;

    printf("Hello World! It's a me, thread #%ld!\n", thread_id);

    pthread_exit(threadid);
}

int main(void)
{
    pthread_t threads[NUM_THREADS];
    int return_code;
    long thread_index;

    for (thread_index = 0; thread_index < NUM_THREADS; thread_index++) {
        printf("In main: creating thread %ld\n", thread_index);

        return_code = pthread_create(
            &threads[thread_index],
            NULL,
            printHello,
            (void *)(intptr_t)thread_index
        );

        if (return_code != 0) {
            fprintf(stderr,
                    "Error: pthread_create() returned %d\n",
                    return_code);
            exit(EXIT_FAILURE);
        }
    }

    void *status[NUM_THREADS];

    for (thread_index = 0; thread_index < NUM_THREADS; thread_index++) {
        return_code = pthread_join(
            threads[thread_index],
            &status[thread_index]
        );

        if (return_code != 0) {
            fprintf(stderr,
                    "Error: pthread_join() returned %d\n",
                    return_code);
            exit(EXIT_FAILURE);
        }

        printf("Everything was completed for thread %ld! Status %ld\n",
               thread_index,
               (long)(intptr_t)status[thread_index]);
    }

    return 0;
}
