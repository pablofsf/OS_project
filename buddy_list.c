#include "buddy_list.h"
#include <errno.h>

#define MIN_ALLOC ((size_t)1 << 4)
#define MAX_ALLOC ((size_t)1 << 16)
#define DATA_SIZES 13
#define SIZE_HEAP_DATA ((void *) sizeof(struct heap_data))

static void merge(struct heap_data *h1,struct heap_data *h2,int index);
static void free_block(struct heap_data *h,int index);


static struct heap_data* freelist[DATA_SIZES];
struct heap_data* head = NULL;
static void *startmem;

static int get_index(size_t size)
{
	int i;
     	for(i = DATA_SIZES - 1; i >= 0; i--){
		if(size >= (1 << (i + 3))){
			break;
		}
	}
	return i;
}

static void add_node(struct heap_data *h,int index)
{
	struct heap_data *tmp = freelist[index];
	freelist[index] = h;
	h->prev = NULL;
	h->next = tmp;
}

static void remove_node(struct heap_data *h,int index)
{
	struct heap_data *current = freelist[index];
	struct heap_data *prev = NULL;

	if (current == NULL)
		return;

	if (current == h) {
		freelist[index] = h->next;
		return;
	}

	current = freelist[index]->next;
	prev = freelist[index];
	
	while (current) {
		if (current == h)
			break;
		prev = current;
		current = current->next;
	}

	prev->next = current->next;
}

void init(){
	startmem = sbrk(MAX_ALLOC+sizeof(struct heap_data));
	if(startmem == (void*) -1){
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
	int i;
	
	if(head == NULL){
		init();
	}
	if(size > MAX_ALLOC || size < MIN_ALLOC){
		return NULL;
	}
        i = get_index(size);
	
	struct heap_data *temphead = freelist[i];
	if(temphead != NULL){
		while(temphead->next != NULL && !(temphead->available)){
			temphead = temphead->next;
		}

	}

}

static void *contiguous(struct heap_data *h,size_t size)
{
	size_t addr = ((size_t) h) - (size_t) startmem;
	size_t base, offset;
	base = addr / (size << 1);
	offset = addr % (size << 1);

	if (offset == 0)
		return (void *) (base + size);
	else
		return (void *) (base);
}

static void free_block(struct heap_data *h,int index)
{
	struct heap_data *aux;
	h->available = 1;

	aux = freelist[index];
	while (aux) {
		if (aux == contiguous(h,h->size) && aux->available) {
			merge(aux,h,index);
			return;
		}
		aux = aux->next;
	}
	return;
}

static void merge(struct heap_data *h1,struct heap_data *h2,int index)
{
	struct heap_data *base;
	if (h1 > h2)
		base = h2;
	else
		base = h1;

	base->size = base->size << 1;
	remove_node(h1,index);
	remove_node(h2,index);
	add_node(base,index + 1);

	if (base->size != MAX_ALLOC)
		free_block(base,index + 1);
	else
		base->available = 1;
	return;
}

void free(void *ptr)
{
	struct heap_data *h = (struct heap_data *) (ptr - SIZE_HEAP_DATA);
	int index = get_index(h->size);

	free_block(h,index);
}
