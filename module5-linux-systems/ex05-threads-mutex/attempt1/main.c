#include <stdio.h>
#include <stdint.h>
#include <pthread.h>

// shared counters
#define ITERATIONS      100000

static int counter_unsafe = 0;
static int counter_safe   = 0;
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

// thread functions
void *thread_unsafe(void *arg)
{
    (void)arg;
    for (int i = 0; i < ITERATIONS; i++) {
        counter_unsafe++;   /* no protection — race condition */
    }
    return NULL;
}

void *thread_safe(void *arg)
{
    (void)arg;
    for (int i = 0; i < ITERATIONS; i++) {
        pthread_mutex_lock(&mtx);
        counter_safe++;     /* protected — always correct */
        pthread_mutex_unlock(&mtx);
    }
    return NULL;
}

int main(void)
{
    pthread_t t1, t2, t3, t4;

    // race condition - no mutex
    printf("=== race condition (no mutex) ===\n");
    printf("two threads each increment %d times\n", ITERATIONS);
    printf("expected = %d\n\n", ITERATIONS * 2);

    pthread_create(&t1, NULL, thread_unsafe, NULL);
    pthread_create(&t2, NULL, thread_unsafe, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("unsafe result = %d\n", counter_unsafe);
    printf("lost updates  = %d\n", ITERATIONS * 2 - counter_unsafe);

    // mutex protected — always correct
    printf("\n=== mutex protected ===\n");
    pthread_create(&t3, NULL, thread_safe, NULL);
    pthread_create(&t4, NULL, thread_safe, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);

    printf("safe result   = %d ✓\n", counter_safe);

    pthread_mutex_destroy(&mtx);

    return 0;
}
