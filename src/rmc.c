#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "in.h"
#include "dispatch.h"
#include "out.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void usage (int exit_code)
{
	fprintf (exit_code ? stderr : stdout, "%s %s\n\
usage:\n\
	<producer> | rmc -p <port number>\n\
options:\n\
	-p <port number>    Port to listen on\n\
	\n", PACKAGE, VERSION);
	exit (1);
}

int main (int argc, char **argv)
{
	int c, index;
	unsigned char l_udp = 0;
	char *l_host = NULL;
	char *l_port = NULL;
	pthread_t thread_in, thread_out, thread_dispatch;

	opterr = 0;
	while ((c = getopt (argc, argv, "hup:f")) != -1) {
		switch (c) {
			case 'h':
				usage (1);
				break;
			case 'u':
				l_udp = 1;
				break;
			case 'p':
				l_port = optarg;
				break;
			case '?':
				if (optopt == 'p')
					fprintf (stderr,
						 "Option -%c requires an argument.\n", optopt);
				else if (isprint (optopt))
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf (stderr,
						 "Unknown option character `\\x%x'.\n", optopt);
				usage (1);
			default:
				abort ();
		}
	}

	for (index = optind; index < argc; index++) {
		printf ("Non-option argument %s\n", argv[index]);
	}

	if (!l_port) {
		usage (1);
	}

	/**
	 * Setting up input and output
	 */
	register_input ();
	register_dispatch();
	register_output ((int) strtol (l_port, NULL, 10));

	/**
	 * Starting input loop
	 */
	pthread_create (&thread_in, NULL, stdin_loop, NULL);

    /**
     * Starting output loop
     */
    pthread_create (&thread_out, NULL, out_loop, NULL);

    /**
	 * Starting dispatch monitor
	 */
    pthread_create (&thread_dispatch, NULL, monitor_queues, NULL);

	/**
	 * Waiting for threads to finish
	 */
	pthread_join (thread_in, NULL);
	pthread_join (thread_out, NULL);

	return 0;
}