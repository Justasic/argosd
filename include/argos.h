/*
 * Purpose: Open source reimplimentation of Samsung's "Adaptive Resource 
 * Governor for Operating Systems" daemon.
 *
 * Copyright (C) 2024  Justin Crawford
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#pragma once
#include <stdint.h>

// https://stackoverflow.com/a/58532788
#define max(a,b)             \
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a > _b ? _a : _b;       \
})

#define min(a,b)             \
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b;       \
})

typedef struct argos_data_s
{
	int node_num;
	char node[64];
	char label[64];
	uint32_t interval;
	uint32_t sysnode;

	// Linked lists aren't really that efficient but they're easy.
	struct argos_data_s *next;
} argos_data_t;

typedef struct argos_state_s
{
	uint32_t interval;
	int net_thru_fd;

	argos_data_t *list_start;
} argos_state_t;

extern char *read_node(const char *node);
extern argos_state_t *argos_boot(void);
