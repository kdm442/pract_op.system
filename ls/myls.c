#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <limits.h>
#include <pwd.h>      
#include <grp.h>

#define COLOR_BLUE  "\033[34m"
#define COLOR_GREEN "\033[32m"
#define COLOR_CYAN  "\033[36m"
#define COLOR_RESET "\033[0m"

void print_colored_name(const char *name, mode_t mode) {
    if (S_ISLNK(mode)) {
        printf(COLOR_CYAN "%s" COLOR_RESET, name);
    } else if (S_ISDIR(mode)) {
        printf(COLOR_BLUE "%s" COLOR_RESET, name);
    } else if (mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
        printf(COLOR_GREEN "%s" COLOR_RESET, name);
    } else {
        printf("%s", name);
    }
}

void print_permissions(mode_t mode) {
    //определение типа файлового объекта
    if (S_ISDIR(mode)) {
        printf("d");
    } else if (S_ISLNK(mode)) {
        printf("l");
    } else {
        printf("-");
    }

    // владелец
    printf((mode & S_IRUSR) ? "r" : "-");
    printf((mode & S_IWUSR) ? "w" : "-");
    printf((mode & S_IXUSR) ? "x" : "-");

    // группа
    printf((mode & S_IRGRP) ? "r" : "-");
    printf((mode & S_IWGRP) ? "w" : "-");
    printf((mode & S_IXGRP) ? "x" : "-");

    // остальные пользователи
    printf((mode & S_IROTH) ? "r" : "-");
    printf((mode & S_IWOTH) ? "w" : "-");
    printf((mode & S_IXOTH) ? "x" : "-");
}

int main(int argc, char *argv[]) {

    int flag_l = 0;
    int flag_a = 0;

    int opt;
    while ((opt = getopt(argc, argv, "la")) != -1) {
        switch (opt){
            case 'l': flag_l = 1; break;
            case 'a': flag_a = 1; break;

            default:
                fprintf(stderr, "Некорректный  флаг ");
                return 1;
        }
    }

        char *path = ".";

        if (optind < argc){
            path = argv[optind];
        }
         // читаем директорию 
        struct dirent **namelist;
        int n=scandir(path, &namelist, NULL, alphasort);

        if (n < 0){
            perror(path);
            return 1;
        }

        for (int i = 0; i < n; i++){
            if(!flag_a && namelist[i] -> d_name[0] == '.'){
                free(namelist[i]);
                continue;
            }

            char fullpath[PATH_MAX];
                snprintf(fullpath,sizeof(fullpath) , "%s/%s" , path,namelist[i]->d_name);//размер буфера и название файла


                //получение инф о файле 
                struct stat st;

                if (lstat(fullpath, &st) == -1){
                    perror(fullpath);
                    free(namelist[i]);
                    continue;
                }
        
            if(flag_l){
                //структуры пользователя и группы
                struct passwd *pw = getpwuid(st.st_uid);
                struct group *gr = getgrgid(st.st_gid);

                //определение времени последнего изменения в файле
                char timebuf[64];
                struct tm *tm_info = localtime(&st.st_mtime);
                 //проверка на ошибку пустого указателя(удалось ли получить время) 
                if (tm_info == NULL) {
                fprintf(stderr, "Ошибка localtime: %s\n", fullpath);
                free(namelist[i]);
                continue;
            }
                strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", tm_info);

                //печать прав 
                print_permissions(st.st_mode);
                printf(" %lu %s %s %lld %s ",
                    (unsigned long)st.st_nlink,//вывод жестких ссылок
                    pw ? pw->pw_name : "?",
                    gr ? gr->gr_name : "?",
                    (long long)st.st_size, //вывод размера файла в байтах
                    timebuf);

                print_colored_name(namelist[i] -> d_name,st.st_mode);
                //чтение и вывод содержимого ссылки
                if(S_ISLNK(st.st_mode)){
                    char target[PATH_MAX];
                    ssize_t len =readlink(fullpath,target,sizeof(target) - 1);
                    if (len != -1) {
                        target[len] = '\0';
                        printf(" -> %s", target);
                    } 
                }

            printf("\n");    
            }else{
                print_colored_name(namelist[i]->d_name, st.st_mode);
                printf("  ");
            }
                free(namelist[i]);
            }
            free(namelist);
            
            if (!flag_l){
                printf("\n");
        }
        return 0; 
}


