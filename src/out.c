#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "out.h"
#include "dispatch.h"
#include "log.h"
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

out_data_t out_data;

void register_output (int port)
{
    register_socket (port);
}

/**
 * Register listening socket
 * @param port
 */
void register_socket (int port)
{
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
	address.sin_port = htons ((unsigned short) port);
	address.sin_addr.s_addr = htonl (INADDR_ANY);

	so_status = bind (out_data.socket_fd, (struct sockaddr *) &address,
					 sizeof	(address));

	if (so_status < 0) {
		writelog (LOG_ERROR, "can't bind");
	}
}

/**
 * Waiting for new connections and pushing new clients to client buffer
 * @param data
 * @return
 */
void * out_loop (void * mutex)
{
	int n;
	char in_buffer[RMC_OUT_BUFFER];

	struct sockaddr_in clientaddr;
	int clientlen = sizeof (clientaddr);

	while (1) {
		n = recvfrom (out_data.socket_fd, in_buffer, RMC_OUT_BUFFER, 0,
					  (struct sockaddr *) &clientaddr, &clientlen);

		if (push_client (&clientaddr) < 0) {
			writelog (LOG_ERROR, "can't push new client");
		}
	}
}

void send_out (struct sockaddr_in *sockaddr, unsigned char *msg, size_t len)
{
    sendto(out_data.socket_fd, msg, len, 0, (struct sockaddr *)sockaddr, sizeof
    	(struct sockaddr));
}