//
// Created by Xiaotian on 2021/3/16.
//
#include <stdlib.h>
#include "readFile.h"

/*  char* cutString(char* input,int start,int end) ->
 *  input source string, output the substring in index interval
 *  [start, end) from source string.
 */
char* CutString(char* input, int start,int end) {

    int i = 0, j = 0;
    int size = end - start;
    char* output = NULL;

    output = (char*) calloc((size + 1), sizeof(char));
    if (output == NULL) exit(1);
    for(i = start; i < end; i++) {
        output[j++] = input[i];
    }
    output[j] = '\0';

    return output;
}

/*
 *  int extractIntNumber(char* buffer, int* start, int* end) ->
 *  input a line of record, index interval, and output the number
 *  converted from string.
 */
unsigned int ExtractIntNumber(char* buffer, int* start, int* end) {

    int i = 0;
    unsigned int number = 0;
    char* catch = NULL;
    *start = *end + 1;
    *end = *start;

    for(i = *start; buffer[i] != ' '; i++) {
        *end = *end + 1;
    }
    catch = CutString(buffer, *start, *end);

    /* convert string to integer number */
    number = (unsigned int)strtoul(catch, NULL, 10);
    free(catch);

    return  number;
}

int ExtractPN(char* buffer, int* start, int* end) {

    int i = 0, number = 0;
    char *catch = NULL;
    *start = *end + 1;
    *end = *start;

    for (i = *start; buffer[i] != '\n'; i++) {
        *end = *end + 1;
    }
    catch = CutString(buffer, *start, *end);

    if (catch[0] == 'p') {
        number = 1;
    }

    free(catch);
    return number;
}