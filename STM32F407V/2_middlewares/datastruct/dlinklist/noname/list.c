#include "stdio.h"
#include "list.h"

#if 0 // freertos
#define malloc pvPortMalloc
#define free   vPortFree
#endif

/*
*brief: create a new list node
*input: void
*return: new list addr
*/
list_t *list_new(void)
{
  list_t *new_list = NULL;
  
  new_list = malloc(sizeof(list_t));
  if(new_list != NULL)
  {
    new_list->prev    = NULL;
    new_list->next    = NULL;
    new_list->obj     = NULL;
    //printf("new_list addr = %d\n", (unsigned int)new_list);
  }
  return new_list;
}

/*
*brief: insert list node
*input: list_head: head list addr,
*       node     : node list addr
*return: head list addr
*/
list_t *list_insert_node(list_t **list_head, list_t *new_list)
{
  if(new_list == NULL)
    return *list_head;
  
  if(*list_head == NULL)
    *list_head =  new_list;
  else
  {
    new_list->next = *list_head;
    (*list_head)->prev = new_list;
    *list_head      = new_list;
    
  }
  return *list_head;
}

/*
*brief: delete list node
*input: list_head: head list addr,
*       node     : node list addr
*return: list addr
*/
list_t *list_delete_node(list_t **list_head, list_t *node)
{
  if(*list_head == NULL || node == NULL)
    return *list_head;
  
  list_t * list = *list_head;
  //printf("del node:%d\n", (unsigned int)node);
  do
  {
    //printf("list:%d\n", (unsigned int)list);
    if(list == node)
    {
      //printf("find node:%d\n", (unsigned int)node);
      if(list->prev == NULL && list->next == NULL)
      {
        *list_head = NULL;
      }
      else if(list->prev == NULL)
      {
        //printf("this is the first node\n");
        list->next->prev = NULL;
        *list_head = list->next;
      }
      else if(list->next == NULL)
      {
        //printf("this is the end node\n");
        list->prev->next = NULL;
      }
      else 
      {
        list->prev->next = list->next;
        list->next->prev = list->prev;
      }
      free(node);
      break;
    }
    else 
    {
      list = list->next;
      //printf("not find node:%d\n", (unsigned int)node);
    }
    
  }while(list != NULL);
  
  return *list_head;
}

/*
*brief: search list node
*input: head list addr, obj addr
*return: list addr
*/
list_t * list_search_node(list_t *list_head, void *obj)
{
  list_t *list = list_head;
  int count = 0;
  
  if(list_head == NULL || obj == NULL) 
    return NULL;
    
  while(list != NULL)
  {
    count ++;
    if(list->obj == obj)
      break;
    list = list->next;  
  }
  
  return list;
}

/*
*brief: get the list size
*input: head list ptr
*return: size
*/
int list_size(list_t *list)
{
  int count = 0;
  
  while(list != NULL)
  {
    count ++;
    list = list->next;  
  }
  return count;
}

