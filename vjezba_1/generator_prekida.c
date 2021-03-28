#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>

int pid=0;
int sig[]={SIGUSR1,SIGUSR2,SIGBUS,SIGCONT};

void prekidna_rutina(int sig){
   kill(pid, SIGKILL);
   exit(0);
}

int slucajan_broj(int d, int g) {
   return rand()%(g-d+1)+d;
}

int main(int argc, char *argv[]){
   pid=atoi(argv[1]);
   sigset(SIGINT, prekidna_rutina);
   int r;
   
   srand(time(NULL));
   
   while(1) {
      /* odspavaj 3-5 sekundi */
      /* slučajno odaberi jedan signal (od 4) */
      /* pošalji odabrani signal procesu 'pid' funkcijom kill*/
      sleep(slucajan_broj(3,5));
      r = slucajan_broj(0, 3);
      printf("Signal %d\n", r);
      kill(pid, sig[r]);
   }
   return 0;
}