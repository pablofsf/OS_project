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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


//#define DEBUG
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
	fprintf(stderr,"We have %i nodes\n",count);
}
#endif

static struct heap_data *get_ptr(void *ptr)
{
	//Consideration: The ptr always exists in mem
	struct heap_data *h = head;

	while (h->addr != ptr) {
		h = h->next;
		if (h == NULL)
			return NULL;
	}
	
	return h;
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
		if (current->next == NULL)
			return NULL;
		
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

static void reduce_heap_size()
{
	void *heap_end;
	ssize_t reduce;

	heap_end = sbrk(0);
		
	if (tail)
		reduce = ((size_t) tail) + sizeof(struct heap_data) + tail->size - ((size_t) heap_end);
	else
		reduce = ((size_t) heap_start) - ((size_t) heap_end);
		
	sbrk(reduce);
#ifdef DEBUG			
	fprintf(stderr,"We have freed mem, properly? %lu\n",sbrk(0));
//	print_list();
#endif
}


void* malloc(size_t mem_size)
{
	char errstr[ERR_BUF];
	void *node_addr, *data_addr;
	struct heap_data *h;

#ifdef DEBUG
	fprintf(stderr,"Malloc: ");
#endif

/*	if (!mem_size)
		return NULL;
*/	
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
//	print_list();
	fprintf(stderr,"%lu. End is in %lu!, returning %lu\n",mem_size,sbrk(0),h->addr);
#endif
	return h->addr;

freeh:
	sbrk(- sizeof(struct heap_data));
err:
	error(errstr);
	return NULL;
}

void free(void *ptr)
{
	struct heap_data *h;
#ifdef DEBUG		
	fprintf(stderr,"Free: %lu:",ptr);
#endif
	if (ptr) {
	        h = remove_list_el(ptr);
		if (h == NULL)
			return;
		if (h->next == NULL) {
			reduce_heap_size();
#ifdef DEBUG		
			return;
#endif			
		}
	}

#ifdef DEBUG	
	fprintf(stderr,"We are leaving our free, %lu\n",sbrk(0));
//	print_list();
#endif
	return;
}

void *calloc(size_t nmemb, size_t size)
{
	void *ptr;
#ifdef DEBUG		
	fprintf(stderr,"Calloc:");
#endif
	
	ptr =  malloc(nmemb*size);
	bzero(ptr,nmemb*size);
	return ptr;
}

void *realloc(void *ptr, size_t size)
{
	void *tmp;
	struct heap_data *h;
	
#ifdef DEBUG		
	fprintf(stderr,"Realloc: %lu, size %lu",ptr,size);
#endif
	if (ptr) {
		h = get_ptr(ptr);
		if (h == NULL)
			return malloc(size);
		if (size) {
			if(h->size < size) {
#ifdef DEBUG
				fprintf(stderr,"Should reallocate mem\n");
#endif
				tmp = malloc(size);
				memcpy(tmp,ptr,h->size);
				free(ptr);
			} else {
#ifdef DEBUG
				fprintf(stderr,"Should reduce allocated memory\n");
#endif
				h->size = size;
				tmp = h->addr;
			}
			return tmp;
			
		} else {
			free(ptr);
		}
	} else {
		return malloc(size);
	}
	return NULL;
}
