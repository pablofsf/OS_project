#include "buddy_list.h"
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#define MIN_ALLOC ((size_t)1 << 4)
#define MAX_ALLOC ((size_t)1 << 16)
#define DATA_SIZES 13;

static heap_data* freelist[DATA_SIZES];
heap_data* head = NULL;

void init(){
void* startmem = sbrk(MAX_ALLOC+sizeof(heap_data));
if(startmem = (void*) -1){
  perror(errno);
}

head->addr = startmem;
head->size = 0;
head->next = NULL;
head->prev = NULL;
head->data = NULL;
head->available = 1;

freelist[12] = head;

}

void *malloc(size_t size){
  if(head == NULL){
    init();
  }
  if(size > MAX_ALLOC || size < MIN_ALLOC){
    return NULL;
  }
  for(int i = 12; i >= 0; i--){
    if(size >= (1 << i+3)){
      break;
      }
    }
    heap_data* temphead = freelist[i];
    if(temphead != NULL){
      while(temphead->next != NULL && !(temphead->available)){
      temphead = temphead->next;
      }

    }

  }

}
