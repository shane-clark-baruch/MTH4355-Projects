#include <stdio.h>
#include <string.h>

int grep(FILE *fp, char *str){
    char *line = NULL;
    size_t len = 0;
    while(getline(&line, &len, fp) > 0){
        if (strstr(line, str) != NULL) { 
            printf("%s", line);
        }
    }
    return 0;
}

int main(int argc, char *argv[]){
    if (argc < 2)
    {
        printf("wgrep: searchterm [file ...]\n");
        return 1;
    } else if (argc == 2) {
        return grep(stdin, argv[1]);
    }

    int rval = 0; 
    for (int i = 2; i < argc; i++){
        FILE *fp = fopen(argv[i], "r");
        if (fp == NULL) {
            printf("wgrep: cannot open file\n");
            return 1;
        }
        rval = grep(fp, argv[1]);
        fclose(fp);
        if (rval != 0)
            return rval;
    }
    return rval;
}
