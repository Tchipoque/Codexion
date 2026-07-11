#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 10
#define INCREMENTS 100000

int counter = 0;
pthread_mutex_t counter_mutex;

void *increment_counter(void *arg)
{
    (void)arg;

    for (int i = 0; i < INCREMENTS; i++)
    {
        pthread_mutex_lock(&counter_mutex);
        counter++;
        pthread_mutex_unlock(&counter_mutex);

    }

    return NULL;
}

int main(void)
{
    pthread_t threads[NUM_THREADS];
    pthread_mutex_init(&counter_mutex, NULL);

    for (int i = 0; i < NUM_THREADS; i++)
    {
        if (pthread_create(&threads[i], NULL, increment_counter, NULL) != 0)
        {
            printf("Error creating thread\n");
            return (1);
        }
    }

    for (int i = 0; i < NUM_THREADS; i++)
        pthread_join(threads[i], NULL);

    pthread_mutex_destroy(&counter_mutex);
    printf("Expected counter: %d\n", NUM_THREADS * INCREMENTS);
    printf("Actual counter:   %d\n", counter);

    return (0);
}
