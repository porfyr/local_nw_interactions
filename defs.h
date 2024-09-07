#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 256
#define NAME_SIZE 32
#define MAX_CONNECTIONS 4

typedef struct {
    int *descriptors;
    int count;
    pthread_mutex_t mutex_descriptors;
    pthread_mutex_t mutex_full_connections;
    struct sockaddr_in *address;
    int server_fd;
    pthread_t *th_ids;
} client_sockets_t;

typedef struct {
    client_sockets_t *client_sockets;
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