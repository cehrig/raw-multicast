#ifndef RAW_MULTICAST_DISPATCH_H
#define RAW_MULTICAST_DISPATCH_H

/* size_t */
#include <unistd.h>

typedef struct out_client_data {
    struct sockaddr_in *client;
    unsigned char * data;
    size_t data_len;
} out_client_data_t;

typedef struct out_client {
    out_client_data_t *clients;
    int client_length;
} out_client_t;

typedef struct out_queue_wrapper {
    unsigned char * data;
    size_t data_len;
} out_queue_wrapper_t;

void register_dispatch ();
void add_data (size_t, unsigned char *);
int push_client (struct sockaddr_in *);
void * monitor_queues (void *);

#endif //RAW_MULTICAST_DISPATCH_H
