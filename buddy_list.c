#include "buddy_list.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

#define MIN_ALLOC_BITS 5
#define MAX_ALLOC_BITS 20
#define MIN_ALLOC ((size_t)1 << MIN_ALLOC_BITS)
#define MAX_ALLOC ((size_t)1 << MAX_ALLOC_BITS)
#define DATA_SIZES (MAX_ALLOC_BITS - MIN_ALLOC_BITS + 1)
#define SIZE_HEAP_DATA (sizeof(struct heap_data))

#define ABUG
static void merge(struct heap_data *h1,struct heap_data *h2,int index);
static void free_block(struct heap_data *h,int index);


static struct heap_data* freelist[DATA_SIZES];
static struct heap_data* head;

static int get_index(size_t size)
{
	int i;
	for(i = DATA_SIZES - 1; i >= 0; i--){
		if(size > (1 << (i + MIN_ALLOC_BITS - 1))){
			break;
		}
	}
	return i;
}

static void printlist(){
	for(int i = DATA_SIZES - 1; i >= 0; i--){
		if(freelist[i] == NULL){
			fprintf(stderr, "%i: Empty\n",i);
		}else{
			struct heap_data *temp = freelist[i];
			fprintf(stderr,"%i: ",i);
			while(temp != NULL){
				fprintf(stderr, "%p ",(void *) temp);
				temp = temp->next;
			}
			fprintf(stderr, "\n");

		}
	}
	fprintf(stderr, "\n");

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
	head = sbrk(MAX_ALLOC+sizeof(struct heap_data));
	if(head == (void*) -1){
		perror("Cannot allocate memory");
	}

	head->size = MAX_ALLOC;
	head->next = NULL;
	head->prev = NULL;

	freelist[DATA_SIZES - 1] = head;

}

void *malloc(size_t size){
#ifdef ABUG
	fprintf(stderr, "We are in malloc: %lu\n",size);
#endif
	if(head == NULL){
		init();
	}

	if(size > (MAX_ALLOC - SIZE_HEAP_DATA) || size == 0){
		return NULL;
	}

	int req = get_index(size + SIZE_HEAP_DATA);

	int available = req;

	struct heap_data *temphead = freelist[req];

//FIX, position of available
	while(temphead == NULL && available < (DATA_SIZES - 1)){
		available++;
		temphead = freelist[available];
	}

	if(temphead == NULL){
		fprintf(stderr,"Not enough memory\n");
		return NULL;
	}

	while(available > req){
#ifdef ABUG
		fprintf(stderr,"available: %i\n",available);
		printlist();
#endif
		struct heap_data *parent = freelist[available];
		freelist[available] = parent->next;

		if(parent->next != NULL){
			freelist[available]->prev = NULL;
		}

		available--;

		struct heap_data *buddy1 = parent;
		struct heap_data *buddy2 = (struct heap_data *) ((size_t) parent + (parent->size >> 1));

		buddy1->size = buddy2->size = 1 << (available + MIN_ALLOC_BITS);
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
#ifdef ABUG
	printlist();
	fprintf(stderr,"return %p, %lu\n",(void *) ((size_t) new_parent + sizeof(struct heap_data)),size);
#endif
	return (void *) ((size_t) new_parent + sizeof(struct heap_data));
}

static void *contiguous(struct heap_data *h,size_t size)
{
	size_t addr = ((size_t) h) - (size_t) head;
	size_t buddy = addr ^ (size);

	return (void *) (buddy + ((size_t) head));
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
#ifdef ABUG
	fprintf(stderr,"We are in free: %p\n",ptr);
#endif
	//Fix, pointer null
	if (ptr == NULL)
		return;
	
	struct heap_data *h = (struct heap_data *) ((size_t) ptr - SIZE_HEAP_DATA);
	int index = get_index(h->size);

	h->next = freelist[index];
	freelist[index] = h;
	free_block(h,index);
#ifdef ABUG
	printlist();
#endif
}

void *calloc(size_t nmemb, size_t size)
{
#ifdef ABUG
	fprintf(stderr, "We are in calloc:\n");
#endif
	void *ptr;

	ptr =  malloc(nmemb*size);
	if (ptr)
		memset(ptr,'\0',nmemb*size);
	return ptr;
}

void *realloc(void *ptr, size_t size)
{
#ifdef ABUG
	fprintf(stderr, "We are in realloc: %p, %lu\n",ptr,size);
#endif
	void *tmp;
	struct heap_data *h;


	if (ptr == NULL) {
		return malloc(size);
	}

	h = (struct heap_data *) ((size_t) ptr - SIZE_HEAP_DATA);
//	if (h == NULL)
//		return malloc(size);

	if (size) {
		if((h->size - SIZE_HEAP_DATA) < size) {
			tmp = malloc(size);
			memcpy(tmp,ptr,h->size - SIZE_HEAP_DATA);
			free(ptr);
		} else {
			tmp = ptr;
		}
		return tmp;

	} else {
		free(ptr);
	}

	return NULL;
}
