#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

void print_permissions(mode_t mode) {
    printf("%c", (mode & S_IRUSR) ? 'r' : '-');
    printf("%c", (mode & S_IWUSR) ? 'w' : '-');
    printf("%c", (mode & S_IXUSR) ? 'x' : '-');

    printf("%c", (mode & S_IRGRP) ? 'r' : '-');
    printf("%c", (mode & S_IWGRP) ? 'w' : '-');
    printf("%c", (mode & S_IXGRP) ? 'x' : '-');

    printf("%c", (mode & S_IROTH) ? 'r' : '-');
    printf("%c", (mode & S_IWOTH) ? 'w' : '-');
    printf("%c", (mode & S_IXOTH) ? 'x' : '-');
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Неправильный ввод");
        return 1;
    }

    char *mode_string = argv[1];
    char *path = argv[2];

    int numeric_mode = 0;
    mode_t numeric_permissions = 0;


    int who_u = 0;
    int who_g = 0;
    int who_o = 0;

    int perm_r = 0;
    int perm_w = 0;
    int perm_x = 0;

    char op = 0;

    int i = 0; 

    //проверка первого символа 
    if (mode_string[0] >= '0' && mode_string[0] <= '9') {
    if (strlen(mode_string) != 3) {
        fprintf(stderr, "Нужны ровно три цифры прав\n");
        return 1;
    }

    for (int j = 0; j < 3; j++) {
        if (mode_string[j] < '0' || mode_string[j] > '7') {
            fprintf(stderr, "Допустимы только цифры 0–7\n");
            return 1;
        }
    }

    numeric_permissions = (mode_t)strtol(mode_string, NULL, 8);
    numeric_mode = 1;
} else {

    //кто
    while (mode_string[i] == 'u' || mode_string[i] == 'g' || mode_string[i] == 'o' || mode_string[i] == 'a') {
        switch (mode_string[i]) {
        case 'u': who_u = 1; break;
        case 'g': who_g = 1; break;
        case 'o': who_o = 1; break;
        case 'a': who_u = who_g = who_o = 1; break;
    }
    i++;
}


    //операция
    if (mode_string[i] == '+' || mode_string[i] == '-' || mode_string[i] == '=') {
        op = mode_string[i];
        i++;
    } else {
        fprintf(stderr, "Неверный оператор\n");
        return 1;
    }

    //права
    while (mode_string[i] == 'r' || mode_string[i] == 'w' || mode_string[i] == 'x') {
        switch (mode_string[i]) {
        case 'r': perm_r = 1; break;
        case 'w': perm_w = 1; break;
        case 'x': perm_x = 1; break;
        }
    i++;
    }

    //проверка на наличие мусора
    if (mode_string[i] != '\0') {
    fprintf(stderr, "Неверный формат прав\n");
    return 1;
    }
}

    struct stat st;

    // получаем  права целевого файла для chmod
    if (stat(path, &st) == -1) {
        perror(path);
        return 1;
    }

    printf("Права до изменения: ");
    print_permissions(st.st_mode);
    printf("\n");

    mode_t new_mode = st.st_mode & 0777; //биотвая маска прав
    mode_t mask = 0;
    mode_t clear_mask = 0;


    // если пользователь не указал who, можно считать как 'a'
    if (!who_u && !who_g && !who_o) {
        who_u = who_g = who_o = 1;
    }

    // собираем mask из perm_r / perm_w / perm_x
    if (perm_r) {
        if (who_u) mask |= S_IRUSR;
        if (who_g) mask |= S_IRGRP;
        if (who_o) mask |= S_IROTH;
    }
    if (perm_w) {
        if (who_u) mask |= S_IWUSR;
        if (who_g) mask |= S_IWGRP;
        if (who_o) mask |= S_IWOTH;
    }
    if (perm_x) {
        if (who_u) mask |= S_IXUSR;
        if (who_g) mask |= S_IXGRP;
        if (who_o) mask |= S_IXOTH;
    }

    // что очищать при '='
    if (who_u) clear_mask |= S_IRUSR | S_IWUSR | S_IXUSR;
    if (who_g) clear_mask |= S_IRGRP | S_IWGRP | S_IXGRP;
    if (who_o) clear_mask |= S_IROTH | S_IWOTH | S_IXOTH;

    // применяем операцию
    if (numeric_mode) {
    new_mode = numeric_permissions;
    } else {
        if (op == '+') {
            new_mode |= mask;
        } else if (op == '-') {
            new_mode &= ~mask;
        } else if (op == '=') {
            new_mode &= ~clear_mask;
            new_mode |= mask;
        }
    }


    if (chmod(path, new_mode) == -1) {
        perror("chmod");
        return 1;
    }

    // повторно читаем реальные права после изменения
    if (stat(path, &st) == -1) {
        perror(path);
        return 1;
    }

    printf("Права после изменения: ");
    print_permissions(st.st_mode);
    printf("\n");

    return 0;
}
