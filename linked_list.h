#include <linux/list.h>

void addfirst(listnode_t **head, int val);
void addlast(listnode_t *head, int val);
void removefirst(listnode_t **head);
void removelast(listnode_t *head);
int remove(listnode_t *head, int val);


typedef struct listnode {
  int val;
  struct node *next;
} listnode_t;
