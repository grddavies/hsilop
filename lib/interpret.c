#include <ctype.h>
#include <stdbool.h>

#include <readline/history.h>
#include <readline/readline.h>
#include <stdlib.h>
#include <string.h>

#include "operators.h"
#include "stack.h"

bool DONE = false;

typedef struct {
  char *name;     /* User printable name of the function. */
  Function *func; /* Function to call to do the job. */
  char *doc;      /* Documentation for this function.  */
} COMMAND;

static int com_quit(const char *arg, int _) {
  DONE = 1;
  return (0);
}

COMMAND commands[] = {
    {":quit", com_quit, "Quit REPL"},
};

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

static bool is_whitespace(char c) { return isspace(c); }

/**
 * @brief Finds a command by name.
 *
 * Searches through the commands array to find a command with the specified
 * name.
 *
 * @param name The name of the command to search for
 * @return Pointer to the COMMAND structure if found, NULL otherwise
 */
static COMMAND *find_command(char *name) {
  register int i;

  for (i = 0; commands[i].name; i++)
    if (strcmp(name, commands[i].name) == 0)
      return (&commands[i]);

  return ((COMMAND *)NULL);
}

static int exec_interactive_cmd(char *line) {
  int i;
  COMMAND *command;
  char *word;

  /* Isolate the command word. */
  i = 0;
  while (line[i] && is_whitespace(line[i]))
    i++;
  word = line + i;

  while (line[i] && !is_whitespace(line[i]))
    i++;

  if (line[i])
    line[i++] = '\0';

  command = find_command(word);

  if (!command) {
    fprintf(stderr, "Command not found: '%s'.\n", word);
    return (-1);
  }

  /* Get argument to command, if any. */
  while (is_whitespace(line[i])) {
    i++;
  }

  word = line + i;

  /* Call the function. */
  return ((*(command->func))(word, 0));
}

static void exec(char *token, Stack *stack) {
  double x = 0;
  switch (try_parse_double(token, &x)) {
  case 0: {
    // Success
    stack_push(stack, x);
    return;
  }
  case 1: {
    // Unexpected number
    fprintf(stderr, "Invalid number: %s\n", token);
    return;
  }
  default: {
    // not number-like: parse as operator
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

static void exec_line(char *line, Stack *stack) {
  char *token = strtok(line, " ");
  while (token != NULL) {
    exec(token, stack);
    token = strtok(NULL, " ");
  }
}

void interpret(char *line, Stack *stack, bool interactive) {
  if (interactive) {
    printf("RPN Calculator (type ':quit' to exit)\n");
  }
  char *prompt = "> ";
  while (!DONE && (line = readline(prompt))) {
    if (interactive && *line == ':') {
      exec_interactive_cmd(line);
      continue;
    }

    exec_line(line, stack);

    // Auto print the top of the stack in interactive mode
    if (interactive) {
      stack_peek(stack);
    }
    add_history(line);
  }

  if (!interactive) {
    // If we aren't interactive we print the top of the stack at the end
    stack_peek(stack);
  }
}
