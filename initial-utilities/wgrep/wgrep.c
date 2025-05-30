#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

bool contains(char* buffer, ssize_t length, char* matchString, ssize_t matchStringLength)
{
  size_t currentMatchIndex = 0;
  size_t currentLineIndex = 0;
  while (currentMatchIndex < matchStringLength && currentLineIndex < length)
    {
      char currentChar = matchString[currentMatchIndex];
      if (buffer[currentLineIndex] == currentChar) {
	currentLineIndex++;
	currentMatchIndex++;
      }
      else {
	currentLineIndex++;
	currentMatchIndex = 0;
      }
    }
  return (currentMatchIndex == matchStringLength);
}

void printMatchingOccurences(FILE* fp, char* matchString)
{
  char* lineptr = NULL;
  size_t n = 0;
  while ((n = getline(&lineptr, &n, fp)) != -1) {
    size_t matchStringLength = 0;
    while (matchString[matchStringLength] != '\0')
      matchStringLength++;
    if (contains(lineptr, n, matchString, matchStringLength)) {
      printf("%s", lineptr);
    }
  }
  free(lineptr);
}

void printMatchingOccurencesFilename(char* filename, char* matchString)
{
  FILE* fp = fopen(filename, "r");
  if (fp == NULL) {
    printf("wgrep: cannot open file\n");
    exit(1);
  }
  printMatchingOccurences(fp, matchString);
}


int main(int argc, char** argv)
{
  if (argc == 1) {
    printf("wgrep: searchterm [file ...]\n");
    exit(1);
  }
  char* searchTerm = argv[1];
  if (argc == 2) {
    printMatchingOccurences(stdin, searchTerm);
  }
  else {
    for (int i = 2; i < argc; i++) {
      printMatchingOccurencesFilename(argv[i], searchTerm);
    }
  }
  exit(0);
}
