#include "common.h"


#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
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

    //блокировка единственного экземпляра передатчика
    int instance_fd = open_lock_file(sender_lock_name);

    if (flock(instance_fd, LOCK_EX | LOCK_NB) == -1) {
        if (errno == EWOULDBLOCK || errno == EAGAIN) {
            fprintf(stderr,
                    "Передатчик уже запущен. "
                    "Повторный запуск запрещён.\n");

            close(instance_fd);
            return EXIT_FAILURE;
        }

        fail("flock singleton");
    }

    int data_fd = open_lock_file(data_lock_name);

    // ащищаем инициализацию разделяемой памяти
    set_lock(data_fd, LOCK_EX);

    int shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0600);
    if (shm_fd == -1)
        fail("shm_open");

    struct stat info;

    if (fstat(shm_fd, &info) == -1)
        fail("fstat");

    if (info.st_size == 0) {
        // задаём размер только новому, ещё пустому объекту
        if (ftruncate(shm_fd, (off_t)sizeof(SharedData)) == -1)
            fail("ftruncate");
    } else if (info.st_size < (off_t)sizeof(SharedData)) {
        fprintf(stderr,
                "Разделяемая память слишком мала: "
                "%lld байт, требуется минимум %zu байт\n",
                (long long)info.st_size,
                sizeof(SharedData));

        close(shm_fd);
        close(data_fd);
        close(instance_fd);

        return EXIT_FAILURE;
    }

    SharedData *shared = mmap(NULL,
                              sizeof(SharedData),
                              PROT_READ | PROT_WRITE,
                              MAP_SHARED,
                              shm_fd,
                              0);

    if (shared == MAP_FAILED)
        fail("mmap");

    close(shm_fd);

    memset(shared, 0, sizeof(*shared));

    set_lock(data_fd, LOCK_UN);

    printf("Передатчик запущен: PID=%ld\n", (long)getpid());
    fflush(stdout);

    for (;;) {
        char time_buffer[64];
        SharedData message = {0};

        current_time(time_buffer, sizeof(time_buffer));

        snprintf(message.text, sizeof(message.text),
                 "Время передатчика: %s; PID передатчика: %ld",
                 time_buffer,
                 (long)getpid());

        set_lock(data_fd, LOCK_EX);

        memcpy(shared, &message, sizeof(message));

        set_lock(data_fd, LOCK_UN);

        sleep(1);
    }
}
