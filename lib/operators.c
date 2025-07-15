#include "stack.h"

void op_add(Stack *stack) {
  double y = stack_pop(stack);
  double x = stack_pop(stack);
  stack_push(stack, x + y);
}

void op_sub(Stack *stack) {
  double y = stack_pop(stack);
  double x = stack_pop(stack);
  stack_push(stack, x - y);
}

void op_div(Stack *stack) {
  double y = stack_pop(stack);
  double x = stack_pop(stack);
  stack_push(stack, x / y);
}

void op_mul(Stack *stack) {
  double y = stack_pop(stack);
  double x = stack_pop(stack);
  stack_push(stack, x * y);
}
