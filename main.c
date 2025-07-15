/**
 * Reverse polish notation calculator
 */

#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include "lib/interpret.h"
#include "lib/stack.h"

size_t MAX_LINE_LENGTH = 256;

int main() {
  char line[MAX_LINE_LENGTH];
  Stack *stack = stack_new();

  bool interactive = isatty(STDIN_FILENO);
  interpret(line, stack, interactive);

  free(stack);
  return EXIT_SUCCESS;
}
