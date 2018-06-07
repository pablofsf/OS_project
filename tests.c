/************************************************************************************
* tests.c: Tests to check malloc() function
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

#include <stdio.h>
#include "linked.h"

int main()
{
	int val = 5;
	int *arr1, *arr2, *arr3;
	arr1 = (int *) malloc(val * sizeof(int));
	arr2 = (int *) malloc((val+1) * sizeof(int));
	arr3 = (int *) malloc((val+2) * sizeof(int));
	
	for (int i = 0; i < val; i++) {
		arr1[i] = i;
		arr2[i] = 2 * i;
		arr3[i+1] = 3 * i;
	}

	free(arr2);
	free(arr3);
	free(arr1);

	return 0;
}
