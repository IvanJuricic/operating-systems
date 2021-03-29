#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <malloc.h>
#include <stdlib.h>

int numOfThreads, numOfIterations, A = 0;
void *threadIncrement() {
    for(int i = 0; i < numOfIterations; i++) A++;
    return NULL;
}

int main(int argc, char *argv[]) {

    pthread_t *thread;

    if(argc != 3) {
        printf("\n\tUsage:    %s broj_dretvi broj_iteracija\n", argv[0]);
        exit(1);
    }

    numOfThreads = atoi(argv[1]);
    numOfIterations = atoi(argv[2]);

    thread = malloc(numOfThreads * sizeof(pthread_t));

    for(int i = 0; i < numOfThreads; i++) {
        if(pthread_create(&thread[i], NULL, threadIncrement, NULL)) {
            printf("\t\nError creating new thread!\n");
            exit(1);
        }
    }

    for(int i = 0; i < numOfThreads; i++) {
        pthread_join(thread[i], NULL);
    }

    printf("\n\tVarijabla A na kraju = %d\n", A);
    return 0;

}