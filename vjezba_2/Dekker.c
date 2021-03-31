#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <malloc.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/wait.h>

#define NUMPROCESS 2

int segmentId, *pravo, *zastavica, *A, numOfIterations, processId;

void into_cs(int i, int j) {

    zastavica[i] = 1;
    while(zastavica[j] != 0) {
        if(*pravo == j) {
            zastavica[i] = 0;
            while(*pravo == j);
            zastavica[i] = 1;
        }
    }
}

void exit_cs(int i, int j) {
    *pravo = j;
    zastavica[i] = 0;
}

void processIncrement(int i) {
    
    into_cs(i, 1 - i);
    for(int j = 0; j < numOfIterations; j++) *A+=1;
    exit_cs(i, 1 - i);
        //sleep(1);
}

int main(int argc, char *argv[]) {

    if(argc != 2) {
        printf("\n\tUsage:    %s broj_iteracija\n", argv[0]);
        exit(1);
    }

    numOfIterations = atoi(argv[1]);

    segmentId = shmget(IPC_PRIVATE, sizeof(int), 0600);
    if( segmentId == -1) exit(1);

    A = (int *) shmat(segmentId, NULL, 0);
    pravo = A + 1;
    zastavica = pravo + 2;

    *A = 0;
    *pravo = 0;
    *(zastavica) = 0;
    *(zastavica + 1) = 0;

    int processId;

    for(int i = 0; i < NUMPROCESS; i++) {
        switch (processId = fork()) {
            case -1:
                printf("\t\nUnable to create new process!\n");
                exit(1);
                
            case 0:
                processIncrement(i);
                exit(0);
            
            default:
                break;
        }
    }

    for(int i = 0; i < NUMPROCESS; i++) wait(NULL);

    printf("\n\tVarijabla A na kraju = %d\n", *A);
    
    shmdt((char *) A);
    shmctl(processId, IPC_RMID, NULL);

    return 0;

}