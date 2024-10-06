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
// standard C includes
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

// Android includes.
#include <android/log.h>

// Project includes.
#include "argos.h"

argos_state_t *argos_boot(void)
{
	// Open the device node and iterate the subdirectories.
	DIR *proc_devtree_argos = opendir("/proc/device-tree/argos");
	if (!proc_devtree_argos)
	{
		__android_log_print(ANDROID_LOG_ERROR, "ARGOS", "argos: (ERR! %s) argos_boot: Fail to open /proc/device-tree/argos with(%d)\n", strerror(errno), errno);
		return NULL;
	}

	struct dirent *rddir_dirent = readdir(proc_devtree_argos);
	if (!rddir_dirent)
	{
		__android_log_print(ANDROID_LOG_ERROR, "ARGOS", "argos: (ERR! %s) argos_boot: Fail to open /proc/device-tree/argos with(%d)\n", strerror(errno), errno);
		return NULL;
	}

	size_t count = 1;
	while ((rddir_dirent = readdir(proc_devtree_argos)))
	{
		if (!strchr(rddir_dirent->d_name, '@'))
			count++;
	}
	closedir(proc_devtree_argos);

	argos_state_t *state = malloc(sizeof(argos_state_t));

	state->net_thru_fd = open("/dev/network_throughput", O_RDWR);
	if (state->net_thru_fd < 0)
	{
		__android_log_print(ANDROID_LOG_ERROR, "ARGOS", "argos: (ERR! %s) argos_boot: Device doesn't support qos\n", strerror(errno));
		return NULL;
	}

	argos_data_t *list_start = malloc(sizeof(argos_data_t));
	argos_data_t *iter = state->list_start = list_start;

	char buffer[64] = {0};
	for (int i = 1; i < count; ++i)
	{
		memset(iter, 0, sizeof(argos_data_t));
		iter->node_num = i;

		snprintf(buffer, sizeof(buffer), "/proc/device-tree/argos/boot_device@%d/net_boost,node", i);
		// XXX: This does not go anywhere, we need to copy it out of the buffer or process the data
		// right here and convert it to something used later.
		char *net_boost_node = read_node(buffer);
		strcpy(iter->node, net_boost_node);

		snprintf(buffer, sizeof(buffer), "/proc/device-tree/argos/boot_device@%d/net_boost,label", i);
		char *net_boost_label = read_node(buffer);
		strcpy(iter->label, net_boost_label);

		snprintf(buffer, sizeof(buffer), "/proc/device-tree/argos/boot_device@%d/net_boost,interval", i);
		char *net_boost_interval = read_node(buffer);

		// Convert to an integer.
		char *end = NULL;
		unsigned long interval = strtoul(net_boost_interval, &end, 10);
		if (errno == ERANGE)
		{
			__android_log_print(ANDROID_LOG_ERROR, "ARGOS", "argos: (ERR! %s) argos_boot: Device interval invalid\n", strerror(errno));
			// FIXME: maybe do something sane here??
		}
		iter->interval = interval;

		// Print some data about the node.
		__android_log_print(ANDROID_LOG_DEBUG, "ARGOS", "argos: argos_boot: NODE:%s\n", iter->node);
		__android_log_print(ANDROID_LOG_DEBUG, "ARGOS", "argos: argos_boot: LABEL:%s\n", iter->label);
		__android_log_print(ANDROID_LOG_DEBUG, "ARGOS", "argos: argos_boot: INTERVAL:%lu\n", iter->interval);

		// Calculate who has the smaller value and that becomes our new minimum.
		state->interval = min(state->interval, iter->interval);

		iter->next = malloc(sizeof(argos_data_t));
		iter = iter->next;
	}

	__android_log_print(ANDROID_LOG_DEBUG, "ARGOS", "argos: argos_boot: MIN INTERVAL:%lu", state->interval);
	
	return state;
}
