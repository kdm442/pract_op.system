#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>

#define TIME_BUFF 64
#define TEXT_BUFF 128
#define SLEEP 6


void get_time(char *timestr, size_t size) {
    time_t now = time(NULL);
    struct tm *timeinfo = localtime(&now);
    strftime(timestr, size, "%Y-%m-%d %H:%M:%S", timeinfo);
}

int main() {
    int pipe_fd[2];
    if (pipe(pipe_fd) != 0) {
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }

    int read_pipe_fd = pipe_fd[0];
    int write_pipe_fd = pipe_fd[1];

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } 
    else if (pid == 0) {
        // Дочерний процесс
        close(write_pipe_fd); // закрываем конец на запись

        sleep(SLEEP); // задержка согласно условию задания

        char child_timestr[TIME_BUFF];
        get_time(child_timestr, sizeof(child_timestr));
        printf("Child PID : %d , Time: %s\n", getpid(), child_timestr);

        char buff[TEXT_BUFF] = {0};
        if (read(read_pipe_fd, buff, sizeof(buff) - 1) < 0) {
            perror("read failed");
            close(read_pipe_fd);
            exit(EXIT_FAILURE);
        }
        
        printf("Child PID %d ,  message: %s\n", getpid(), buff);
        close(read_pipe_fd);
        exit(EXIT_SUCCESS);
    } 
    else {
        // Родительский процесс
        close(read_pipe_fd); // закрываем конец на чтение

        char parent_timestr[TIME_BUFF];
        get_time(parent_timestr, sizeof(parent_timestr));
        
        char text[TEXT_BUFF];
        snprintf(text, sizeof(text), "parent pid = %d, my time: %s", getpid(), parent_timestr);
        
        if (write(write_pipe_fd, text, strlen(text) + 1) == -1) {
            perror("write failed");
            close(write_pipe_fd);
            exit(EXIT_FAILURE);
        }

        close(write_pipe_fd);
        wait(NULL); // ожидаем завершения дочернего процесса
        exit(EXIT_SUCCESS);
    }

    return 0;
}
