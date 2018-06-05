/************************************************************************************
* linked.c: Linked list that implements malloc()
* This file is part of EDAF35 memory management project
* 	                                                                            *
* Copyright (C) 2018 Pablo Correa Gomez                                             *
*                                                                                   *
*    This program is free software; you can redistribute it and/or modify           *
*    it under the terms of the GNU General Public License as published by           *
*    the Free Software Foundation; either version 3 of the License, or              *
*    (at your option) any later version.                                            *
*                                                                                   *
*    This program is distributed in the hope that it will be useful,                *
*    but WITHOUT ANY WARRANTY; without even the implied warranty of                 *
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                  *
*    GNU General Public License for more details.                                   *
*    ( http://www.fsf.org/licenses/gpl.txt )                                        *
************************************************************************************/

#include "linked.h"
#include <errno.h>
#include <string.h>
#include <stdio.h>

#define DEBUG
//This might also have to be place inside kernel memory
//That would mean that some initialization has to be done in malloc
static struct heap_data* head = NULL;
static struct heap_data* tail = NULL;
static struct heap_data* heap_start;
extern errno;
#ifdef DEBUG	
static int count;

static void print_list()
{	
	fprintf(stdout,"We have %i list nodes\n",count);
//	fprintf(stdout,"Tail is: %lu, head is: %lu\n",tail,head);
}
#endif

static size_t get_size(void *ptr)
{
	//Consideration: The ptr always exists in mem
	struct heap_data *h = head;

	while (h->addr != ptr) {
		h = h->next;
	}

	return h->size;
}

static struct heap_data *remove_list_el(void *ptr)
{
	struct heap_data *current = head;
	struct heap_data *temp = NULL;
		
	if (head->next == NULL) {
		temp = head;
		head = tail = NULL;
		return temp;
	}

	if (head->addr == ptr) {
		temp = head;
		head = head->next;
		return temp;
	}

	while(current->next->addr != ptr){
		current = current->next;
	}

	temp = current->next;
	current->next = temp->next;

	if (current->next == NULL) {
		tail = current;
	}
	
	return temp;
}

static void addlast(struct heap_data *val)
	{
	struct heap_data *current = head;
#ifdef DEBUG	
	count++;
#endif
	while(current->next != NULL) {
	current = current-> next;
}
	
	current->next = val;
	tail = val;
}

static void error(const char *str)
{
	perror(str);
}

void* malloc(size_t mem_size)
{
	char errstr[ERR_BUF];
	void *node_addr, *data_addr;
	struct heap_data *h;

#ifdef DEBUG		
	fprintf(stdout,"Malloc: ");
#endif
	node_addr = sbrk(sizeof(struct heap_data));
	if (node_addr == -1) {
		strncpy(errstr,"Couldn't allocate list node",ERR_BUF);
		goto err;
        }
	h = (struct heap_data*) node_addr;

	data_addr = sbrk(mem_size);
	if (data_addr == -1) {
		strncpy(errstr,"Couldn't increase heap size for data allocation",ERR_BUF);
		goto freeh;
	}
	
	h->addr = (void*) data_addr;
	h->size = mem_size;
	h->next = NULL;

	if (head != NULL) {
		addlast(h);
	} else {
		head = h;
		tail = h;
		heap_start = h;
	}
#ifdef DEBUG	
	print_list();
	fprintf(stdout,"%lu. End is in %lu!\n",mem_size,sbrk(0));
#endif
	return h->addr;

freeh:
	sbrk(- sizeof(struct heap_data));
err:
	error(errstr);
	return NULL;
}

void free(void *addr)
{
	struct heap_data *h;
	void *heap_end;
	ssize_t reduce;
#ifdef DEBUG		
	fprintf(stdout,"Free:");
#endif
	if (addr) {
	        h = remove_list_el(addr);
	}
	
	if (h->next == NULL) {
		heap_end = sbrk(0);
		
		if (tail)
			reduce = ((size_t) tail) + sizeof(struct heap_data) + tail->size - ((size_t) heap_end);
		else
			reduce = ((size_t) heap_start) - ((size_t) heap_end);
		
		sbrk(reduce);
#ifdef DEBUG			
		fprintf(stdout,"We have freed mem, properly? %lu\n",sbrk(0));
		print_list();
#endif
		return;
	}

#ifdef DEBUG	
	fprintf(stdout,"We are leaving our free, %lu\n",sbrk(0));
//	print_list();
#endif
	return;
}

void *calloc(size_t nmemb, size_t size)
{
	return malloc(nmemb*size);
}

void *realloc(void *ptr, size_t size)
{
	void *tmp;
	size_t old_size, alloc_size;
#ifdef DEBUG		
//	fprintf(stderr,"Realloc:");
#endif
	if (ptr) {
		if (size) {
			fprintf(stderr,"Should reallocate mem\n");
			tmp = malloc(size);
			
			old_size = get_size(ptr);
			alloc_size = old_size < size ? old_size : size;
			memcpy(tmp,ptr,alloc_size);
			//	free(ptr);
			
			return tmp;
		} else {
			free(ptr);
		}
	} else {
		return malloc(size);
	}
	return NULL;
}
