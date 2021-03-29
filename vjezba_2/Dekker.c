#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <malloc.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/wait.h>

#define NUMPROCESS 2

int *pravo, *zastavica, *A, processId;

int main() {

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
                processIncrement();
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