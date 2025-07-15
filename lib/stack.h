#ifndef STACK_H
#define STACK_H

#define MAX_STACK_SIZE 100

typedef struct {
  double data[MAX_STACK_SIZE];
  int top;
} Stack;

Stack *stack_new();

int stack_empty(Stack *stack);

void stack_push(Stack *stack, double value);

double stack_pop(Stack *stack);

void stack_peek(Stack *stack);

#endif // !STACK_H
