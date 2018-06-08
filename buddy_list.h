#include <unistd.h>


struct heap_data {
	void *addr;
	size_t size;
	struct heap_data *next;
	struct heap_data *prev;
	struct heap_data *data;
	int available :1;
};
