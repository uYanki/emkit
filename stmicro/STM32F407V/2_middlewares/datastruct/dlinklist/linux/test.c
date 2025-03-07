#include "dlist.h"
#include "stdio.h"

LIST_HEAD(test);

void main()
{
	struct list_head list[5];
	int i;
	
	printf("head address:0x%x prev:0x%x next:0x%x\r\n", &test, test.prev,test.next);
	
	for(i=0;i<5;i++)
	{
		printf("list[%d] address:0x%x\r\n",i, (unsigned int)(&list[i]));
		INIT_LIST_HEAD(&list[i]);
		list_add_tail(&list[i],&test);
	}

	struct list_head *lst;
	list_for_each(lst, &test)
	{
		printf("current:0x%x prev:0x%x next:0x%x\r\n", lst, lst->prev, lst->next);
	}
	printf("\r\n");
	list_move_tail(&list[1],&test);

	list_for_each(lst, &test)
	{
		printf("current:0x%x prev:0x%x next:0x%x\r\n", lst, lst->prev, lst->next);
	}


	printf("\r\n");
	list_del(&list[1]);

	list_for_each(lst, &test)
	{
		printf("current:0x%x prev:0x%x next:0x%x\r\n", lst, lst->prev, lst->next);
	}

}
