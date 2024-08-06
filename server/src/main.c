#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_CONNECTIONS 3
// int curr_connections = 0;

// int client_sockets[MAX_CONNECTIONS] = {0, 0};
// int *client_sockets;
// pthread_mutex_t cs_mutex;

typedef struct {
    int *descriptors;   // before was global int *client_sockets
    pthread_mutex_t mutex;  // before was global pthread_mutex_t cs_mutex;
    int curr_connections;
} Client_sockets;

typedef struct {
    Client_sockets client_sockets;
    int sock;
} HC_arg;

void *handle_client(void* vp_hc_arg) {// void *client_socket) {
    HC_arg hc_arg = *((HC_arg *)vp_hc_arg);
    // int sock = *((int *)client_socket);
    Client_sockets client_sockets = hc_arg.client_sockets;
    int *cs_descriptors = client_sockets.descriptors;
    int sock = hc_arg.sock;
    char buffer[BUFFER_SIZE];
    int valread;

    while ((valread = read(sock, buffer, BUFFER_SIZE)) > 0) {
        buffer[valread] = '\0';
        printf("Зчитав значення у handle_client > while , buffer = %s\n", buffer);


        pthread_mutex_lock(&(client_sockets.mutex));
        for (int i = 0; i < MAX_CONNECTIONS; i++) {
            printf("client_sockets[%d] = %d у handle_client()\n", i, cs_descriptors[i]);
            if (cs_descriptors[i] != 0 && cs_descriptors[i] != sock) {
                send(cs_descriptors[i], buffer, strlen(buffer), 0);
            }
            printf("Надіслав\n");
        }
        pthread_mutex_unlock(&(client_sockets.mutex));
    }

    if (valread == 0) {
        printf("Client disconnected, socket = %d\n", sock);
        -- client_sockets.curr_connections;
    } else {
        perror("read");
    }

    pthread_mutex_lock(&(client_sockets.mutex));
    for (int i = 0; i < MAX_CONNECTIONS; ++i) {
        if (cs_descriptors[i] == sock){
            cs_descriptors[i] = 0;
        break;
        }
    }
    pthread_mutex_unlock(&(client_sockets.mutex));

    // printf("Кінець виконання handle_client\n");

    close(sock);
    return NULL;
}

int find_first_free_index(int *client_sockets) {
    for (int i = 0; i < MAX_CONNECTIONS; ++i) {
        if (client_sockets[i] == 0) {
            return i;
        }
    }
    return -1;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

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

    pthread_t tid[MAX_CONNECTIONS];
    // pthread_mutex_init(&cs_mutex, NULL);


    Client_sockets client_sockets = {
        .descriptors = (int*)calloc(MAX_CONNECTIONS, sizeof(int)),
        .curr_connections = 0
    };
    pthread_mutex_init(&(client_sockets.mutex), NULL);

    // client_sockets = (int *)calloc(MAX_CONNECTIONS, sizeof(int));
    // unsigned int curr_clients = 0;


    printf("Waiting for connections...\n");
    
    for (int i = 0; i < MAX_CONNECTIONS; ++i) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("socket accept");
            exit(EXIT_FAILURE);
        }
        ++ client_sockets.curr_connections;

        pthread_mutex_lock(&(client_sockets.mutex));
        client_sockets.descriptors[find_first_free_index(client_sockets.descriptors)] = new_socket;
        printf("client_sockets[%d] = %d у main()\n", i, client_sockets.descriptors[i]);
        pthread_mutex_unlock(&(client_sockets.mutex));

        HC_arg hc_arg= {
            .client_sockets = client_sockets,
            .sock = new_socket
        };
        if (pthread_create(&tid[i], NULL, handle_client, (void *)&hc_arg) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    printf("Вийшов за цикл CONNECTIONS\n");

    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        pthread_join(tid[i], NULL);
    }

    free(client_sockets.descriptors);
    pthread_mutex_destroy(&(client_sockets.mutex));
    close(server_fd);
    return 0;
}
