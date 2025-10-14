#include <stdio.h>
#include <string.h>
#include <stdint.h>

void zip(FILE *fp, FILE *output){
    char ch, prev = EOF;
    int32_t count = 0;
    while((ch = fgetc(fp)) != EOF){
        if (ch != prev && prev != EOF) { 
            fwrite(&count, sizeof(int32_t), 1, output);
            fwrite(&prev, sizeof(char), 1, output);
            count = 1;
        } else {
            count++;
        }
        prev = ch; 
    }
    if (count > 0){
        fwrite(&count, sizeof(int32_t), 1, output);
        fwrite(&prev, sizeof(char), 1, output);
    }
}

int main(int argc, char *argv[]){
    if (argc < 2){
        printf("wzip: file1 [file2 ...]\n");
        return 1;
    }
    FILE *fp = NULL;
    FILE *output = stdout;
    int i, j = 0;
    for (j = 1; j < argc; j++ ){
        if ((strcmp(argv[j], ">") == 0) && (j+1 < argc)) {
            output = fopen(argv[j+1], "wb");
            break; 
        }
    }
    
    char ch, prev = EOF;
    int32_t count = 0;
  
    for (i = 1; i < j; i++ ){
        fp = fopen(argv[i], "rb");
        if (fp == NULL){
            printf("wzip: can't open file\n");
            return 1;
        }
        while((ch = fgetc(fp)) != EOF){
            if (ch != prev && prev != EOF) { 
                fwrite(&count, sizeof(int32_t), 1, output);
                fwrite(&prev, sizeof(char), 1, output);
                count = 1;
            } else {
                count++;
            }
            prev = ch; 
        }
        fclose(fp);
    }
    if (count > 0){
        fwrite(&count, sizeof(int32_t), 1, output);
        fwrite(&prev, sizeof(char), 1, output);
    }
    if (output != stdout)
        fclose(output);
    return 0;
}
