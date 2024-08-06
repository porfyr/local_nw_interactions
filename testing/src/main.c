// Online C compiler to run C program online
#include <stdio.h>
#include <pthread.h>

void *func(void *arg) {
    int *a = arg;
    scanf("%d", a);
    
    printf("дождався\n");

    *(int*)arg = *a;
    return a;
}


int main() {
    // Write C code here
    
    pthread_t t;
    
    
    int a;
    printf("Try programiz.pro\nYou: ");
    // scanf("%d", &a);
    pthread_create(&t, NULL, func, &a);
    

    printf("%d\n", a);

    pthread_join(t, NULL);

    return 0;
}
