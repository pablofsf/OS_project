#include linked.h
#include <linux/list.h>
#include <errno.h>

void addlast(listnode_t *head, int val){
  listnode_t *current = head;
    while(current->next != NULL){
      current = current-> next;
    }

  current->next = malloc(sizeof(listnode_t));
  current->next->val = val;
  current->next->next = NULL;

}

void addfirst(listnode_t **head, int val){
  listnode_t *new_head;
  new_head = malloc(sizeof(listnode_t));


  new_head->val = val;
  new_head->next = *head;
  *head = new_head;

}

void removelast(listnode_t *head){
  if(head->next == NULL){
    free(head);
  }
  listnode_t *current = head;
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

int remove(listnode_t **head, int val){
  int i = 0;
  listnode_t *current = *head;
  listnode_t *temp = NULL;

  if(val == 0){
    return 0;
  }

  for(i = 0; i < val-1; i++){
    if(current->next == NULL){
      return 0;
    }

    current = current->next;

  }
  temp = current->next;
  current->next = temp->next;
  free(temp);
  return 0;
}
