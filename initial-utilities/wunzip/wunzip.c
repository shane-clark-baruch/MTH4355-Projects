#include <stdio.h>

void wunzip(FILE *fp){
    char ch;
    int num = 0;
    while(fread(&num, sizeof(num), 1, fp) > 0 && fread(&ch, sizeof(ch), 1, fp) > 0) {
        for (int i = 0; i < num; i++)
            printf("%c", ch);
    }
}

int main(int argc, char *argv[]){
    if (argc < 2){
        printf("wunzip: file1 [file2 ...]\n");
        return 1;
    }
    FILE *fp = NULL;
    for (int i = 1; i < argc; i++ ){
         fp = fopen(argv[i], "rb");
         if (fp == NULL){
            printf("wunzip: can't open file\n");
            return 1;
         }
         wunzip(fp);
         fclose(fp);
    }
    return 0;
}
