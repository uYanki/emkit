#include "shell_command.h"

#include "command_list.h"
#include "shell_io.h"
static struct Node *cmd_list;

int cmd_match(void *a, void *b) {
  char *name = a;
  shell_command *cmd2 = b;
  return name != cmd2->name;
}

int shell_run_cmd(int argc, char *const argv[]) {
  if (cmd_list == NULL) {
    cmd_list = createList();
    return -1;
  }

  struct Node *node = searchCmdNode(cmd_list, argv[0]);
  if (node == NULL) {
    shell_printf("  %s: command not found.\r\n", argv[0]);
    return -1;
  }
  const shell_command cmd = node->data;
  return cmd.callback(&cmd, argc, argv);
  shell_puts("cmd null");
  shell_printf("  %s: command not found.\r\n", argv[0]);

  return -1;
}

void shell_add_cmd(shell_command cmd) {
  if (cmd_list == NULL) {
    cmd_list = createList();
  }
  insertNodeByHead(cmd_list, cmd);
}