#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

#include "../../defs.h"

pthread_mutex_t mutex;  // now unused
int keep_work = 1;  // global mutable

void get_ready_to_hande() {
    //// regexes
    // printf("\r");    // перенести курсор на початок рядка
    // printf("\e[1A"); // Очищення рядка
    // printf("\e[1A\e[2K\r");  // Очищення рядка та переміщення курсора на рядок вгору
    // printf("\e[0K"); // Переміщення курсора до кінця рядка
    printf("\e[2K\r"); // Очищення рядка та переміщення курсора на початок
    printf("You: ");
}

void *receive_msgs(void *vp_socket) {
    int sock = *((int *)vp_socket);
    char buffer[BUFFER_SIZE];
    int valread;

    while ((valread = read(sock, buffer, BUFFER_SIZE)) > 0) {
        buffer[valread] = '\0';
        printf("\e[2K\r");
        printf("%s", buffer);
        get_ready_to_hande();
        fflush(stdout);
        get_ready_to_hande();
    }

    return NULL;
}


int set_name(char my_name[BUFFER_SIZE]) {
    char format_string[8];
    snprintf(format_string, sizeof(format_string), "%%%ds", NAME_SIZE-1);
    printf("Type your name: ");
    if (scanf(format_string, my_name) < 0)
        perror("scanf failed");
    if (!strncmp(my_name, "You:", 5))
        perror("Name can't be \"You:\"");
    return 1;
}


void stop_work(int sig) {
    keep_work = 0;
    // printf("keep_work = 0\n");
}


void finish(int sig, siginfo_t *siginfo, void *context) {
    printf("finishing, k_w = %d\n", keep_work);
    sigaction_context_t *sigaction_conext = (sigaction_context_t*)context;
    pthread_mutex_destroy(&(sigaction_conext->mutex));
    close(sigaction_conext->sock);
    printf("End of client's runtime\n");
    exit(0);
}


int init_client(int *sock, struct sockaddr_in *server_address, pthread_t *tid) {
    if ((*sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error\n");
        return -1;
    }

    server_address->sin_family = AF_INET;
    server_address->sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server_address->sin_addr) <= 0) {
        perror("Invalid address or Address not supported\n");
        return -1;
    }

    if (connect(*sock, (struct sockaddr *)server_address, sizeof(*server_address)) < 0) {
        perror("Connection failed\n");
        return -1;
    }
    printf("connected\n");

    if (pthread_create(tid, NULL, receive_msgs, (void *)sock) != 0) {
        perror("pthread_create");
        return -1;
    }

    return 0;
}


void *send_msgs(void *vp_args) {
    send_msgs_args_t args = *(send_msgs_args_t*)vp_args;

    while (keep_work) {
        // printf("keep_work = %d\n", keep_work);
        bzero(args.buffer, BUFFER_SIZE);
        fflush(stdin);
        get_ready_to_hande();
        fgets(args.buffer, BUFFER_SIZE-NAME_SIZE, stdin);
        char msg[BUFFER_SIZE];
        snprintf(msg, BUFFER_SIZE, "%s: %s", args.my_name, args.buffer);
        send(args.sock, msg, strlen(msg), 0);
    }

    return NULL;
}


int main() {
    char my_name[NAME_SIZE];
    set_name(my_name);

    printf("-- Got name: %s --\n", my_name);

    int sock;
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE];
    pthread_t tid;
    pthread_mutex_init(&mutex, NULL);

    // works not so, ^C inputs with Enter key
    signal(SIGINT, stop_work);


    if (init_client(&sock, &server_address, &tid) != 0) {
        perror("init_client failed");
    }
    
    // printf("connected\n");


    // maybe it will be in new thread
    send_msgs_args_t send_msgs_args = {
        .buffer = buffer,
        .my_name = my_name,
        .sock = sock
    };
    send_msgs((void*)&send_msgs_args);
    

    // finish();

    pthread_mutex_destroy(&mutex);
    close(sock);

    printf("finished");

    return 0;
}
