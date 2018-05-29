#ifndef RAW_MULTICAST_IN_H
#define RAW_MULTICAST_IN_H

#include <poll.h>

void register_input ();
void * stdin_loop (void *);

extern struct pollfd in_fd[1];

#endif //RAW_MULTICAST_IN_H
