#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_CONNECTIONS 10
int curr_connections = 0;

// int client_sockets[MAX_CONNECTIONS] = {0, 0};
int *client_sockets;
pthread_mutex_t cs_mutex;