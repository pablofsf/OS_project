/************************************************************************************
* linked.h: Linked list that implements malloc()
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

#ifndef __LINKED_H__
#define __LINKED_H__

#include <unistd.h>

void *malloc(size_t mem_size);
void free(void *);

struct heap_data {
	void *addr;
	size_t size;
	struct heap_data *next;
};

struct heap_data* remove(struct heap_data **head, void *val);
void addlast(heap_data *head, heap_data *val);

#define ERR_BUF 1024

#endif //__LINKED_H__
