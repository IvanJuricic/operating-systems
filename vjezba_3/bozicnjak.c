//#include <cstddef>
#include <bits/pthreadtypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>
#include <malloc.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

//#define DRETVE 3
sem_t *dida, *konzalting, *K, *sob;
int sobovi, patuljci, segId;

int slucajan_broj(int d, int g) {
   return rand()%(g-d+1)+d;
}

void *proces_patuljak() {

    // CekajBSEM(K)

    patuljci++;
    if(patuljci == 3); //PostaviBSEM(dida)

    // PostaviBSEM(K)
    // CekajOSEM(konzalting)

}

void *proces_sob() {

}

void *proces_sjeverni_pol() {

    pthread_t *sob;
    pthread_t *patuljak;

    for(;;) {

        srand(time(NULL));
        sleep(slucajan_broj(1,3));
        // Provjera jesu li se sobovi vratili i vjv pojavljivanja
        if(1 == 1) {
            if(pthread_create(sob, NULL, dretva_sob, NULL)) {
                printf("\t\nError creating new thread!\n");
                exit(1);
            }
        // Provjera vjv hoce li se patuljak pojaviti
        } else if(1 == 2) {
            if(pthread_create(patuljak, NULL, dretva_patuljak, NULL)) {
                printf("\t\nError creating new thread!\n");
                exit(1);
            }
        }

    }

}

void *proces_dida_mraz() {

    for(;;) {

        // CekajBSEM(dida)
        // CekajBSEM(K)

        if(sobovi == 10 && patuljci > 0) {
            // Postavi BSEM(K)

            sleep(2); //Ukrcaj poklone i nosi

            // Cekaj BSEM(K)

            // Postavi OSEM(sobovi, 10)
            sobovi = 0;
        } else if(sobovi == 10) {
            // Postavi BSEM(K)

            sleep(2); //Nahrani sobove

            // Cekaj BSEM(K)
        } //ako je samo_tri_patuljka_pred_vratima {
		while(patuljci > 4) {
			// PostaviBSem(K)
			sleep(2); // Rijesi problem
			// CekajBSem(K)

			// PostaviOSem(konzultacije, 3) //povećaj semafor za 3
			patuljci -= 3;
		}
		//PostaviBSem(K)

    }

}

int main(int argc, char *argv[]) {

    pthread_t *sjeverni_pol;
    pthread_t *dida_mraz;

	segId = shmget(IPC_PRIVATE, sizeof(sem_t)*4, 0600);
	sem_init(&dida, 0, 1);
    sem_init(&konzalting, 0, 1);
    sem_init(&K, 0, 1);
    sem_init(&sob, 0, 1);

    if(pthread_create(sjeverni_pol, NULL, dretva_sjeverni_pol, NULL)) {
        printf("\t\nError creating thread North Pole!\n");
        exit(1);
    }

    if(pthread_create(dida_mraz, NULL, dretva_dida_mraz, NULL)) {
        printf("\t\nError creating thread Santa!\n");
        exit(1);
    }

    pthread_join(*sjeverni_pol, NULL);
    pthread_join(*dida_mraz, NULL);
    

    return 0;
}