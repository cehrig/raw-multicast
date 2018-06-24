#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "in.h"
#include "dispatch.h"
#include "log.h"
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/select.h>

struct pollfd in_fd[1];

/**
 * Register stdin for polling
 * @return
 */
void register_input()
{
	int stdin, flags;

	/**
	 * If stdin is not set we are using 0 by default
	 */
	#ifndef STDIN_FILENO
	stdin = 0;
    #else
	stdin = STDIN_FILENO;
    #endif

	if(fcntl (stdin, F_GETFD) < 0) {
		writelog (LOG_ERROR, "stdin is not a valid file descriptor");
	}

	/**
	 * Setting stdin to non-blocking mode
	 */
	flags = fcntl (stdin, F_GETFL, 0);
	if (fcntl (stdin, F_SETFL, flags | O_NONBLOCK) < 0) {
		writelog (LOG_ERROR, "cannot set stdin to non-blocking mode");
	}

	memset (in_fd, 0, sizeof (struct pollfd));
	in_fd[0].fd = stdin;
	in_fd[0].events = POLLIN;
	in_fd[0].revents = 0;
}

/**
 * reading from stdin
 *
 * @param data
 * @return
 */
void * stdin_loop(void * mutex)
{
	int in_poll;
	ssize_t read_bytes;

	unsigned char in_data[RMC_IN_BUFFER];

	do {
		in_fd[0].revents = 0;
		in_poll = poll (in_fd, 1, RMC_POLL_TIMEOUT);

		/**
		 * error or timed out
		 */
		if (in_poll < 1) {
			continue;
		}

		while ((read_bytes = read (in_fd[0].fd, in_data, RMC_IN_BUFFER)) > 0) {
			if (read_bytes < 1) {
			    continue;
			}

			/* pushing data to dispatcher */
			add_data ((size_t) read_bytes, in_data);
		}

        if (in_fd[0].revents & POLLHUP) {
            writelog (LOG_ERROR, "remote end hung up");
        }
	} while (1);

	return NULL;
}