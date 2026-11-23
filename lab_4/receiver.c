#include "common.h"


#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

int main(void)
{
    char shm_name[128];
    char data_lock_name[128];
    char sender_lock_name[128];

    make_names(shm_name, sizeof(shm_name),
               data_lock_name, sizeof(data_lock_name),
               sender_lock_name, sizeof(sender_lock_name));

    int data_fd = open_lock_file(data_lock_name);

    set_lock(data_fd, LOCK_SH);

    int shm_fd = shm_open(shm_name, O_RDONLY, 0600);
    if (shm_fd == -1) {
        if (errno == ENOENT) {
            fprintf(stderr,
                    "Разделяемая память ещё не создана. "
                    "Сначала запустите sender.\n");
            return EXIT_FAILURE;
        }

        fail("shm_open");
    }

    struct stat info;
    if (fstat(shm_fd, &info) == -1)
        fail("fstat");

    printf("Размер общей памяти: %lld байт; требуется: %zu байт\n",
       (long long)info.st_size,
       sizeof(SharedData));

    if (info.st_size < (off_t)sizeof(SharedData)) {
        fprintf(stderr, "Недостаточный размер разделяемой памяти\n");
        return EXIT_FAILURE;
    }


    const SharedData *shared = mmap(NULL, sizeof(SharedData),
                                    PROT_READ, MAP_SHARED, shm_fd, 0);
    if (shared == MAP_FAILED)
        fail("mmap");

    close(shm_fd);
    set_lock(data_fd, LOCK_UN);

    for (;;) {
        SharedData message;
        char time_buffer[64];

        set_lock(data_fd, LOCK_SH);
        memcpy(&message, shared, sizeof(message));
        set_lock(data_fd, LOCK_UN);

        message.text[MESSAGE_SIZE - 1] = '\0';

        current_time(time_buffer, sizeof(time_buffer));

        printf("Приёмник: время=%s; PID=%ld | Принято: %s\n",
               time_buffer,
               (long)getpid(),
               message.text[0] != '\0'
                   ? message.text
                   : "(ожидание первой записи)");

        fflush(stdout);
        sleep(1);
    }
}
