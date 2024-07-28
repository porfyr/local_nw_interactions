#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int client_sockets[2] = {0, 0};
pthread_mutex_t lock;

void *handle_client(void *client_socket) {
    int sock = *((int *)client_socket);
    char buffer[BUFFER_SIZE];
    int valread;

    while ((valread = read(sock, buffer, BUFFER_SIZE)) > 0) {
        buffer[valread] = '\0';

        pthread_mutex_lock(&lock);
        for (int i = 0; i < 2; i++) {
            if (client_sockets[i] != 0 && client_sockets[i] != sock) {
                send(client_sockets[i], buffer, strlen(buffer), 0);
            }
        }
        pthread_mutex_unlock(&lock);
    }

    close(sock);
    return NULL;
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

    pthread_t tid[2];
    pthread_mutex_init(&lock, NULL);

    printf("Waiting for connections...\n");

    for (int i = 0; i < 2; i++) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        pthread_mutex_lock(&lock);
        client_sockets[i] = new_socket;
        pthread_mutex_unlock(&lock);

        if (pthread_create(&tid[i], NULL, handle_client, (void *)&new_socket) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < 2; i++) {
        pthread_join(tid[i], NULL);
    }

    pthread_mutex_destroy(&lock);
    close(server_fd);
    return 0;
}
