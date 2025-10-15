#include <stdio.h>

int cat(char *filename){
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("wcat: cannot open file\n");
        return 1;
    }
    char buf[100];
    while(fgets(buf, sizeof(buf), fp)  != NULL){
        printf("%s", buf);
    }
    fclose(fp);
    return 0;
}

int main(int argc, char *argv[]){
    
    if (argc < 2)
    {
        return 0;
    }
    int rval = 0; 
    for (int i = 1; i < argc; i++){
        rval = cat(argv[i]);
        if (rval != 0)
            return rval;
    }
    return rval;
}
