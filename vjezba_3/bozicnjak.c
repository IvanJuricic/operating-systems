//#include <bits/pthreadtypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>
#include <malloc.h>
//#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <signal.h>

struct SPREMNIK {
    sem_t dida;
    sem_t konzalting;
    sem_t K;
    sem_t sob;
    int sobovi;
    int patuljci;
} *spremnik;

int segId;

int slucajan_broj(int d, int g) {
   return rand()%(g-d+1)+d;
}

void CekajOSEM(sem_t *semafor) {
    sem_wait(semafor);
}

void PostaviOSEM(sem_t *semafor) {
    sem_post(semafor);
}

void *proces_patuljak() {

    // CekajBSEM(K)
    CekajOSEM(&spremnik->K);

    spremnik->patuljci++;

    printf("\nBroj patuljaka => %d\n", spremnik->patuljci);

    if(spremnik->patuljci == 3) PostaviOSEM(&spremnik->dida); //PostaviBSEM(dida)

    // PostaviBSEM(K)
    PostaviOSEM(&spremnik->K);
    // CekajOSEM(konzalting)
    CekajOSEM(&spremnik->konzalting);

    exit(0);

}

void *proces_sob() {
    // CekajBSEM(K)
    CekajOSEM(&spremnik->K);

    spremnik->sobovi++;

    printf("\nBroj sobova => %d\n", spremnik->sobovi);

    if(spremnik->sobovi == 10) PostaviOSEM(&spremnik->dida); //PostaviBSEM(dida)

    // PostaviBSEM(K)
    PostaviOSEM(&spremnik->K);

    exit(0);
}

void *proces_sjeverni_pol() {

    printf("\nPokrenut proces sjeverni pol\n");

    int sob_ili_patuljak;
    pid_t pid;

    for(;;) {

        srand(time(NULL));
        sleep(slucajan_broj(1,3));
        // Provjera jesu li se sobovi vratili i vjv pojavljivanja
        sob_ili_patuljak = rand()%2;
        if(sob_ili_patuljak == 1 && spremnik->sobovi < 10) {
            // Stvaranje procesa sob
            pid = fork();
            if (pid == 0) {
                //printf("\nStvoren proces sob!\n");
                proces_sob();
            } else if(pid == -1) {
                printf("\n\tGreska prilikom stvaranja procesa sob!\n");
                exit(1);
            }
        } else {
            // Stvaranje procesa patuljak
            pid = fork();
            if (pid == 0) {
                //printf("\nStvoren proces patuljak!\n");
                proces_patuljak();
            } else if(pid == -1) {
                printf("\n\tGreska prilikom stvaranja procesa patuljak!\n");
                exit(1);
            }
        }

    }

}

void *proces_dida_mraz() {

    printf("\nPokrenut proces dida mraz\n");

    for(;;) {

        // Binarni semafor dida
        CekajOSEM(&spremnik->dida);

        // Binarni semafor K
        CekajOSEM(&spremnik->K);

        if(spremnik->sobovi == 10 && spremnik->patuljci > 0) {
            // Postavi BSEM(K)
            PostaviOSEM(&spremnik->K);

            printf("\n\t\tDjed mraz raznosi poklone!\n");
            sleep(1); //Ukrcaj poklone i nosi

            // Cekaj BSEM(K)
            CekajOSEM(&spremnik->K);

            // Postavi OSEM(sobovi, 10)
            for(int i = 0; i < 10; i++) PostaviOSEM(&spremnik->sob);
            
            spremnik->sobovi = 0;
            
        } else if(spremnik->sobovi == 10) {
            // Postavi BSEM(K)
            PostaviOSEM(&spremnik->K);

            printf("\n\t\tDjed mraz hrani sobove!\n");
            sleep(1); //Nahrani sobove

            // Cekaj BSEM(K)
            CekajOSEM(&spremnik->K);

        } else if(spremnik->patuljci >= 3) {
            
            // PostaviBSem(K)
            PostaviOSEM(&spremnik->K);

            printf("\n\t\tDjed mraz prica s patuljcima!\n");
			sleep(1); // Rijesi problem
			
            // CekajBSem(K)
            CekajOSEM(&spremnik->K);

			// PostaviOSem(konzultacije, 3) //povećaj semafor za 3
			for(int i = 0; i < 3; i++) PostaviOSEM(&spremnik->konzalting);

            spremnik->patuljci -= 3;

        }

        //PostaviBSem(K)
        PostaviOSEM(&spremnik->K);
		
    }

}

void brisi_sve() {
    
    printf("\nBrisem sve iz spremnika!\n");

    sem_destroy(&spremnik->dida);
    sem_destroy(&spremnik->konzalting);
    sem_destroy(&spremnik->K);
    sem_destroy(&spremnik->sob);

    shmdt(spremnik);
    shmctl(segId, IPC_RMID, NULL);

    exit(0);
}


int main(int argc, char *argv[]) {

    pid_t pid;

    // Dohvacanje dijeljenog segmenta
	segId = shmget(IPC_PRIVATE, sizeof(struct SPREMNIK), 0600);

    if (segId == -1) {
        printf("\n\tGreska prilikom stvaranja memorijskog bloka\n");
        exit(1);
    }

    spremnik = (struct SPREMNIK*) shmat(segId, NULL, 0);

    // Inicijalizacija semafora i podataka u zajednickom spremniku

    spremnik->patuljci = 0;
    spremnik->sobovi = 0;
	sem_init(&spremnik -> dida, 1, 0);
    sem_init(&spremnik -> konzalting, 1, 0);
    sem_init(&spremnik -> K, 1, 1);
    sem_init(&spremnik -> sob, 1, 0);
    
    sigset(SIGINT, brisi_sve);

    // Stvaranje procesa dida mraz
    pid = fork();
    if (pid == 0) {
        printf("\nStvoren proces dida mraz!\n");
        proces_dida_mraz();
    } else if(pid == -1) {
        printf("\n\tGreska prilikom stvaranja procesa dida mraz!\n");
        exit(1);
    }

    // Stvaranje procesa sjeverni pol
    pid = fork();
    if (pid == 0) {
        printf("\nStvoren proces sjeverni pol!\n");
        proces_sjeverni_pol();
    } else if(pid == -1) {
        printf("\n\tGreska prilikom stvaranja procesa sjeverni pol!\n");
        exit(1);
    }

    wait(NULL);
    wait(NULL);

    sem_destroy(&spremnik->dida);
    sem_destroy(&spremnik->konzalting);
    sem_destroy(&spremnik->K);
    sem_destroy(&spremnik->sob);

    shmdt(spremnik);
    shmctl(segId, IPC_RMID, NULL);

    return 0;
}