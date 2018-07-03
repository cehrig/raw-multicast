#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "dispatch.h"
#include "out.h"
#include "log.h"
#include <arpa/inet.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

out_client_t out_client;
pthread_mutex_t mtx_dispatch, mtx_cond_dispatch;
pthread_cond_t cond_dispatch;

void register_dispatch ()
{
    pthread_mutex_init(&mtx_dispatch, NULL);
    pthread_mutex_init(&mtx_cond_dispatch, NULL);
    pthread_cond_init(&cond_dispatch, NULL);

    /**
     * No clients at the beginning
     */
    out_client.client_length = 0;
    out_client.clients = NULL;
}

/**
 * Checks if client already registered
 * @param client
 * @return
 */
int client_exists (struct sockaddr_in *client)
{
    int i;
    out_client_data_t *it = out_client.clients;

    for (i = 0; i < out_client.client_length; i++) {
        if (it->client->sin_port == client->sin_port
            && it->client->sin_addr.s_addr == client->sin_addr.s_addr) {

            return 1;
        }
        it++;
    }

    return 0;
}

/**
 * Returns number of registered clients
 * @return
 */
int clients_registered ()
{
    return out_client.client_length;
}

/**
 * Register new client
 * @param client
 * @return
 */
int push_client (struct sockaddr_in *client)
{
    pthread_mutex_lock(&mtx_dispatch);
    void *out_mem;

    if (!client_exists (client)) {
        out_client.client_length++;

        /**
         * Creating new client node
         */
        out_client_data_t *client_data;
        client_data = malloc (sizeof (out_client_data_t));
        client_data->client = malloc (sizeof (struct sockaddr_in));
        client_data->data = NULL;
        client_data->data_len = 0;
        memcpy (client_data->client, client, sizeof (struct sockaddr_in));

        /**
         * Resizing Clients Storage
         */
        out_client.clients = realloc (out_client.clients,
                                     out_client.client_length
                                     * sizeof (out_client_data_t));

        /**
         * Copy client node to head
         */
        out_mem = memcpy (out_client.clients + (out_client.client_length-1),
                          client_data, sizeof(out_client_data_t));

        /**
         * Freeing client node
         */
        free (client_data);

        writelog (LOG_DEBUG, "registering new client: %s (source port: %d)",
                  inet_ntoa (client->sin_addr), htons (client->sin_port));

        if (!out_mem) {
            pthread_mutex_unlock(&mtx_dispatch);
            return -1;
        }
    }

    pthread_mutex_unlock(&mtx_dispatch);
    return 0;
}

int push_data_to_clients (size_t len, unsigned char * data)
{
    int i;
    out_client_data_t *it = out_client.clients;

    for (i = 0; i < out_client.client_length; i++) {
        if(!(it->data = realloc (it->data, it->data_len+len))) {
            writelog (LOG_DEBUG, "could not realloc client data, when pushing"
                                 " data");
        }

        if(!memcpy (it->data + it->data_len, data, len)) {
            writelog(LOG_ERROR, "could not copy to client data");
        }

        it->data_len += len;
        it++;
    }

    return 0;
}

void add_data (size_t len, unsigned char * data)
{
    pthread_mutex_lock(&mtx_dispatch);
    /* checking if clients are registered, if not do nothing */
    if (!clients_registered ()) {
        //writelog (LOG_DEBUG, "data but no client is registered");
        pthread_mutex_unlock(&mtx_dispatch);
        return;
    }

    push_data_to_clients (len, data);
    pthread_mutex_unlock(&mtx_dispatch);
    pthread_cond_signal(&cond_dispatch);
}

out_queue_wrapper_t * prepare_push(out_client_data_t *client)
{
    size_t len = 0, ur_len = 0, l_len = client->data_len;

    out_queue_wrapper_t *pass = malloc (sizeof (out_queue_wrapper_t));
    pass->data = NULL;
    pass->data_len = 0;

    do {
        ur_len = client->data_len;
        if (ur_len > RMC_DISPATCH_CHUNKSIZE) {
            len = RMC_DISPATCH_CHUNKSIZE;
        } else {
            len = client->data_len;
        }

        l_len -= len;
        unsigned char * left_mem = calloc (l_len, sizeof (unsigned char));

        pass->data = realloc (pass->data, pass->data_len + len);
        memcpy (pass->data + pass->data_len, client->data, len);
        memcpy(left_mem, client->data+len, l_len);

        free(client->data);

        pass->data_len += len;
        client->data_len = l_len;
        client->data = left_mem;
    } while(client->data_len > 0);

    return pass;
}

void * monitor_queues (void *data)
{
    int i, r ;
    out_client_data_t *it = NULL;
    out_queue_wrapper_t *wrapper = NULL;

    while(1) {
        pthread_mutex_lock(&mtx_cond_dispatch);
        pthread_cond_wait(&cond_dispatch, &mtx_cond_dispatch);
        pthread_mutex_lock(&mtx_dispatch);

        //writelog(LOG_DEBUG, "running");

        it = out_client.clients;
        for (i = 0; i < out_client.client_length; i++) {

            if (it->data_len > 0) {
                r++;
                wrapper = prepare_push(it);
                send_out (it->client, wrapper->data, wrapper->data_len);
                free (wrapper->data);
                free (wrapper);
            }
            it++;
        }

        pthread_mutex_unlock(&mtx_dispatch);
        pthread_mutex_unlock(&mtx_cond_dispatch);

    }

    return NULL;
}