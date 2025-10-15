#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int filecmp(char *f1, char *f2){
    FILE *fp1 = fopen(f1, "rb");
    FILE *fp2 = fopen(f2, "rb");
    if (fp2 == NULL)
        return 1;

    fseek(fp1, 0, SEEK_END);
    fseek(fp2, 0, SEEK_END);
    long size1 = ftell(fp1);
    long size2 = ftell(fp2);
    rewind(fp1);
    rewind(fp2);

    if (size1 != size2)
        return 1;
    char ch1, ch2; 
    while (( ch1 = fgetc(fp1)) != EOF && (ch2 = fgetc(fp2)) != EOF){
        if (ch1 != ch2){
            fclose(fp1);
            fclose(fp2);
            return 1;
        }
    }
    fclose(fp1);
    fclose(fp2);
    return 0;
}

int main(int argc, char *argv[]){
    FILE *fIn = stdin;
    FILE *fOut = stdout;
    if (argc > 3){
        fprintf(stderr, "usage: reverse <input> <output>\n");
        exit(1);
    } else if (argc > 1) {
        fIn = fopen(argv[1], "r"); 
        if (fIn == NULL) {
            fprintf(stderr, "reverse: cannot open file '%s'\n", argv[1]);
            exit(1);    
        }
        if (argc == 3) {
            if (strcmp(argv[1], argv[2]) == 0 || filecmp(argv[1], argv[2]) == 0){
                fprintf(stderr, "reverse: input and output file must differ\n");
                exit(1);
            }
            fOut = fopen(argv[2], "w+");
        }
    } else {
        
    }
    char *line;
    size_t n;
    int i, line_num = 0;
    // Count the line
    while(getline(&line, &n, fIn) > 0){
        line_num++;
    }
    
    char *lines[line_num+1];
    rewind(fIn);
    
    i = 0;
    while(getline(&line, &n, fIn) > 0){
        lines[i] = (char *)malloc(n * sizeof(char));
        strcpy(lines[i], line);
        i++; 
    }
    for (i = line_num - 1; i> -1; i--){
        fprintf(fOut, "%s", lines[i]);
    }

    if (fIn != stdin && fIn) fclose(fIn);
    if (fOut != stdout && fOut) fclose(fOut);
    return 0;
}

