#define _POSIX_C_SOURCE 200809L

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#define READERS_COUNT 10
#define ITERATIONS 20
#define BUFFER_SIZE 64

static char buffer[BUFFER_SIZE] = "Запись 0";
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static void check(int error, const char *operation)
{
    if (error != 0) {
        fprintf(stderr, "%s: %s\n", operation, strerror(error));
        exit(EXIT_FAILURE);
    }
}

static void pause_ms(long ms)
{
    struct timespec delay = {
        .tv_sec = ms / 1000,
        .tv_nsec = (ms % 1000) * 1000000L
    };

    while (nanosleep(&delay, &delay) == -1) {
        if (errno != EINTR) {
            perror("nanosleep");
            exit(EXIT_FAILURE);
        }
    }
}

static void *writer(void *arg)
{
    (void)arg;

    for (int counter = 1; counter <= ITERATIONS; ++counter) {
        check(pthread_mutex_lock(&mutex), "mutex_lock");

        snprintf(buffer, sizeof(buffer), "Запись %d", counter);

        check(pthread_mutex_unlock(&mutex), "mutex_unlock");
        pause_ms(200);
    }

    return NULL;
}

static void *reader(void *arg)
{
    int number = *(const int *)arg;
    pthread_t tid = pthread_self();

    for (int i = 0; i < ITERATIONS; ++i) {
        check(pthread_mutex_lock(&mutex), "mutex_lock");

        printf("Читатель %2d | tid=%lu | %s\n",
               number, (unsigned long)tid, buffer);

        check(pthread_mutex_unlock(&mutex), "mutex_unlock");
        pause_ms(200);
    }

    return NULL;
}

int main(void)
{
    pthread_t writer_thread;
    pthread_t readers[READERS_COUNT];
    int numbers[READERS_COUNT];

    check(pthread_create(&writer_thread, NULL, writer, NULL),
          "create writer");

    for (int i = 0; i < READERS_COUNT; ++i) {
        numbers[i] = i + 1;
        check(pthread_create(&readers[i], NULL, reader, &numbers[i]),
              "create reader");
    }

    check(pthread_join(writer_thread, NULL), "join writer");

    for (int i = 0; i < READERS_COUNT; ++i) {
        check(pthread_join(readers[i], NULL), "join reader");
    }

    check(pthread_mutex_destroy(&mutex), "mutex_destroy");

    return EXIT_SUCCESS;
}
