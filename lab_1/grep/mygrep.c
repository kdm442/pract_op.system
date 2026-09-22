#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {

    if (argc < 2){
        fprintf(stderr, "Неправильный ввод");
            return 1;
    }

    char line[1024];
    char *pattern = argv[1];
    int found = 0;

    FILE *file = stdin;
        if (argc >=3){
            file = fopen(argv[2], "r");
            if (file == NULL) {
                perror(argv[2]);
                return 1;
            }   
        }

    while (fgets(line,sizeof(line),file) != NULL){
        if(strstr(line,pattern) != NULL){
            printf("%s",line);
            found = 1;
        }
    } 

    if(found < 1 ){printf("Данный pattern отсутствует\n");
            return 1;
        }

    if (file != stdin) {
        fclose(file);
    }
    return 0;
}
    



