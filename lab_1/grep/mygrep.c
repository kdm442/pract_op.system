#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

    int flag_i = 0;
    int flag_v= 0;
    int flag_n = 0;
    int flag_in = 0;
    int flag_c = 0;

    int opt;
    while ((opt = getopt(argc, argv, "nbE")) != -1) {

        switch (opt){
            case 'i': flag_i = 0; = 1; break;
            case 'v': flag_v= 0; = 1; break;
            case 'n': flag_n = 0; = 1; break;
            case 'in': flag_in = 0; = 1; break;
            case 'c': flag_c = 0; = 1; break;

            default:
                fprintf(stderr, "Отсутствует флаг ");
                return 1;
        }
    }

    FILE *file = NULL;
        if (optind >= argc){
            file = stdin;
        }
        else{
            file = fopen(argv[optind], "r");
            if (file == NULL) {
                perror(argv[optind]);
                return 1;
            }
        }

    int c;
    int line_number = 1;
    int start_line = 1;

    while ((c = fgetc(file)) != EOF) {
        if (start_line) {
            if (flag_b) {
                if (c != '\n') {
                    printf("%6d\t", line_number++);}
                    start_line = 0;
            }
                else {
                    if (flag_n){
                        printf("%6d\t", line_number++);
                        start_line = 0;
                    }
                else{
                    start_line = 0;
                }
                }  
            }      

        if (c == '\n') {
            if (flag_E) {
                putchar('$');
            }
            putchar('\n');
            start_line = 1;
        } else {
            putchar(c);
        }
    }

    if (file != stdin) {
        fclose(file);
    }
}


