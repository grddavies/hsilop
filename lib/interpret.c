#include <ctype.h>
#include <stdbool.h>

#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "operators.h"
#include "stack.h"

/**
 * BUILTINS
 */

typedef int (*BuiltinFunction)(Stack *stack);

typedef struct {
  char *name;
  BuiltinFunction func;
  char *doc;
} BUILTIN;

static int builtin_peek(Stack *stack) {
  stack_peek(stack);
  return 0;
}

static int builtin_pop(Stack *stack) {
  double value = stack_pop(stack);
  printf("%.6g\n", value);
  return 0;
}

BUILTIN builtins[] = {
    {"peek", builtin_peek,
     "Display the top value on the stack without removing it"},
    {"pop", builtin_pop, "Remove and display the top value from the stack"},
    {NULL, NULL, NULL} /* end marker */
};

/**
 * @brief Finds a builtin by name.
 *
 * Searches through the builtins array to find a builtin with the specified
 * name.
 *
 * @param name The name of the builtin to search for
 * @return Pointer to the BUILTIN structure if found, NULL otherwise
 */
static BUILTIN *find_builtin(char *name) {
  register int i;

  for (i = 0; builtins[i].name; i++)
    if (strcmp(name, builtins[i].name) == 0 && builtins[i].func != NULL) {
      return &builtins[i];
    }

  return (BUILTIN *)NULL;
}

/**
 * @brief Generator function for builtin completion.
 * @param text The text to complete
 * @param state Lets us know whether to start from scratch; without any state
 * (i.e. STATE == 0), then we start at the top of the list
 * @return The next name which partially matches from the builtin list, or NULL
 * if no names matched
 */
char *builtin_generator(const char *text, int state) {
  static int list_index, len;
  char *name;

  if (!state) {
    list_index = 0;
    len = strlen(text);
  }

  while ((name = builtins[list_index].name)) {
    list_index++;

    if (strncmp(name, text, len) == 0) {
      return (strdup(name));
    }
  }

  return (char *)NULL;
}

/**
 * COMMANDS
 */

/**
 * If true, REPL terminates
 */
bool DONE = false;

typedef struct {
  char *name;     /* User printable name of the function. */
  Function *func; /* Function to call to do the job. */
  char *doc;      /* Documentation for this function.  */
} COMMAND;

COMMAND commands[];

static int com_quit(const char *arg, int _) {
  DONE = 1;
  return (0);
}

static int com_help(const char *arg, int _) {
  register int i;

  printf("Available commands:\n");
  printf("\nREPL Commands:\n");
  for (i = 0; commands[i].name; i++) {
    printf("  %-10s %s\n", commands[i].name, commands[i].doc);
  }

  printf("\nCalculator Commands:\n");
  printf("  %-10s %s\n", "<number>", "Push a number onto the stack");
  for (i = 0; builtins[i].name; i++) {
    printf("  %-10s %s\n", builtins[i].name, builtins[i].doc);
  }

  return (0);
}

COMMAND commands[] = {{":quit", com_quit, "Quit REPL"},
                      {":help", com_help, "Show available REPL commands"},
                      {NULL, NULL, NULL} /* end marker */};

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
    if (strcmp(name, commands[i].name) == 0) {
      return &commands[i];
    }

  return (COMMAND *)NULL;
}

/**
 * @brief Generator function for REPL command completion.
 * @param text The text to complete
 * @param state Lets us know whether to start from scratch; without any state
 * (i.e. STATE == 0), then we start at the top of the list
 * @return The next name which partially matches from the command list, or NULL
 * if no names matched
 */
char *command_generator(const char *text, int state) {
  static int list_index, len;
  char *name;

  /* If this is a new word to complete, initialize now.  This includes
     saving the length of TEXT for efficiency, and initializing the index
     variable to 0. */
  if (!state) {
    list_index = 0;
    len = strlen(text);
  }

  /* Return the next name which partially matches from the command list. */
  while ((name = commands[list_index].name)) {
    list_index++;

    if (strncmp(name, text, len) == 0) {
      return (strdup(name));
    }
  }

  /* If no names matched, then return NULL. */
  return (char *)NULL;
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

static bool is_whitespace(char c) { return isspace(c); }

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

  // Note integer second arg is not used
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

  BUILTIN *builtin = find_builtin(token);
  if (builtin) {
    builtin->func(stack);
    return;
  }

  if (strlen(token) == 1) {
    switch (*token) {
    case '+':
      op_add(stack);
      return;
    case '-':
      op_sub(stack);
      return;
    case '/':
      op_div(stack);
      return;
    case '*':
      op_mul(stack);
      return;
    }
  }

  fprintf(stderr, "Invalid operator: %s\n", token);
}

static void exec_line(char *line, Stack *stack) {
  char *token = strtok(line, " ");
  while (token != NULL) {
    exec(token, stack);
    token = strtok(NULL, " ");
  }
}

char **hsilop_completion(const char *text, int start, int end) {
  char **matches;

  matches = (char **)NULL;

  // if word is at start of the line it is a command completion
  if (start == 0 && text[0] == ':') {
    matches = rl_completion_matches(text, command_generator);
  } else {
    matches = rl_completion_matches(text, builtin_generator);
  }

  // We don't want default filename completion if not a command
  rl_attempted_completion_over = 1;

  return (matches);
}

static void initialize_readline() {
  rl_readline_name = "hsilop";
  rl_attempted_completion_function = hsilop_completion;
}

void interpret(char *line, Stack *stack, bool interactive) {
  initialize_readline();
  if (interactive) {
    printf("RPN Calculator (type ':quit' to exit, ':help' for info)\n");
  }
  char *prompt = "> ";
  while (!DONE && (line = readline(prompt))) {
    if (interactive && line[0] == ':') {
      exec_interactive_cmd(line);
      add_history(line);
      continue;
    } else {
      exec_line(line, stack);
      // Auto-print the top of the stack in interactive mode
      if (interactive) {
        if (!stack_empty(stack)) {
          fputs("=> ", stdout);
        }
        stack_peek(stack);
      }
    }

    add_history(line);
    free(line);
  }

  if (!interactive) {
    // If we aren't interactive we print the top of the stack at the end
    stack_peek(stack);
  }
}
