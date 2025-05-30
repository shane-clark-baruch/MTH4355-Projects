#include <stdlib.h>
#include <stdio.h>

void writeOut(char* filename)
{
  FILE* fp = fopen(filename, "r");
  if (fp == NULL) {
    printf("cannot open file with name %s\n", filename);
    exit(1);
  }
  char buffer[5];
  while (fread(buffer, 1, 5, fp) == 5) {
    int length = *(int*)buffer; // first 4 bytes in buffer;
    char character = buffer[4];
    for (int i = 0; i < length; i++) {
      printf("%c", character);
    }
  }
}

int main(int argc, char* argv[])
{
  if (argc == 1) {
    printf("wunzip: file1 [file2 ...]\n");
    exit(1);
  }
  for (int i = 1; i < argc; i++) {
    char* filename = argv[i];
    writeOut(filename);
  }
}
