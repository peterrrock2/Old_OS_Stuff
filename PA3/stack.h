#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef STACK_H
#define STACK_H


typedef struct Stack
{
    char **nameArray;
    int height;
    int maxHeight;
    int finishedAdding;
    int finishedFiles;
    int nQuest;
    int nqDone;
} Stack;

Stack *init_Stk(int size);
int testStkFull(Stack *stack);
int testStkEmpty(Stack *stack);
int pushStk(Stack *stack, char *newName);
char *popStk(Stack *stack);
void destroyStk(Stack *stack);
void printStk(Stack *stack);

#endif

// int init_Stk(Stack *stack, int size);
// int testStkFull(Stack *stack);
// int testStkEmpty(Stack *stack);
// int pushStk(Stack *stack, char *newName);
// char *popStk(Stack *stack);
// void destroyStk(Stack *stack);
// void printStk(Stack *stack);