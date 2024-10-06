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

// Standard C includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

// Android-specific includes
#include <android/log.h>

// Project specific includes
#include "sysconf.h"
#include "argos.h"

char buffer[128] = {0};
char *read_node(const char *node)
{
    FILE *stream = fopen(node, "r");
    if (!stream)
    {
		__android_log_print(ANDROID_LOG_ERROR, "ARGOS", "argos: (ERR! %s) %s: Failed to open with(%d)\n", strerror(errno), "read_node", errno);
        return NULL;
    }

    char *str = fgets_unlocked(buffer, sizeof(buffer), stream);
    if (!str)
    {
        // Replace with android_log_buf_print?
        __android_log_print(ANDROID_LOG_ERROR, "ARGOS", "argos: (ERR! %s) %s: fgets error with(%d)\n", strerror(errno), "read_node", errno);
        fclose(stream);
        return NULL;
    }

    return str;
}

int main(int argc, char **argv)
{
	// Just handle -v and --version.
	for (int i = 0; i < argc; ++i)
	{
		if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--version"))
		{
			printf("Adaptive Resource Governor for Operating Systems Daemon (" PROJECT_NAME " " PROJECT_VERSION ")\n");
			printf("Written my Justin Crawford (NightShadow/Justasic)\n\n");
			printf("Copyright (C) 2024 Justin Crawford\n");
			printf("This is free software; see the source for copying conditions.  There is NO\n");
			printf("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
			return EXIT_SUCCESS;
		}
	}

	// Post to logcat that we've started for debugging reasons.
	__android_log_print(ANDROID_LOG_DEBUG, "ARGOS", "argos: main: Argos Daemon Boot\n");

	argos_state_t *state = argos_boot();

	while (true)
	{
		for (argos_data_t *it = state->list_start; it; it = it->next)
		{
			// XXX: This node may not exist?
			snprintf(buffer, sizeof(buffer), "/proc/device-tree/argos/boot_device@%d/net_boost,sysnode", it->node_num);
			char *net_boost_sysnode = read_node(buffer);
			it->sysnode = strtoull(net_boost_sysnode, NULL, 10);
			if (errno == ERANGE)
			{
				__android_log_print(ANDROID_LOG_ERROR, "ARGOS", "argos: (ERR! %s) argos_main: Error parsing %s sysnode value(%d)\n", strerror(errno), buffer, errno);
				it->sysnode = 0;
				continue; // FIXME: sanity??
			}
		}
		
		FILE *fp = fopen("/proc/net/dev", "r");
		if (!fp)
		{
			__android_log_print(ANDROID_LOG_ERROR, "ARGOS", "argos: (ERR! %s) argos_main: Failed to open node with(%d)\n", strerror(errno), errno);
			close(state->net_thru_fd);
			return EXIT_FAILURE;
	}
		
		// The original daemon reads the data slowly using fgets. I'm fucking lazy, we're gonna read the whole ass file 
		// into the ram and parse it using strtok to get rid of the first two headers of the file. This is somewhat
		// inefficient but easier to work with.
		fseek(fp, 0, SEEK_END);
		size_t file_sz = ftell(fp);
		rewind(fp);

		// Copy the data.
		char *netdev = malloc(file_sz);
		fread(netdev, file_sz, 1, fp);
		fclose(fp);

		// Now parse the data.
		char *rest = netdev;
		char *token;
		size_t pos = 0;

		while ((token = strtok_r(netdev, "\n", &rest)))
		{
			// Skip the first and second headers of this pseudo-file
			if (pos == 0 || pos == 1)
			{
				pos++;
				continue;
			}

			for (argos_data_t *it = state->list_start; it; it = it->next)
			{
				// ???? some random iteration to some point in the net dev file??
				char interface_name[32];
				unsigned long long RX = 0;
				unsigned long long TX = 0;

				sscanf(token, "%30[^:]%*[:] %20llu %*s %*s %*s %*s %*s %*s %*s %20llu", interface_name, &RX, &TX);

				// TODO: Tokenize the string then compare it to something???
				// Then use something that gets "RX" and "TX" strings???
				// Looks like the strings are compared to something possibly in argos_data_t?
			}

			pos++;
		}


		// Iterate the list of interfaces, check if it's our time to check them
		// then check it.
		for (argos_data_t *it = state->list_start; it; it = it->next)
		{
			
			// write(state.net_thru_fd, )
		}

		free(netdev);
		usleep(state->interval * 1000);
	}


	__android_log_print(ANDROID_LOG_DEBUG, "ARGOS", "argos: main: Argos Daemon Shutdown\n");

	return EXIT_SUCCESS;
}
