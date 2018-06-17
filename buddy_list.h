#include <unistd.h>


struct heap_data {
	size_t size;
	struct heap_data *next;
	struct heap_data *prev;
};
