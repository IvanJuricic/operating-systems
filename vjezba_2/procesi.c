#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <malloc.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/wait.h>

int segmentId, numOfProcesses, numOfIterations, *A;

void processIncrement() {
    for(int i = 0; i < numOfIterations; i++) *A+=1;
}

int main(int argc, char *argv[]) {

    if(argc != 3) {
        printf("\n\tUsage:    %s broj_procesa broj_iteracija\n", argv[0]);
        exit(1);
    }

    numOfProcesses = atoi(argv[1]);
    numOfIterations = atoi(argv[2]);

    segmentId = shmget(IPC_PRIVATE, sizeof(int), 0600);
    if( segmentId == -1) exit(1);

    A = (int *) shmat(segmentId, NULL, 0);
    *A = 0;


    int processId;

    for(int i = 0; i < numOfProcesses; i++) {
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

    for(int i = 0; i < numOfProcesses; i++) wait(NULL);

    printf("\n\tVarijabla A na kraju = %d\n", *A);
    
    shmdt((char *) A);
    shmctl(processId, IPC_RMID, NULL);

    return 0;

}