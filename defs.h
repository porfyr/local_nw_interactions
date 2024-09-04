#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define NAME_SIZE 32
#define MAX_CONNECTIONS 4

typedef struct {
    int *descriptors;
    int count;
    pthread_mutex_t mutex_descriptors;
    pthread_mutex_t mutex_full_connections;
} Client_sockets;

typedef struct {
    Client_sockets client_sockets;
    int sock;
} handle_client_arg_t;

typedef struct {
    pthread_mutex_t mutex;
    int sock;
} sigaction_context_t;

typedef struct {
    char *buffer;
    char *my_name;
    int sock;
} send_msgs_args_t;