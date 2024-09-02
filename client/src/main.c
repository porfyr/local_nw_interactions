#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

#include "../../defs.h"

pthread_mutex_t mutex;

void get_ready_to_hande() {
    // printf("\e[1A\e[2K\r"); // Очищення рядка та переміщення курсора на рядок вгору
    printf("\e[2K\r"); // Очищення рядка та переміщення курсора на початок
    printf("You: ");
}

void *receive_messages(void *socket) {
    int sock = *((int *)socket);
    char buffer[BUFFER_SIZE];
    int valread;

    while ((valread = read(sock, buffer, BUFFER_SIZE)) > 0) {
        buffer[valread] = '\0';
            // printf("\r");    // перенести курсор на початок рядка
        printf("\e[2K\r");
        printf("%s", buffer);
        // printf("You: ");
        // printf("\e[1A");
        get_ready_to_hande();
        fflush(stdout);
        get_ready_to_hande();
    }

    return NULL;
}

int set_name(char *my_name) {
    char format_string[8];
    snprintf(format_string, sizeof(format_string), "%%%ds", NAME_SIZE-1);
    printf("Type your name: ");
    if (scanf(format_string, my_name) < 0)
        perror("scanf failed");
    if (!strncmp(my_name, "You:", 5))
        perror("Name can't be \"You:\"");
    return 1;
}

int finish(pthread_mutex_t mutex, int sock) {
    pthread_mutex_destroy(&mutex);
    close(sock);
    exit(0);

    return 0;
}

int main() {
    char my_name[NAME_SIZE];
    set_name(&my_name);

    printf("-- Got name: %s --\n", my_name);

    int sock;
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE];
    pthread_t tid;
    pthread_mutex_init(&mutex, NULL);

    signal(SIGINT, finish);

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
        fflush(stdin);
        get_ready_to_hande();
        fgets(buffer, BUFFER_SIZE-NAME_SIZE, stdin);
        char msg[BUFFER_SIZE];
        snprintf(msg, BUFFER_SIZE, "%s: %s", my_name, buffer);
        send(sock, msg, strlen(msg), 0);
    }

    pthread_mutex_destroy(&mutex);
    close(sock);

    return 0;
}











// printf("\e[0K"); // Переміщення курсора до кінця рядка