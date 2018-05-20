#include linked.h
#include <linux/list.h>
#include <errno.h>



void addfirst(listnode_t **head, int val){
  listnode_t *new_head;
  new_head =  malloc(sizeof(listnode_t));


  new_head->val = val;
  new_head->next = *head;
  *head = new_head;

}

void removelast(heap_data *head){
  if(head->next == NULL){
    return;
  }
  heap_data *current = head;
  while(current->next->next != NULL){
    current = current->next;

  }

  free(current->next);
  current->next = NULL;

}

void removefirst(listnode_t **head){
  listnode_t *new_head = NULL;

  if(*head = NULL)
  perror("list does not exist");
  }
  new_head = (*head)->next;
  free(*head);
  *head = new_head;

}


