#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <sys/stat.h>

#define FIFO_PATH "/tmp/my_fifo_pipe"
#define TIME_LEN 64
#define MSG_LEN 256

void get_time(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", t);
}

int main() {
    // создаем канал, если его еще нет
    if (mkfifo(FIFO_PATH, 0666) == -1 && errno != EEXIST) {
        perror("mkfifo failed");
        return 1;
    }

    char time_str[TIME_LEN];
    get_time(time_str, sizeof(time_str));

    char message[MSG_LEN];
    snprintf(message, sizeof(message), "Writer PID: %d, time: %s", getpid(), time_str);

    printf("Opening FIFO (waiting for reader)...\n");

    int fd = open(FIFO_PATH, O_WRONLY);
    if (fd < 0) {
        perror("open failed");
        return 1;
    }

    if (write(fd, message, strlen(message) + 1) == -1) {
        perror("write failed");
        close(fd);
        return 1;
    }

    printf("Sent: %s\n", message);

    close(fd);
    return 0;
}
