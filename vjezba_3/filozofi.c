#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <signal.h>
//#include <string.h>

#define BROJ_FILOZOFA 5

// Globalne varijable
char filozofi[5] = {'O', 'O', 'O', 'O', 'O'};
int viljuske[5] = {1, 1, 1, 1, 1};

// Monitor
pthread_mutex_t monitor;
pthread_cond_t cekanje[5];

void filozof_jede(int id);
void filozof_razmislja();
void *dretva_filozof(int *id);

int main() {

        int *index;
        pthread_t *dretva;
        dretva = malloc(BROJ_FILOZOFA * sizeof(pthread_t));
        index = malloc(BROJ_FILOZOFA * sizeof(int));

        pthread_mutex_init(&monitor, NULL);
        pthread_cond_init(cekanje, NULL);

        for(int i = 0; i < BROJ_FILOZOFA; i++) {

                index[i] = i;
                
                if(pthread_create(&dretva[i], NULL, (void *)dretva_filozof, &index[i])) {
                        printf("\t\nError creating new thread!\n");
                        exit(1);
                }
        }

        for(int i = 0; i < BROJ_FILOZOFA; i++) {
                pthread_join(dretva[i], NULL);
        }

        pthread_mutex_destroy(&monitor);
        pthread_cond_destroy(cekanje);

        return 0;
}

void *dretva_filozof(int *id) {
        // Petlja misli - jedi zauvijek
        printf("\n\tStvoren %d. filozof\n", *id + 1);

        for(;;) {
                filozof_razmislja();
                filozof_jede(*id);
        }
}

void filozof_razmislja() {
        sleep(3);
}

// "Stanje filozofa: X o O X o" (X-jede, O-razmišlja, o-čeka na vilice).

void filozof_jede(int id) {

        pthread_mutex_lock(&monitor);
        
        // kriticni odsjecak
        // filozof na mjestu id ceka vilice
        filozofi[id] = 'o';

        while(viljuske[id] == 0 || viljuske[(id + 1) % 5] == 0)
                pthread_cond_wait(&cekanje[id], &monitor);

        viljuske[id] = viljuske[(id + 1) % 5] = 0;
        
        filozofi[id] = 'X';
        
        printf("\n%s (%d)", filozofi, id+1);
        
        pthread_mutex_unlock(&monitor);

        sleep(2); // vrijeme kroz koje filozof jede

        pthread_mutex_lock(&monitor);

        filozofi[id] = 'O';

        viljuske[id] = viljuske[(id + 1) % 5] = 1;
        
        if(id == 0){
                pthread_cond_signal(&cekanje[BROJ_FILOZOFA-1]);
        }
        else{
                pthread_cond_signal(&cekanje[id-1]);
        }
        printf("\n%s (%d)", filozofi, id+1);

        pthread_mutex_unlock(&monitor);
}