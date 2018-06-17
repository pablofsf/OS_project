#include "buddy_list.h"
#include <errno.h>

#define MIN_ALLOC ((size_t)1 << 4)
#define MAX_ALLOC ((size_t)1 << 16)
#define DATA_SIZES 13
#define SIZE_HEAP_DATA ((void *) sizeof(struct heap_data))

static void merge(struct heap_data *h1,struct heap_data *h2,int index);
static void free_block(struct heap_data *h,int index);


static struct heap_data* freelist[DATA_SIZES];
static struct heap_data head;
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

	head.size = MAX_ALLOC;
	head.next = NULL;
	head.prev = NULL;

	freelist[12] = &head;

}

void *malloc(size_t size){

	if(startmem == NULL){
		init();
	}

	if(size > MAX_ALLOC || size == 0){
		return NULL;
	}

	int req = get_index(size);

	int available = req;

	struct heap_data *temphead = freelist[req];

	while(temphead == NULL && available < 13){
		available++;
		temphead = freelist[available];
	}

	if(temphead == NULL){
		return NULL;
	}

	while(available > req){

		struct heap_data *parent = freelist[available];
		freelist[available] = parent->next;

		if(parent->next != NULL){
			freelist[available]->prev = NULL;
		}

		available--;

		struct heap_data *buddy1 = parent;
		struct heap_data *buddy2 = ((size_t) parent) + (parent->size >> 1);

		buddy1->size = buddy2->size = () + MIN_ALLOC;
		buddy1->prev = NULL;
		buddy2->prev = buddy1;
		buddy1->next = buddy2;
		buddy2->next = freelist[available];

  		freelist[available] = buddy1;
	}

	struct heap_data *new_parent = freelist[available];
	freelist[available] = freelist[available]->next;

	if(new_parent->next != NULL){
		new_parent->next->prev = NULL;
		new_parent->prev = NULL;
	}

	printlist();

	return (void *) ((size_t) new_parent + sizeof(struct heap_data));
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

	//First block is the one that has just been added
	aux = freelist[index]->next;
	while (aux) {
		if (aux == contiguous(h,h->size)) {
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

	return;
}

void free(void *ptr)
{
	struct heap_data *h = (struct heap_data *) (ptr - SIZE_HEAP_DATA);
	int index = get_index(h->size);

	h->next = freelist[index];
	freelist[index] = h;
	free_block(h,index);
}

void printlist(){
	for(int i = 12; i > 0; i--){
		if(freelist[i] == NULL){
			fprintf(stderr, "Not created \n");
		}else{
			struct heap_data *temp = freelist[i];
			while(temp != NULL){
				fprintf(stderr, "size: %lu ",temp->size);
				temp = temp->next;
		}
	}
	fprintf(stderr, "\n");
	}

}
