#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char ** readDict(char * file, int * dictLen);
int checkDict(char ** dict, char * word, int dictLen);
void printDict(char ** dict, int dictLen);

// reads dictionary to array
char ** readDict(char * file, int * dictLen){
  // open dictionary
  FILE * dictFile = fopen(file, "r");

  // check for success
  if(dictFile == NULL){
    printf("ERROR: failed to open dictionary [%s]\n", file);
    exit(EXIT_FAILURE);
  }

  // lines in dictionary
  int lines = 0;

  // max size of buffer
  int BUFFSIZE = 50;

  // buffer for file
  char buffer[BUFFSIZE];

  // counts file lines
  while(fgets(buffer, BUFFSIZE, dictFile) != NULL){
    lines++;
  }

  // resets file head
  rewind(dictFile);

  // list of words in dictionary
  char ** dict;

  // stores len of dict
  *dictLen = lines;

  if(lines < 0){
    printf("ERROR: Dictionary length less than 0.\n");
    exit(EXIT_FAILURE);
  }
  
  // mallocs size of dict
  dict = (char**)malloc(sizeof(char*) * lines);

  // checks for success
  if(dict == NULL){
    printf("ERROR: Failed to malloc dictionary.\n");
    exit(EXIT_FAILURE);
  }

  // stores lines of dictionary
  int i = 0;
  while(fgets(buffer, BUFFSIZE, dictFile) != NULL){
    // mallocs space for word
    dict[i] = (char*)malloc(sizeof(char)*BUFFSIZE);

    // copies word into array
    strcpy(dict[i], buffer);

    // finds newline
    int j = 0;
    while(dict[i][j] != '\n'){
      // converts every letter to uppercase
      dict[i][j] = toupper(dict[i][j]);

      j++;
    }

    // removes newline
    dict[i][j] = '\0';
    
    i++;
  }

  return dict;
}

// prints entire dictionary
void printDict(char ** dict, int dictLen){
  int i = 0;
  for(i = 0; i < dictLen; ++i){
    printf("%s\n", dict[i]);
  }
}

// checks if word is within dictionary, returns 1 if it is, 0 if not
int checkDict(char ** dict, char * word, int dictLen){
  int i = 0;

  // converts word to upper case
  while(word[i] != '\0'){
    word[i] = toupper(word[i]);
    i++;
  }
  
  for(i = 0; i < dictLen; ++i){
    if(strcmp(dict[i], word) == 0){
      // word is in dict
      return 1;
    }
  }

  // word is not in dict
  return 0;
}
