#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFER_SIZE 512

int main(int argc, char *argv[])
{
    if (argc == 1)
        return 0;

    for (int fileNumber = 1; fileNumber < argc; fileNumber++)
    {
        FILE *file = fopen(argv[fileNumber], "r");

        if (file == NULL)
        {
            printf("wcat: cannot open file\n");
            exit(1);
        }

        char lines[BUFFER_SIZE];

        while (fgets(lines, BUFFER_SIZE, file) != NULL)
            printf("%s", lines);

        fclose(file);
    }

    return 0;
}
