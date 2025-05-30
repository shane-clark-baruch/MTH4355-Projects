#include <stdio.h>
#include <stdlib.h>
#define OUTPUT_BUFFER_SIZE 16384

void printFile(char* filename)
{
  FILE* fp = fopen(filename, "r");
  char outputBuffer[OUTPUT_BUFFER_SIZE];
  if (fp == NULL) {
    printf("wcat: cannot open file\n");
    exit(1);
  }
  while (fgets(outputBuffer, 16384, fp) != NULL)
    printf("%s", outputBuffer);
  fclose(fp);
}


int main(int argc, char** argv)
{
  for (int i = 1; i < argc; i++) {
    printFile(argv[i]);
  }
  exit(0);
}
