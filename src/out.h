#ifndef RAW_MULTICAST_OUT_H
#define RAW_MULTICAST_OUT_H

#include <unistd.h>
#include <netinet/ip.h>

typedef struct out_data {
	int socket_fd;
} out_data_t;

void send_out (struct sockaddr_in *sockaddr, unsigned char *msg, size_t len);
void register_output (int);
void register_socket (int);
void * out_loop (void *);


#endif //RAW_MULTICAST_OUT_H
