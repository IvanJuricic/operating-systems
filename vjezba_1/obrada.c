#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#define N 6   /* broj razina proriteta */

int CEKANJE[N];
int PRIORITET[N];
int TEKUCI_PRIORITET=0;

int sig[N]={0,SIGUSR1,SIGUSR2,SIGBUS,SIGCONT,SIGINT};

void zabrani_prekidanje() {
   int i;
   for(i=1; i<N; i++) sighold(sig[i]);
}

void dozvoli_prekidanje() {
   int i;
   for(i=1; i<N; i++) sigrelse(sig[i]);

}

void obrada_signala(int i){
   /* obrada se simulira trošenjem vremena,

      obrada traje 5 sekundi, ispis treba biti svake sekunde */

      int br;

      switch (i)
      {
      case 1:
         printf("-  P  -  -  -  -\n");
         for(br=1;br<N;br++) {
            printf("-  %d  -  -  -  -\n", br);
            sleep(1);
         }
         printf("-  K  -  -  -  -\n");
         break;
      case 2:
         printf("-  -  P  -  -  -\n");
         for(br=1;br<N;br++) {
            printf("-  -  %d  -  -  -\n", br);
            sleep(1);
         }
         printf("-  -  K  -  -  -\n");
         break;

      case 3:
         printf("-  -  -  P  -  -\n");
         for(br=1;br<N;br++) {
            printf("-  -  -  %d  -  -\n", br);
            sleep(1);
         }
         printf("-  -  -  K  -  -\n");
         break;

      case 4:
         printf("-  -  -  -  P  -\n");
            for(br=1;br<N;br++) {
               printf("-  -  -  -  %d  -\n", br);
               sleep(1);
            }
            printf("-  -  -  -  K  -\n");
            break;

      case 5:
         printf("-  -  -  -  -  P\n");
         for(br=1;br<N;br++) {
            printf("-  -  -  -  -  %d\n", br);
            sleep(1);
         }
         printf("-  -  -  -  -  K\n");
         break;

      default:
         break;
      }

}

void prekidna_rutina(int sig){
   
   int n, i, j;
   n=-1;

   zabrani_prekidanje();
   
   switch(sig){
      case SIGUSR1:
         n=1;
         //printf("Vrijednost SIGUSR1 => %d\n", SIGUSR1);
         printf("-  X  -  -  -  -\n");
         break;
      case SIGUSR2:
         n=2;
         //printf("Vrijednost SIGUSR2 => %d\n", SIGUSR2);
         printf("-  -  X  -  -  -\n");
         break;
      case SIGBUS:
         n=3;
         //printf("Vrijednost SIGBUS => %d\n", SIGBUS);
         printf("-  -  -  X  -  -\n");
         break;
      case SIGCONT:
         n=4;
         //printf("Vrijednost SIGCONT => %d\n", SIGCONT);
         printf("-  -  -  -  X  -\n");
         break;
      case SIGINT:
         n=5;
         //printf("Vrijednost SIGINT => %d\n", SIGINT);
         printf("-  -  -  -  -  X\n");
         break;

   }

   CEKANJE[n]++;
   
   do {

      i = 0;

      for(j=TEKUCI_PRIORITET + 1; j < N; j++) {

         if(CEKANJE[j]) i = j;

      }

      if(i > 0) {

         CEKANJE[i]--;
         PRIORITET[i] = TEKUCI_PRIORITET;
         TEKUCI_PRIORITET = i;

         dozvoli_prekidanje();
         obrada_signala(i);
         zabrani_prekidanje();

         TEKUCI_PRIORITET=PRIORITET[i];
         PRIORITET[i]=0;
      
      }

   } while (i > 0);

   dozvoli_prekidanje();

}

int main () {
   sigset(SIGUSR1, prekidna_rutina);
   sigset(SIGUSR2, prekidna_rutina);
   sigset(SIGBUS, prekidna_rutina);
   sigset(SIGCONT, prekidna_rutina);
   sigset(SIGINT, prekidna_rutina);

   printf("Proces obrade prekida, PID=%ld\n", getpid());

   printf("GP S1 S2 S3 S4 S5\n");
   printf("-----------------\n");

   /* troši vrijeme da se ima šta prekinuti - 10 s */

   int br = 1;
   for(;;) {
      printf("%d  -  -  -  -  -\n", br);
      br++;
      sleep(1);
      if(br == 10) br = 1;
   }

   printf ("Zavrsio osnovni program\n");

   return 0;

}

