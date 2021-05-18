#include <bits/pthreadtypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <malloc.h>
#include <time.h>

// Struktura za pracenje rupa u memoriji
// id => indeks zadnjeg clana u memoriji nakon kojega postoji rupa
// num => broj rupa nakon tog clana
struct Rupa {
	int id, num;
};

void ispisi_stanje(int *spremnik, int velicina_spremnika);
void napuni_spremnik(int *spremnik, int velicina_spremnika, int *zahtjev, int velicina_zahtjeva);
void oslobodi_zahtjev(int *spremnik, int velicina_spremnika, int zahtjev);
void isprazni_spremnik(int *spremnik, int velicina_spremnika);
int provjeri_spremnik(int *spremnik, int velicina_spremnika, int velicina_zahtjeva);
struct Rupa provjeri_slobodno(int *spremnik, int velicina_spremnika, int velicina_zahtjeva);
int slucajan_broj(int d, int g);

int main(int argc, char *argv[]) {

	int *velicina_spremnika, *mem = NULL, zahtjev = 0, *pZahtjev = &zahtjev, tmp;
	char naredba, oslobadanje;
	

	printf("\n\tSimulacija rasporedivanja spremnika\n");

	if(argc != 2) {
        printf("\n\tFormat:    %s velicina_spremnika\n", argv[0]);
        exit(1);
    }
	
	*velicina_spremnika = atoi(argv[1]);

	// Rezerviranje memorije kojoj cemo pristupati
	mem = (int *) malloc(*velicina_spremnika * sizeof(int));
	if(mem == NULL) {
		printf("\n\tGreska prilikom rezerviranje zajednickog spremnika\n");
        exit(1);
	}

	printf("\n\tPocetno stanje spremnika\n");
	
	isprazni_spremnik(mem, *velicina_spremnika);
	ispisi_stanje(mem, *velicina_spremnika);

	srand(time(NULL));

	printf("\n\tZa stvaranje novog zahtjeva za spremnikom pritisnite Z/z\n");
	printf("\n\tZa oslobadanje postojeceg zahtjeva pritisnite O/o\n");
	printf("\n\tZa pregledavanje trenutnog stanja spremnika pritisnite T/t\n");
	printf("\n\tZa prekid programa i oslobadanje memorije pritisnite Q/q\n");

	printf("\nUnesite naredbu => ");

	for(;;) {

		naredba = getchar();
		
		if(naredba == 'Z' || naredba == 'z') {
			// Stvaranje novog zahtjeva
			tmp = slucajan_broj(2, 5);
			printf("\n\tNovi zahtjev br. %d za %d mjesta spremnika\n", *pZahtjev, tmp);

			napuni_spremnik(mem, *velicina_spremnika, pZahtjev, tmp);
			ispisi_stanje(mem, *velicina_spremnika);

			printf("\nUnesite naredbu => ");


		} else if (naredba == 'O' || naredba == 'o') {
			// Oslobadanje zahtjeva
			printf("\n\tUnesite zahtjev koji zelite osloboditi => ");
			scanf(" %c", &oslobadanje);

			oslobodi_zahtjev(mem, *velicina_spremnika, oslobadanje);
			ispisi_stanje(mem, *velicina_spremnika);

			printf("\nUnesite naredbu => ");

		} else if (naredba == 'T' || naredba == 't') {
			// Ispis trenutnog stanja spremnika
			ispisi_stanje(mem, *velicina_spremnika);

			printf("\nUnesite naredbu => ");


		} else if (naredba == 'Q' || naredba == 'q') {
			// Oslobadanje memorije, izlaz iz programa
			free(mem);
			exit(1);

		}

	}


    return 0;
}


void isprazni_spremnik(int *spremnik, int velicina_spremnika) {
	char tmp = '-';
	for(int i = 0; i < velicina_spremnika; i++ ) {
		spremnik[i] = tmp;
	}
}

int provjeri_spremnik(int *spremnik, int velicina_spremnika, int velicina_zahtjeva) {

	int tmp = 0;
	for(int i = 0; i < velicina_spremnika; i++) {
		if(spremnik[i] == '-' ) tmp++;
	}

	if(tmp > 0) return 1;
	else return 0;

}

void ispisi_stanje(int *spremnik, int velicina_spremnika) {

	int tmp = velicina_spremnika;
	
	printf("\n\t**** SPREMNIK ****");
	printf("\n\t");
	for(int i = 0; i < tmp; i++) {
		printf("%d", i%10);
	}

	printf("\n\t");
	for(int i = 0; i < tmp; i++) {
		printf("%c", spremnik[i]);
	}
	printf("\n\n");

}

void napuni_spremnik(int *spremnik, int velicina_spremnika, int *zahtjev, int velicina_zahtjeva) {

	if(provjeri_spremnik(spremnik, velicina_spremnika, velicina_zahtjeva)) {
		
		if(*zahtjev == 10) *zahtjev = 0;
		char tmp = *zahtjev;
		struct Rupa rupa;
		int br = 0;

		rupa = provjeri_slobodno(spremnik, velicina_spremnika, velicina_zahtjeva);
		//printf("dobivena rupa ==> %d, %d\n", rupa.id, rupa.num);

		/*if(rupa.id == -1) {
			printf("\n\tNema dovoljno prostora u spremniku\n");
			return;
		}*/

		printf("\n\tPunjenje spremnika zahtjevom %d\n", *zahtjev);
		
		while(br != velicina_zahtjeva) {
			spremnik[rupa.id] = tmp + '0';
			rupa.id++;
			br++;
		}
		/*for(int i = rupa.id; i < velicina_zahtjeva; i++ ) {
			spremnik[rupa.id] = tmp + '0';
		}*/

		*zahtjev += 1;
	
	} else {
		printf("\n\tSpremnik pun\n");
	}

}

void oslobodi_zahtjev(int *spremnik, int velicina_spremnika, int zahtjev) {

	printf("\n\tOslobadanje zahtjeva %d\n", zahtjev);

	for(int i = 0; i < velicina_spremnika; i++) {
		if(spremnik[i] == zahtjev) spremnik[i] = '-'; 
	}

}

int slucajan_broj(int d, int g) {
   return rand()%(g-d+1)+d;
}

struct Rupa provjeri_slobodno(int *spremnik, int velicina_spremnika, int velicina_zahtjeva) {

	struct Rupa rupe[50], min;
	
	for(int i = 0; i < 50; i++) {
		rupe[i].id = 0;
		rupe[i].num = 0;
	}

	int id = 0, min_broj = 20, j = 0, br = 0;

	while( br != velicina_spremnika ) {
		if(spremnik[br] == '-') {
			rupe[j].id = br;
			while(spremnik[br] == '-') {
				rupe[j].num++;
				br++;
			}
			if(br == velicina_spremnika) break;
			j++;
		}
		br++;
	}
	
	
	/*printf("\n\tPopis rupa =====> \n");
	for(int i = 0; i < velicina_spremnika; i++) {
		printf("\nID => %d, NUM => %d\n", rupe[i].id, rupe[i].num);
	}*/

	for(int i = 0; i < velicina_spremnika; i++) {
		if(rupe[i].num >= velicina_zahtjeva) {
			if(rupe[i].num <= min_broj) {
				min.num = rupe[i].num;
				min.id = rupe[i].id;
				min_broj = rupe[i].num;
			}
		} else continue;
	}

	return min;
}