#ifndef COMMON_H
#define COMMON_H


#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#include <unistd.h>



#define MESSAGE_SIZE 256

typedef struct {
    char text[MESSAGE_SIZE];
} SharedData;

static void fail(const char *operation)
{
    perror(operation);
    exit(EXIT_FAILURE);
}

// для разных пользователей создаются разные объекты
static void make_names(char *shm_name, size_t shm_size,
                       char *data_lock, size_t data_size,
                       char *sender_lock, size_t sender_size)
{
    unsigned long uid = (unsigned long)getuid();

    snprintf(shm_name, shm_size, "/time_pid_shm_%lu", uid);
    snprintf(data_lock, data_size, "/tmp/time_pid_data_%lu.lock", uid);
    snprintf(sender_lock, sender_size, "/tmp/time_pid_sender_%lu.lock", uid);
}

static int open_lock_file(const char *path)
{
    int fd = open(path, O_CREAT | O_RDWR, 0600);
    if (fd == -1)
        fail("open lock file");

    return fd;
}

static void set_lock(int fd, int operation)
{
    while (flock(fd, operation) == -1) {
        if (errno != EINTR)
            fail("flock");
    }
}

static void current_time(char *buffer, size_t size)
{
    time_t now = time(NULL);
    if (now == (time_t)-1)
        fail("time");

    struct tm tm_value;
    if (localtime_r(&now, &tm_value) == NULL)
        fail("localtime_r");

    if (strftime(buffer, size, "%Y-%m-%d %H:%M:%S", &tm_value) == 0) {
        fprintf(stderr, "Недостаточный размер буфера времени\n");
        exit(EXIT_FAILURE);
    }
}

#endif
