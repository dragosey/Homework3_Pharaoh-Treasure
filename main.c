//##########################################################\\
//															\\
//		TEMA 3, SDA, 313AB									\\
//															\\
//	Author: Dragos-Nicolae Sandu, dragos.n.sandu@gmail.com 	\\
//															\\
//##########################################################\\

#include <stdlib.h>
#include <stdio.h>
#define numar_rezultate 4
//MODULATOR = 2^31 - 1
#define MODULATOR 2147483647
#define MULTIP 1103515245
#define INCREMENT 12345
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

//Variabile globale	
int nivel_matrice;
long long suma_max_admisa;
int nr_max_incercari;
long long **harta=NULL;
long long results[numar_rezultate];
int cost = 0;
int contor_drumuri = 0;

//functie pentru generarea random a directiei de deplasare
unsigned int seed = 0;
int rand() {
		seed = (MULTIP * seed + INCREMENT) % MODULATOR;
		return seed;
	}

void backtracking (long long **harta, int nivel_matrice, int i, int j, int cost)
{
	//definim posibilitatile de deplasare: stanga si in sus
	static int dirI[] = {0, -1};
	static int dirJ[] = {-1, 0};
	int d;

	//daca am ajuns pe pozitia [0][0] in matrice 
	if( i == 0 && j == 0)
	{
		//adunam la cost si valoarea de pe pozitia [0][0]
		cost += harta[i][j];

		//verificam daca suma este mai mica sau egala decat suma maxima admisa si, in caz afirmativ, incrementam contorul drumurilor
		if( cost <= suma_max_admisa)
			contor_drumuri++;
	}

	else
	{
		for( d = 0 ; d < 2 ; d++)
		{
			//ne asiguram ca noile valori i si j nu parasesc matricea si nu iau valori negative
			int	newI = min(nivel_matrice - 1, max(i + dirI[d], 0));
			int	newJ = min(nivel_matrice - 1, max(j + dirJ[d], 0));

			//daca noile valori i si j sunt diferite de cele actuale, aplicam recursiv functia pe noile pozitii i si j
			if(newI != i || newJ != j)
			{
				cost = cost + harta[i][j];

				backtracking( harta, nivel_matrice, newI, newJ, cost);

				cost = cost - harta[i][j];
			}
		}
	}
}

void random_optimul_local()
{
	long long suma;
	int i,j;
	int contor_nr_incercari;

	//incercam de atatea ori cate sanse (incercari) avem
	for( contor_nr_incercari = 1; contor_nr_incercari <= nr_max_incercari ; contor_nr_incercari++ )
	{
		//initializam suma cu valoarea aflata in coltul din dreapta jos al matricei
		suma = harta[nivel_matrice-1][nivel_matrice-1];

		i = nivel_matrice-1;
		j = nivel_matrice-1;

		while(i != 0 && j != 0)
		{
			//folosind functia rand se hotaraste "aleator" directia de deplasare - stanga sau dreapta si se adauga la suma elementul corespunzator
			if(rand() % 2)
			{
				suma += harta[i-1][j];
				i--;
			}

			else
			{
				suma += harta[i][j-1];
				j--;
			}
		}

		while(i != 0 || j != 0)
		{
			//daca am ajuns pe prima linie, adunam pe rand elementele de pe aceasta la suma
			if(i == 0)
			{
				suma += harta[i][j-1];
				j--;
			}

			//daca am ajuns pe prima coloana, adunam pe rand elementele de pe aceasta la suma
			else if(j == 0)
			{
				suma += harta[i-1][j];
				i--;
			}
		}

		//daca suma obtinuta este mai mica sau egala decat suma maxima admisa scriem in vectorul de solutii numarul de incercari pe care le-am facut si ne oprim
		if(suma <= suma_max_admisa)
		{
			results[2] = contor_nr_incercari;
			break;
		}
	}

	//daca am trecut prin toate incercarile si nu am obtinut o suma mai mica decat suma maxima admisa, punem in vectorul de solutii valoarea 0
	if(contor_nr_incercari > nr_max_incercari)
		results[2] = 0;
}

void optimul_local()
{
	long long suma = harta[nivel_matrice-1][nivel_matrice-1];
	int i = nivel_matrice-1;
	int j = nivel_matrice-1;
	
	//cat timp i si j nu sunt in acelasi timp 0, deci nu am ajuns in coltul din stanga sus al matricei
	while(i != 0 || j != 0)
	{
		//daca am ajuns pe prima linie, adunam la suma toate elementele de pe acea linie, deoarece nu mai exista alte optiuni de parcurgere
		if(i == 0)
		{
			suma = suma + harta[i][j-1];
			j--;
		}

		//daca am ajuns pe prima coloana, adunam la suma toate elementele de pe acea coloana, deoarece nu mai exista alte optiuni de parcurgere
		else if(j == 0)
		{
			suma = suma + harta[i-1][j];
			i--;
		}

		//daca nu suntem pe prima linie sau prima coloana se va adauga la suma valoarea cea mai mica dintre cele doua directii de deplasare, stanga sau sus (Greedy). Conform cerintei, algoritmul prefera sa mearga in sus in caz de egalitate
		else if(i>0 && j>0 && harta[i-1][j] <= harta[i][j-1])
		{
			suma = suma + harta[i-1][j];
			i--;
		}

		else if(i>0 && j>0 && harta[i-1][j] > harta[i][j-1])
		{
			suma = suma + harta[i][j-1];
			j--;
		}
	}

	//suma obtinuta este trecuta in vectorul de valori
	results[0] = suma;
}

