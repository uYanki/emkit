#ifndef __OBJ_LIST_H__
#define __OBJ_LIST_H__

typedef struct _list
{
  void   *obj;
  void   *fd;
  struct _list *prev;
  struct _list *next;
}list_t;

list_t *list_new(void);
list_t *list_insert_node(list_t **list_head, list_t *new_list);
list_t *list_delete_node(list_t **list_head, list_t *node);
list_t * list_search_node(list_t *list_head, void *obj);
int list_size(list_t *list);

#endif