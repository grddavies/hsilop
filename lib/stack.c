#include <stdio.h>
#include <stdlib.h>

#include "stack.h"

#define MAX_STACK_SIZE 100

int stack_empty(Stack *stack) { return stack->top == 0; }

void stack_push(Stack *stack, double value) {
  if (stack->top >= MAX_STACK_SIZE) {
    fprintf(stderr, "Stack overflow\n");
    exit(EXIT_FAILURE);
  }
  stack->data[stack->top++] = value;
}

double stack_pop(Stack *stack) {
  if (stack_empty(stack)) {
    fprintf(stderr, "Stack underflow\n");
    exit(EXIT_FAILURE);
  }
  return stack->data[--stack->top];
}

void stack_peek(Stack *stack) {
  if (stack_empty(stack)) {
    fprintf(stderr, "(stack empty)\n");
  } else {
    printf("%g\n", stack->data[stack->top - 1]);
  }
}

Stack *stack_new() {
  Stack *stack = malloc(sizeof(Stack));
  stack->top = 0;
  return stack;
}
