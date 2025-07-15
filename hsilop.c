/**
 * Reverse polish notation calculator
 */

#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_STACK_SIZE 100

typedef struct {
  double data[MAX_STACK_SIZE];
  int top;
} Stack;

static int stack_empty(Stack *stack) { return stack->top == 0; }

static void stack_push(Stack *stack, double value) {
  if (stack->top >= MAX_STACK_SIZE) {
    fprintf(stderr, "Stack overflow\n");
    exit(EXIT_FAILURE);
  }
  stack->data[stack->top++] = value;
}

static double stack_pop(Stack *stack) {
  if (stack_empty(stack)) {
    fprintf(stderr, "Stack underflow\n");
    exit(EXIT_FAILURE);
  }
  return stack->data[--stack->top];
}

static void op_add(Stack *stack) {
  double y = stack_pop(stack);
  double x = stack_pop(stack);
  stack_push(stack, x + y);
}

static void op_sub(Stack *stack) {
  double y = stack_pop(stack);
  double x = stack_pop(stack);
  stack_push(stack, x - y);
}

static void op_div(Stack *stack) {
  double y = stack_pop(stack);
  double x = stack_pop(stack);
  stack_push(stack, x / y);
}

static void op_mul(Stack *stack) {
  double y = stack_pop(stack);
  double x = stack_pop(stack);
  stack_push(stack, x * y);
}

static int try_parse_double(char *token, double *ptr) {
  char *p = token;
  *ptr = strtod(token, &p);
  if (p == token) {
    return -1;
  }

  if (*p != '\0') {
    return 1;
  }

  return 0;
}

static void exec(char *token, Stack *stack) {
  double x = 0;
  switch (try_parse_double(token, &x)) {
  case 0: {
    // Success
    return stack_push(stack, x);
  }
  case 1: {
    // Unexpected number
    fprintf(stderr, "Invalid number: %s\n", token);
    return;
  }
  default: {
    // non-number-like
    break;
  }
  }

  if (strlen(token) != 1) {
    fprintf(stderr, "Invalid operator: %s\n", token);
    return;
  }

  switch (*token) {
  case '+':
    op_add(stack);
    break;
  case '-':
    op_sub(stack);
    break;
  case '/':
    op_div(stack);
    break;
  case '*':
    op_mul(stack);
    break;
  default:
    fprintf(stderr, "Invalid operator: %s\n", token);
  }
}

static void tokenize_exec(char *line, Stack *stack) {
  // mutates line!
  char *token = strtok(line, " ");
  while (token != NULL) {
    exec(token, stack);
    token = strtok(NULL, " ");
  }

  if (stack_empty(stack)) {
    fprintf(stderr, "No result (stack empty)\n");
  } else {
    printf("%g\n", stack->data[stack->top - 1]);
  }
}

static void interactive_mode(char *line, Stack *stack) {
  char *prompt = "> ";
  printf("RPN Calculator (type 'quit' to exit)\n");
  fputs(prompt, stdout);

  while (fgets(line, sizeof(line), stdin)) {
    // Remove newline
    line[strcspn(line, "\n")] = 0;

    if (strcmp(line, "quit") == 0) {
      break;
    }
    tokenize_exec(line, stack);
    fputs(prompt, stdout);
  }
}

int main() {
  char line[256];
  Stack stack = {.data = {0}, .top = 0};

  if (isatty(STDIN_FILENO)) {
    // Interactive mode
    interactive_mode(line, &stack);
  } else {
    // Reading from pipe/file
    while (fgets(line, sizeof(line), stdin)) {
      // Remove newline
      line[strcspn(line, "\n")] = 0;
      tokenize_exec(line, &stack);
    }
  }

  return EXIT_SUCCESS;
}
