#ifndef RAW_MULTICAST_OUT_H
#define RAW_MULTICAST_OUT_H

typedef struct out_data {
	int socket_fd;
} out_data_t;

typedef struct out_client {
	struct sockaddr_in *client;
	int client_length;
} out_client_t;

void register_socket (int port);
void * out_loop (void *);

#endif //RAW_MULTICAST_OUT_H
