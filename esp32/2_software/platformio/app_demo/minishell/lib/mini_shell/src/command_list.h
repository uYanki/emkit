#ifndef __COMMAND_LIST_H__
#define __COMMAND_LIST_H__

#include <stdbool.h>
#include <stdlib.h>

#include "shell_command.h"
struct Node {
  shell_command data;  //数据域
  struct Node *next;   //指针域
};
struct Node *createList();
struct Node *createNode(shell_command cmd);
void insertNodeByHead(struct Node *headNode, shell_command cmd);
struct Node *searchCmdNode(struct Node *headNode, char *name);
#endif  // !__COMMAND_LIST_H__