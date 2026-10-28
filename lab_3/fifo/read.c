#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/stat.h>
#include <errno.h>


#define FIFO_PATH "/tmp/my_fifo_pipe"
#define TIME_LEN 64
#define MSG_LEN 256
#define SLEEP_SEC 10

void get_time(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", t);
}

int main() {
    if (mkfifo(FIFO_PATH, 0666) == -1) {
    if (errno != EEXIST) {
        perror("mkfifo failed");
        return 1;
    }
}

    printf("Reader PID: %d. Sleeping for %d seconds...\n", getpid(), SLEEP_SEC);
    sleep(SLEEP_SEC);

    char time_str[TIME_LEN];
    get_time(time_str, sizeof(time_str));

    int fd = open(FIFO_PATH, O_RDONLY);
    if (fd < 0) {
        perror("open failed");
        return 1;
    }

    char message[MSG_LEN] = {0};
    if (read(fd, message, sizeof(message) - 1) == -1) {
        perror("read failed");
        close(fd);
        return 1;
    }

    printf("Reader time: %s\n", time_str);
    printf("Received: %s\n", message);

    close(fd);
    unlink(FIFO_PATH); // удаляем файл канала
    return 0;
}
