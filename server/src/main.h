#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "../../defs.h"


int connections_condition = 1;
pthread_mutex_t connections_condition_mutex;


static int find_my_index(int sock, int *descriptors) {
    for (int i = 0; i < MAX_CONNECTIONS; ++i) {
        if (sock == descriptors[i])
            printf("found first free index=%d\n", i);   
            return i;
    }
    perror("find_my_index failed, not found");
    return -1;
}


static void print_clients(client_sockets_t *client_sockets) {
    printf("clients descriptors: ");    // necessary
    for (int i = 0; i < MAX_CONNECTIONS; ++i)
        printf("%d ", client_sockets->descriptors[i]);
    printf(", count = %d\n", client_sockets->count);
}


static int read_send_message(handle_client_arg_t *hc_arg) {
    int *descriptors = hc_arg->client_sockets->descriptors;

    int valread;
    char buffer[BUFFER_SIZE];

    while ((valread = read(hc_arg->sock, buffer, BUFFER_SIZE)) > 0) {
        fflush(stdout);
        buffer[valread] = '\0';
        // printf("Зчитав значення у handle_client > while , buffer = %s\n", buffer);
        pthread_mutex_lock(&(hc_arg->client_sockets->descriptors_mutex));
            for (int i = 0; i < MAX_CONNECTIONS; i++) {
                if (descriptors[i] > 0 && descriptors[i] != hc_arg->sock) {
                    send(descriptors[i], buffer, strlen(buffer), 0);
                    printf("Надіслав %d -> %d\n", hc_arg->sock, descriptors[i]);
                }
            }
        pthread_mutex_unlock(&(hc_arg->client_sockets->descriptors_mutex));
    }

    return valread;
}


static int find_first_free_index(int *client_sockets) {
    for (int i = 0; i < MAX_CONNECTIONS; ++i) {
        if (client_sockets[i] == -2) {
            return i;
        }
    }
    return -1;
}