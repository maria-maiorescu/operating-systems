
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>

void *functie(void *threadid);

void *functie(void *threadid)
{
    long level = (long)(intptr_t)threadid;

    for (int i = 0; i < level; i++) {
        printf("I am the son! My id is %ld\n", level);
    }

    pthread_exit((void *)(intptr_t)6);
}

int main(void)
{
    pthread_t thread, thread1;
    long id = 5;

    void *status, *status1;

    int rc = pthread_create(&thread, NULL, functie,
                            (void *)(intptr_t)id);

    if (rc != 0) {
        printf("ERROR: return code from pthread_create() is %d\n", rc);
        exit(-1);
    }

    id = 3;

    int rc1 = pthread_create(&thread1, NULL, functie,
                             (void *)(intptr_t)id);

    if (rc1 != 0) {
        printf("ERROR: return code from pthread_create() is %d\n", rc1);
        pthread_join(thread, NULL);
        exit(-1);
    }

    printf("This is the father!\n");

    rc = pthread_join(thread, &status);
    rc1 = pthread_join(thread1, &status1);

    if (rc != 0 || rc1 != 0) {
        fprintf(stderr, "ERROR: pthread_join() failed\n");
        return 1;
    }

    printf("Everything was completed! Status %ld\n",
           (long)(intptr_t)status);

    return 0;
}
