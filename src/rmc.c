#include "rmc.h"
#include <stdio.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char ** argv)
{
	int c, index;
	char *port;

	opterr = 0;
	while ((c = getopt (argc, argv, "p:")) != -1)
	{
		switch (c)
		{
			case 'p':
				port = optarg;
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
				return 1;
			default:
				abort ();
		}
	}


	for (index = optind; index < argc; index++)
	{
		printf ("Non-option argument %s\n", argv[index]);
	}


	printf("-> %s \n", port);
	return 0;
}