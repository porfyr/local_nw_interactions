#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define NAME_SIZE 32
#define MAX_CONNECTIONS 2

typedef struct {
    int *descriptors;
    pthread_mutex_t mutex;
    int curr_connections;
} Client_sockets;

typedef struct {
    Client_sockets client_sockets;
    int sock;
} HC_arg;
