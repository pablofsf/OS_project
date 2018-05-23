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

//This might also have to be place inside kernel memory
//That would mean that some initialization has to be done in malloc
static struct heap_data* head = NULL;
static unsigned int heap_size;
extern void* edata;
extern errno;

struct heap_data *remove_list_el(struct heap_data **head, void *val){
	struct heap_data *current = *head;
	struct heap_data *temp = NULL;

	if ((*head)->next == NULL || (*head)->addr == val)
		return head;

	while(current->next->addr != val){
		current = current->next;
	}

	temp = current->next;
	current->next = temp->next;

	return temp;
}

void addlast(struct heap_data *head, struct heap_data *val){
	struct heap_data *current = head;
	while(current->next != NULL){
		current = current-> next;
	}

	current->next = val;
}

static void error(const char *str)
{
	perror(str);
}

void* malloc(size_t mem_size)
{
	//printf("We got into our malloc, hooray!\n");
	char errstr[ERR_BUF];
	void *node_addr, *data_addr;
	struct heap_data *h;
	//TODO: Allocate list head somewhere

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
	heap_size += mem_size + sizeof(struct heap_data);
	h->addr = (void*) data_addr;
	h->size = mem_size;
	h->next = NULL;

	if (head != NULL)
		addlast(head,h);
	else
		head = h;

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

	if (addr) {
	        h = remove_list_el(&head,addr);
	}

	heap_size -= (h->size + sizeof(struct heap_data));

	if (h->next == NULL) {
		if (h == head)
			head = NULL;
		void *addr = sbrk(0);
		sbrk(heap_size + edata - addr);
		return;
	}

	if (h == head)
		head = h->next;

	return;
}
