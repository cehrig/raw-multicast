#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "out.h"
#include "log.h"
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

out_data_t out_data;
out_client_t out_client;

/**
 * Checks if client already registered
 * @param client
 * @return
 */
int client_exists (struct sockaddr_in *client)
{
	int i;
	struct sockaddr_in *it = out_client.client;

	for (i = 0; i < out_client.client_length; i++) {
		if (it->sin_port == client->sin_port
			&& it->sin_addr.s_addr == client->sin_addr.s_addr) {
			return 1;
		}
		it++;
	}

	return 0;
}

/**
 * Register new client
 * @param client
 * @return
 */
int push_client (struct sockaddr_in *client)
{
	void * out_mem;

	if (!client_exists(client)) {
		out_client.client_length++;

		out_client.client = realloc (out_client.client,
									 out_client.client_length
									 * sizeof (struct sockaddr_in ));


		out_mem = memcpy (out_client.client + ((out_client.client_length-1)),
					  client, sizeof(struct sockaddr_in));

		if(!out_mem) {
			return -1;
		}
	}

	return 0;
}

/**
 * Register listening socket
 * @param port
 */
void register_socket (int port)
{
	/**
	 * No clients at the beginning
	 */
	out_client.client_length = 0;
	out_client.client = NULL;

	int so_status;
	int so_yes = 1;
	struct sockaddr_in address;

	out_data.socket_fd = socket (AF_INET, SOCK_DGRAM, 0);

	if (out_data.socket_fd < 0) {
		writelog (LOG_ERROR, "can't get socket");
	}

	so_status = setsockopt (out_data.socket_fd, SOL_SOCKET, SO_REUSEADDR,
						   &so_yes, sizeof (int));

	if (so_status < 0) {
		writelog (LOG_ERROR, "can't set SO_REUSEADDR");
	}

	/**
	 * Binding to port
	 */
	memset (&address, 0, sizeof (struct sockaddr_in));
	address.sin_family = AF_INET;
	address.sin_port = htons (port);
	address.sin_addr.s_addr = htonl (INADDR_ANY);

	so_status = bind (out_data.socket_fd, (struct sockaddr *) &address,
					 sizeof	(address));

	if (so_status < 0) {
		writelog (LOG_ERROR, "can't bind");
	}
}

void * out_loop (void * data)
{
	int n;
	char in_buffer[RMC_OUT_BUFFER];

	struct sockaddr_in clientaddr;
	int clientlen = sizeof (clientaddr);

	while (1) {
		n = recvfrom (out_data.socket_fd, in_buffer, RMC_OUT_BUFFER, 0,
					  (struct sockaddr *) &clientaddr, &clientlen);

		if(push_client (&clientaddr) < 0) {
			writelog (LOG_ERROR, "can't push new client");
		}
	}
}