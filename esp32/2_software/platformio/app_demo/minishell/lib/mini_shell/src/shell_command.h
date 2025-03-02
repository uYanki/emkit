#ifndef MINI_SHELL_COMMAND_H
#define MINI_SHELL_COMMAND_H

struct _shell_cmd;
typedef int (*shell_command_callback)(const struct _shell_cmd *pCmdt,
                                      int argc, char *const argv[]);

typedef struct _shell_cmd
{
  const char *name;
  const char *help;
  shell_command_callback callback;
} shell_command;

int shell_run_cmd(int argc, char *const argv[]);

void shell_add_cmd(shell_command cmd);

#endif