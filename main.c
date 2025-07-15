/**
 * Reverse polish notation calculator
 */

#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include "lib/interpret.h"
#include "lib/stack.h"

int main() {
  char line[256];
  Stack *stack = stack_new();

  bool interactive = isatty(STDIN_FILENO);
  interpret(line, stack, interactive);

  free(stack);
  return EXIT_SUCCESS;
}
