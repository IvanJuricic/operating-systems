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
void oslobodi_zahtjev(int *spremnik, int zahtjev);
void isprazni_spremnik(int *spremnik, int velicina_spremnika);
int provjeri_spremnik(int *spremnik, int velicina_spremnika);
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
			oslobadanje = getchar();

			oslobodi_zahtjev(mem, oslobadanje);

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

int provjeri_spremnik(int *spremnik, int velicina_spremnika) {

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

	if(provjeri_spremnik(spremnik, velicina_spremnika)) {
		
		if(*zahtjev == 10) *zahtjev = 0;
		char tmp = *zahtjev;
		struct Rupa rupa;
		
		rupa = provjeri_slobodno(spremnik, velicina_spremnika, velicina_zahtjeva);
		//printf("dobivena rupa ==> %d, %d\n", rupa.id, rupa.num);

		printf("\n\tPunjenje spremnika zahtjevom %d\n", *zahtjev);
		printf("\n\tPunim od pozicije %d.\n", rupa.id);
		
		for(int i = rupa.id; i < velicina_zahtjeva; i++ ) {
			spremnik[i] = tmp + '0';
		}

		*zahtjev += 1;
	
	} else {
		printf("\n\tSpremnik pun\n");
	}

}

void oslobodi_zahtjev(int *spremnik, int zahtjev) {

	printf("\n\tOslobadanje zahtjeva %d\n", zahtjev);

	/*char tmp = *zahtjev;
	if(tmp == 45) {
	
		tmp = '-';
		for(int i = 0; i < velicina_spremnika; i++ ) {
			spremnik[i] = tmp;
		}

	} else {

		for(int i = 0; i < velicina_spremnika; i++ ) {
			spremnik[i] = tmp + '0';
		}

		*zahtjev += 1;
	}*/

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
		}
	}

	return min;
}
/* 

#include<iostream>
#include<malloc.h>
using namespace std;

void printStatus(int *a, int n) {
	int i;
	for (i = 1; i < n + 1; i++) {
		cout << i % 10 << " ";
	}
	cout << "\n";
	for (i = 1; i < n + 1; i++) {
		cout << a[i-1] << " ";
	}
	cout << "\n";
}
void setZeros(int *a, int n) {
	for (int i = 0; i < n; i++) {
		a[i] = 0;
	}
}
/*
1234567890123456
1112000004440005
    000004440005
	l1

int zeros(int *p) {
	int size = 0;
	int *a = p;
	do {
		if (*a == 0) {
			size++;
			a = a + 1;
		}
		else break;
	} while (true);
	return size;
}
void defragment(int *a, int n) {
	//l=&l1	
	int *o;
	int br = 0;
	o= (int*)malloc(sizeof(int) * n);
	setZeros(o, n);
	//1100033344005506
	for (int i = 0; i < n; i++) {
		if (a[i] != 0) {
			o[br] = a[i];
			br++;
		}
	}
	for (int i = 0; i < n; i++)	{
		a[i] = o[i];
	}
	free(o);
}
bool location(int *p,int n, int k, int **l) {
	//  k-size koji zauzimamo, l-lokacija za vracanje
	bool x = false;
	int *l1=NULL;
	int curr,min,all=0;
	min = 100;
	
	for (int  i = 0; i < n; i++){
		if (p[i] == 0) {
			if (!x) {
				curr = zeros(p + i);
				all += curr;
				l1 = p + i;
			}
			x = true;
		}else {
			x = false;
			curr = 0;
		}
		if ((curr < min || min ==0)&& curr!=0) {
			min = curr;
			*l = l1;		
		}
	}
	if (all < k) {
		cout << "nemoze stati\n";
		return false;
	}
	else if (min < k) {
		cout << "Potrebna defragmentacija za defragmentaciju pritisnite D: "<<endl;
		return false;
	}
	else {
		return true;
	}

}


void createNewRequest(int *a, int n) {
	static int reqNum = 1;
	int size;
	int *a1 = a;
	int **l=&a1;// l->l1->0
	cout << "Koliko lokacija zelite zauzet: ";
	cin >> size;
	cout << "\n";
	if (location(a,n, size,l)) {
		for (int i = 0; i < size; i++) {
			*(*l+i) = reqNum;
		}
		reqNum++;
	}

	printStatus(a, n);
	
	
}
void removeRequest(int *a, int n) {
	cout << "Koji zahtjev zelite ukloniti: ";
	int x;
	cin>> x;
	cout << "\n";
	for (int i = 0; i < n; i++) {
		if (a[i] == x)
			a[i] = 0;
	}
	printStatus(a, n);
}


int main() {
	int n;
	int *p;
	cin >> n;
	p = (int *)malloc(sizeof(int) * n);
	setZeros(p, n);
	printStatus(p, n);
	char command;
	while (true) {
		cout << "Unesite zahtjev: ";
		cin >> command;
		cout << "\n";

		switch (command)
		{
		case 'Z':
		case 'z':
			createNewRequest(p, n);
			break;
		case 'O':
		case 'o':
			removeRequest(p, n);
			break;
		case 'D':
		case 'd':
			defragment(p, n);
			printStatus(p, n);
			break;
		case 'K':
		case 'k':
			return 0;
		}
		
	}
	system("PAUSE");
	return 0;
}



*/