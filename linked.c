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
#include <linux/list.h>
#include <errno.h>

//This might also have to be place inside kernel memory
//That would mean that some initialization has to be done in malloc
static LIST_HEAD(heap_list);
static heap_size;
extern edata;
extern errno;

static void error(const char str)
{
	perror(str);
}

void *malloc(size_t mem_size)
{
	char errstr[ERR_BUF];
	int node_addr,data_addr;
	struct heap_data *h;
	//TODO: Allocate list head somewhere
	
	node_addr = sbrk(sizeof heap_data);
	if (node_addr == -1) {
		strncpy(errstr,"Couldn't allocate list node",ERR_BUF);
		goto err;
	}
	h = (struct heap_data*) list_addr;

	data_addr = sbrk(mem_size);
	if (data_addr == -1) {
		strncpy(errstr,"Couldn't increase heap size for data allocation",ERR_BUF);
		goto freeh:
	}
	heap_size += mem_size + sizeof(heap_data);
	h->addr = (void*) addr;
	h->size = mem_size;
	INIT_LIST_HEAD(&h->list);//TODO: replace 

	list_add_tail(&h->list,&heap_list);//TODO: replace
	return h->addr;

freeh:
	sbrk(- sizeof heap_data);
err:
	error(errstr);
	return NULL;
}

static void free_heap(struct heap_data *data)
{
	heap_size -= (data->size + sizeof(heap_data));
	
	//TODO: If the element is the last element in the list, then we can shrink
	if (heap_list.prev == data->list) {
		int addr = sbrk(0);
		sbrk(heap_size + &edata - addr);
	}
	
	list_del(&data->list);
	kfree(data);
}

void free(void *addr)
{
	if (addr) {
		struct heap_data *data, *next;

		list_for_each_entry_save(data,next,&heap_list,list) {
			if (data->addr == addr) {
				free_heap(data);
			}	
		}
	}
	return;
}
