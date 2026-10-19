#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

//переменная-флаг
volatile sig_atomic_t sigterm_received = 0;


// оповещение о завершении работы процесса 
void exit_handler(void) {
    printf("Процесс с PID = %d завершил работу\n", getpid());
}

// обработчик сигнала sigint
void sigint_handler(int signal_number) {
    // номер сигнала 
    (void)signal_number;

    // в обработчике сигнала printf использовать нежелательно
    const char msg[] = "Получен SIGINT, номер сигнала: 2\n";

    // выводим сообщение через write
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
}

// обработчик сигнала sigterm
void sigterm_handler(int signal_number) {
    // номер сигнала 
    (void)signal_number;

    // отмечаем, что sigterm получен
    sigterm_received = 1;
}

int main(void) {
    int status;

    //проверка exit_handler
    if (atexit(exit_handler) != 0) {
        perror("atexit");
        return EXIT_FAILURE;
    }

    // установка обработчика для sigint
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("signal SIGINT");
        return EXIT_FAILURE;
    }

    struct sigaction sa_term = {0}; // создание структуры и описание поведения процесса при получении сигнала
    sigemptyset(&sa_term.sa_mask); //очистка маски сигналов
    sa_term.sa_handler = sigterm_handler; // установка обработчика для sigterm
    sa_term.sa_flags = 0; // стандартные параметры

    // установка обработчика для sigterm
    if (sigaction(SIGTERM, &sa_term, NULL) == -1) {
        perror("sigaction SIGTERM");
        return EXIT_FAILURE;
    }

    // создаем новый процесс 
    pid_t pid = fork();

    // проверка ошибки fork
    if (pid < 0) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        // вывод pid и ppid дочернего процесса
        printf("Дочерний процесс: PID = %d, PPID = %d\n",
               getpid(), getppid());

        sleep(10);

        // проверка, был ли получен sigterm
        if (sigterm_received) {
            printf("Дочерний процесс получил SIGTERM\n");
        }

        // завершение дочернего процесса с кодом 0
        exit(EXIT_SUCCESS);
    } else {
        // вывод pid и ppid родительского процесса
        printf("Родительский процесс: PID = %d, PPID = %d\n",
               getpid(), getppid());

        // проверка ожидания 
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            return EXIT_FAILURE;
        }

        // проверка завершения waitpid через return 
        if (WIFEXITED(status)) {
            printf("Дочерний процесс завершился нормально\n");

            // вывод кода завершения дочернего процесса
            printf("Код завершения: %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            // дочерний процесс был завершён сигналом
            printf("Дочерний процесс завершён сигналом %d\n",
                   WTERMSIG(status));
        }
    }

    // нормальное завершение родительского процесса
    return EXIT_SUCCESS;
}