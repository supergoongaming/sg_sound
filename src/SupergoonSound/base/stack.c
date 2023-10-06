#include <stdlib.h>
#include <stdio.h>
#include "stack.h"

Stack *CreateStack(int capacity)
{
    Stack *stack = malloc(sizeof(*stack));
    stack->data = calloc(capacity, sizeof(int));
    stack->capacity = capacity;
    stack->size = 0;
    return stack;
}

int PopStack(Stack *stack)
{
    if (stack->size == 0)
    {
        puts("Stack is currently empty");
        return 0;
    }
    return stack->data[--stack->size];
}
void PushStack(Stack *stack, int item)
{
    if (stack->size + 1 > stack->capacity)
    {
        return;
    }
    stack->data[stack->size++] = item;
}

void DestroyStack(Stack *stack)
{
    free(stack->data);
    free(stack);
    stack = NULL;
}
