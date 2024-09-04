// Online C compiler to run C program online
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int foo(int *a) {
    
    *a = 8;
    
    return 0;
}

int main() {
    // Write C code here
    
    int a;
    
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    
    // pthread_mutex_lock(&mutex);
    
    a = pthread_mutex_trylock(&mutex);
    printf("trylock returned %d\n", a);
    
    pthread_mutex_unlock(&mutex);
    // ptherad_mutex_destroy(&mutex);

    return 0;
}
