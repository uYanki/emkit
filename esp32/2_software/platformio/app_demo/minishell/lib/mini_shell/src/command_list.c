#include "command_list.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shell_command.h"
#include "shell_io.h"

struct Node *createList() {
  struct Node *headNode = (struct Node *)malloc(sizeof(struct Node));
  headNode->next = NULL;
  return headNode;
}
//创建结点
struct Node *createNode(shell_command cmd) {
  struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
  newNode->data = cmd;
  newNode->next = NULL;
  return newNode;
}

void insertNodeByHead(struct Node *headNode, shell_command cmd) {
  //创建插入的结点
  struct Node *newNode = createNode(cmd);
  newNode->next = headNode->next;
  headNode->next = newNode;
}

struct Node *searchCmdNode(struct Node *headNode, char *name) {
  struct Node *posNode = headNode->next;
  struct Node *posNodeFront = headNode;
  if (posNode == NULL) {
    shell_printf("命令链表为空\r\n");
  }
  while (strcmp(posNode->data.name, name) != 0) {
    posNodeFront = posNode;
    posNode = posNodeFront->next;
    if (posNode == NULL) {
      break;
    }
  }
  return posNode;
}
