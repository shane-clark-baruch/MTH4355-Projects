#include <stdlib.h>
#include <stdio.h>

void writeInt(int target)
{
  fwrite(&target, 1, 4, stdout);
}

void writeChar(char target)
{
  printf("%c", target);
}

void writePair(int targetNum, char targetChar)
{
  writeInt(targetNum);
  writeChar(targetChar);
}

void runLengthEncode(FILE* fp, int* nextChar, int* lastChar, int* currentCount)
{
  if (*nextChar == 0) {
    // Seed values for first file
    *nextChar = fgetc(fp);
    *lastChar = *nextChar;
    *currentCount = 1;
  }
  while ((*nextChar = fgetc(fp)) != EOF) {
    if (*nextChar == *lastChar) {
      (*currentCount)++;
    }
    else {
      writePair(*currentCount, (char) *lastChar);
      *currentCount = 1;
    }
    *lastChar = *nextChar;
  }
}


void processFile(char* filename, int* nextChar, int* lastChar, int* currentCount)
{
  FILE* fp = fopen(filename, "r");
  if (fp == NULL) {
    printf("cannot open file with name %s\n", filename);
    exit(1);
  }
  runLengthEncode(fp, nextChar, lastChar, currentCount);
}


int main(int argc, char** argv)
{
  if (argc == 1) {
    printf("wzip: file1 [file2 ...]\n");
    exit(1);
  }
  int lastChar = 0;
  int nextChar = 0;
  int currentCount = 0;
  for (int i = 1; i < argc; i++) {
    char* filename = argv[i];
    processFile(filename, &nextChar, &lastChar, &currentCount);
  }
  // Write leftover character
  writePair(currentCount, (char) lastChar);
}
