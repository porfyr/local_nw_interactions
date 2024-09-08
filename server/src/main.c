#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "main.h"


void *handle_client(void* vp_hc_arg) {
    handle_client_arg_t *hc_arg = (handle_client_arg_t *)vp_hc_arg;
    client_sockets_t *client_sockets = hc_arg->client_sockets;
    int *descriptors = client_sockets->descriptors;
    int sock = hc_arg->sock;

    int valread = read_send_message(hc_arg);

    int sock_index = find_my_index(sock, descriptors);

    if (valread == 0) {
        printf("Client disconnected, socket = %d\n", sock);
        if (client_sockets->count == MAX_CONNECTIONS)
            pthread_mutex_unlock(&client_sockets->full_connections_mutex);
        -- client_sockets->count;
        client_sockets->descriptors[sock_index] = -2;
        if (client_sockets->count == 0) {
            pthread_mutex_lock(&connections_condition_mutex);
                connections_condition = 0;
            pthread_mutex_unlock(&connections_condition_mutex);
;       }
        print_clients(client_sockets);
    } else {
        perror("read");
    }

    pthread_mutex_lock(&(client_sockets->descriptors_mutex));
        for (int i = 0; i < MAX_CONNECTIONS; ++i) {
            if (descriptors[i] == sock){
                descriptors[i] = -2;
                break;
            }
        }
    pthread_mutex_unlock(&(client_sockets->descriptors_mutex));


    printf("Кінець виконання handle_client для sock=%d\n", sock);

    close(sock);
    return NULL;
}


int process_clients(client_sockets_t *client_sockets) {
    for (int i = 0; connections_condition; ) {
        // printf("iteration of client connection loop, i=%d\n", i);
        if (client_sockets->count == MAX_CONNECTIONS-1) {
            pthread_mutex_lock(&client_sockets->full_connections_mutex);
            printf("penaltimate client joined, lock full_connections_mutex\n");
        }
        // printf("    пройшов провірку == 3\n");

        // Check if client_connections isn't full, if so - continue, if no - wait until somebody disconnect
        // int trylock = pthread_mutex_trylock(&client_sockets.full_connections_mutex);
        // printf("trylock = %d\n", trylock);
        if (!pthread_mutex_trylock(&client_sockets->full_connections_mutex)) {
        // if (trylock) {
            pthread_mutex_unlock(&client_sockets->full_connections_mutex);
            // printf("    trylock\n");
        } else {
            pthread_mutex_lock(&client_sockets->full_connections_mutex);
            pthread_mutex_unlock(&client_sockets->full_connections_mutex);
        }

        printf("lock unlock done\n");


        int new_socket, addrlen = sizeof(*(client_sockets->address));
        // printf("before sock accept\n");
        if ((new_socket = accept(client_sockets->server_fd, (struct sockaddr *)client_sockets->address, (socklen_t *)&addrlen)) < 0) {
            perror("socket accept");
            exit(EXIT_FAILURE);
        }
        // printf("sock accepted\n");

        pthread_mutex_lock(&(client_sockets->descriptors_mutex));
            client_sockets->descriptors[find_first_free_index(client_sockets->descriptors)] = new_socket;
        pthread_mutex_unlock(&(client_sockets->descriptors_mutex));
        ++ client_sockets->count;
        
        printf("+ client i=%d sock=%d\n", i, client_sockets->descriptors[i]);
        fflush(stdout);

        handle_client_arg_t hc_arg = {
            .client_sockets = client_sockets,
            .sock = new_socket
        };

        // printf("before thread creation\n");
        if (pthread_create(&client_sockets->th_ids[i], NULL, handle_client, (void *)&hc_arg) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
        // printf("after thread creation\n");

        print_clients(client_sockets);

        if (i < MAX_CONNECTIONS-1)
            ++ i;
    }

    return 1; // seccess
}


int main() {
    int server_fd;
    struct sockaddr_in address;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 2) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    pthread_t th_ids[MAX_CONNECTIONS];

    int descriptors[MAX_CONNECTIONS];
    for (int i = 0; i < MAX_CONNECTIONS; ++i)
        descriptors[i] = -2;

    client_sockets_t client_sockets = {
        .descriptors = descriptors,
        .count = 0,
        // pthread_mutex_t descriptors_mutex;
        // pthread_mutex_t full_connections_mutex;
        .address = &address,
        .server_fd = server_fd,
        .th_ids = th_ids
    };
    pthread_mutex_init(&(client_sockets.descriptors_mutex), NULL);
    pthread_mutex_init(&(client_sockets.full_connections_mutex), NULL);
    pthread_mutex_init(&connections_condition_mutex, NULL);


    printf("Waiting for connections...\n");
    
    process_clients(&client_sockets);

    printf("left connections cycle loop, then threads joining\n");

    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        printf("joining %d thread\n", i);
        pthread_join(th_ids[i], NULL);
    }

    pthread_mutex_destroy(&(client_sockets.descriptors_mutex));
    pthread_mutex_destroy(&(client_sockets.full_connections_mutex));
    pthread_mutex_destroy(&(connections_condition_mutex));
    close(server_fd);
    return 0;
}