void pd()
{
	int i, j;
	long long suma = 0;
	long long **matrice_cost = NULL;
	
	//alocarea matricei ajutatoare pe baza careia se calculeaza drumul cu costul cel mai mic
	matrice_cost = (long long**)malloc(nivel_matrice * sizeof(long long*));
	for(i = 0; i < nivel_matrice; i++)
		matrice_cost[i] = (long long*)malloc(nivel_matrice * sizeof(long long));

	//se calculeaza valorile de pe ultima linie a matricei, valorile reprezentand costul necesar pentru a ajunge pe respectivele pozitii din matrice plecand din coltul din dreapta jos
	for(i = nivel_matrice-1; i >= 0; i--)
	{
		matrice_cost[nivel_matrice-1][i] = suma + harta[nivel_matrice-1][i];
		suma = matrice_cost[nivel_matrice-1][i];
	}

	//reinitializez suma cu 0 si incepem sa formam si ultima coloana a matricei
	suma = 0;

	for(i = nivel_matrice-1; i>= 0; i--)
	{
		matrice_cost[i][nivel_matrice-1] = suma + harta[i][nivel_matrice-1];
		suma = matrice_cost[i][nivel_matrice-1];
	}

	//restul de pozitii din matrice vor fi completate prin compararea costurilor a "drumurilor" pana la pozitia respectiva din matrice. Se va alege minimul dintre cele doua posibilitati
	for(i = nivel_matrice-2; i>= 0; i--)
	{
		for(j = nivel_matrice-2; j>= 0; j--)
			matrice_cost[i][j] = harta[i][j] + min(matrice_cost[i+1][j],matrice_cost[i][j+1]);
	}

	//valoarea costului cel mai mic este reprezentata de valoarea obtinuta in pozitia [0][0] a matricei de costuri si ea se va scrie in vectorul de solutii
	results[1] = matrice_cost[0][0];

	//eliberez memoria folosita de matricea ajutatoare de costuri
	for(i = 0; i < nivel_matrice; i++)
		free(matrice_cost[i]);
	free(matrice_cost);
}

void get_values(char *valuesin)
{
	int i,j;
	int C1, C2, C3, C4;
	FILE *filein;

	if((filein = fopen(valuesin,"r")) == NULL)
	{
		printf("Fisierul %s nu a putut fi deschis", valuesin);
		exit(1);
	}

	//se preiau din fisierul sursa valorile celor 4 cerinte, nivelul matricei si suma maxima admisa
	fscanf(filein, "%d %d %d %d\n" , &C1, &C2, &C3, &C4);
	fscanf(filein, "%d\n" , &nivel_matrice);
	fscanf(filein, "%lld\n" , &suma_max_admisa);

	//alocarea matricei pe care se vor executa toate cerintele
	harta = (long long**)malloc(nivel_matrice * sizeof(long long*));
	for(i = 0; i < nivel_matrice; i++)
		harta[i]=(long long*)malloc(nivel_matrice * sizeof(long long));

	//se preiau din fisierul sursa elementele din matrice
	for(i = 0; i < nivel_matrice; i++)
	{
		for(j = 0; j < nivel_matrice; j++)
			fscanf(filein, "%lld ", &harta[i][j]);
	}

	//se preia din fisierul sursa numarul maxim de incercari
	fscanf(filein,"%d\n", &nr_max_incercari);
	
	fclose(filein);


	//se verifica, pe rand, care dintre cerinte trebuiesc executate si se apeleaza functiile corespunzatoare.
	//in cazul in care una dintre cerinte nu este ceruta, valoarea corespunzatoare in vectorul de solutii va fi -1.
	if(C1 == 1)
	 	optimul_local();
	else
		results[0] = -1;

	if(C2 == 1)
		pd();
	else
		results[1] = -1;
	
	if(C3 == 1)
		random_optimul_local();
	else
		results[2] = -1;

	if(C4 == 1)
	{
		backtracking(harta,nivel_matrice,nivel_matrice-1,nivel_matrice-1,cost);
		results[3] = contor_drumuri;
	}
	else
		results[3] = -1;

	//eliberez memoria folosita de matrice
	for(i = 0; i < nivel_matrice; i++)
		free(harta[i]);
	free(harta);
}

void results_puts(char *valuesout)
{
	FILE *fileout;
	int i;

	if((fileout = fopen(valuesout,"w")) == 0)
	{
		printf("Fisierul %s nu poate fi deschis", valuesout);
		exit(1);
	}

	//se scriu in fisierul de output valorile din vectorul de solutii, verificandu-se ca valoarea sa nu fie -1
	for(i = 0; i < numar_rezultate; i++)
	{
		if( results[i] != -1)
			fprintf(fileout, "%lld\n" , results[i]);
	}

	fclose(fileout);
}

int main(int argc,char *argv[])
{
	get_values(argv[1]);
	results_puts(argv[2]);
	return 0;
}