#include "shell_config.h"
#include "shell_parse.h"
#include "shell_readline.h"
#include "shell_io.h"
#include "shell_command.h"

void mini_shell_task(void *params)
{
  static char *argv[CONFIG_SHELL_CMD_MAX_ARGC + 1];
  char *line;
  // 参数个数
  int argc;
  shell_puts(
      "\r\n"
      "  __  __ _       _ ____  _          _ _ \r\n"
      " |  \\/  (_)_ __ (_) ___|| |__   ___| | |\r\n"
      " | |\\/| | | '_ \\| \\___ \\| '_ \\ / _ \\ | |\r\n"
      " | |  | | | | | | |___) | | | |  __/ | |\r\n"
      " |_|  |_|_|_| |_|_|____/|_| |_|\\___|_|_|\r\n"
      "\r\n"
      "Welcome to MiniShell " MINI_SHELL_BUILD_VERDION "\r\n");
  while (1)
  {
    line = shell_readline();
    argc = shell_parse_line(line, argv, CONFIG_SHELL_CMD_MAX_ARGC + 1);
    if (argc > CONFIG_SHELL_CMD_MAX_ARGC)
    {
      argc = CONFIG_SHELL_CMD_MAX_ARGC;
    }
    if (argc > 0)
    {
      shell_run_cmd(argc, argv);
    }
  }
}