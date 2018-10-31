#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char ** readDict(char * file, int * dictLen);
int checkDict(char ** dict, char * word, int dictLen);
void printDict(char ** dict, int dictLen);

#endif /** DICTIONARY_H */
