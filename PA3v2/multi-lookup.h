#ifndef MULTI_LOOKUP_H
#define MULTI_LOOKUP_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#include "util.h"
#include "stack.h"


#define ARRAY_SIZE 10
#define MAX_INPUT_FILES 100
#define MAX_REQUESTER_THREADS 10
#define MAX_RESOLVER_THREADS 10
#define MAX_NAME_LENGTH 255
#define MAX_IP_LENGTH INET6_ADDRSTRLEN

void printRuntime(char *progName, clock_t startTime, clock_t stopTime);
int check_args(int argc, char *argv[]);
void *request(void *qData);
void *resolve(void *sData);

// char** init_circArray()
// {
//     char** sharedArray = malloc(ARRAY_SIZE * sizeof(char*));
// }


// Using idea from recitation here to define shared memory strucutres
// see Mar 26 stuff.
typedef struct questData_s
{
    Stack *stk;
    Stack *fileStk;
    FILE  *requestLogfp;
} questData;

typedef struct solveData_s
{
    Stack *stk;
    FILE  *resolvefp;
} solveData;

#endif