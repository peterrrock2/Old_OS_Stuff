#include "stack.h"

Stack *init_Stk(int size)
{
    Stack *stk = (Stack*) malloc(sizeof(struct Stack));
    stk->maxHeight = size;
    stk->height = 0;
    stk->finishedAdding = 0;
    stk->finishedFiles = 0;
    stk->nQuest = 0;
    stk->nqDone = 0;
    stk->nameArray = (char**)malloc(stk->maxHeight * sizeof(char*));
    for(int i = 0; i < size; i++)
        stk->nameArray[i] = (char *) malloc(255 * sizeof(char));
    return stk;
}
int testStkFull(Stack *stack)
{
    return stack->height == stack->maxHeight;
}
int testStkEmpty(Stack *stack)
{
    return stack->height == 0;
}
int pushStk(Stack *stack, char* newName)
{
    if(testStkFull(stack)) return -1;
    strcpy(stack->nameArray[stack->height], newName);
    stack->height += 1;
    return 0;
}
char *popStk(Stack *stack)
{
    if(testStkEmpty(stack)) return "Not Valid";
    stack->height -= 1;
    char *rName = stack->nameArray[stack->height];
    return rName;
}
void destroyStk(Stack *stack)
{   
    for (int i = 0; i < stack->maxHeight; i++)
        free(stack->nameArray[i]);
    free(stack->nameArray);
    free(stack);
    return;
}

void printStk(Stack *stack)
{
    for(int i = 0; i < stack->height; i++)
    {
        printf("Stack position %d contains %s\n", i, stack->nameArray[i]);
    }
}

