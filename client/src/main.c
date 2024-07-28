#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void *receive_messages(void *socket) {
    int sock = *((int *)socket);
    char buffer[BUFFER_SIZE];
    int valread;

    while ((valread = read(sock, buffer, BUFFER_SIZE)) > 0) {
        buffer[valread] = '\0';
        printf("Received: %s\n", buffer);
        printf("You ") ;
    }

    return NULL;
}

int main() {
    int sock;
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE];
    pthread_t tid;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Socket creation error\n");
        return -1;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
        printf("Invalid address or Address not supported\n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        printf("Connection failed\n");
        return -1;
    }

    if (pthread_create(&tid, NULL, receive_messages, (void *)&sock) != 0) {
        perror("pthread_create");
        return -1;
    }

    while (1) {
        bzero(buffer, BUFFER_SIZE);
        printf("You: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        send(sock, buffer, strlen(buffer), 0);
    }

    close(sock);
    return 0;
}
