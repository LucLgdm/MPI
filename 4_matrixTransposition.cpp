#include <iostream>
#include <mpi.h>

	/**************************
	* Transposition de matrice
	***************************/
	
	/**
	* On veut envoyer une matrice d'un processus à un autre
	* tout en la transposant
	*/
	int main(int argc, char argv[]){
	const int nLigne = 4, nColonne = 5;
	int rang, etiquette = 100, signe = 1;
	int a[nLigne][nColonne];
	int at[nColonne][nLigne];
	MPI_Status statut;
	MPI_Datatype typeColonne, typeTranspose;

	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rang);

	if (rang == 0) {
		// Initialisation sur le processus 0
		for (int i = 0; i < nLigne; i++) {
			for (int j = 0; j < nColonne; j++) {
				a[i][j] = i * (nLigne + 1) + j + 1;
			}
		}
		for (int i = 0; i < nLigne; i++) {
			for (int j = 0; j < nColonne; j++) {
				printf("%4.d ", a[i][j]);
			}
			printf("\n");
		}
		// Création des types typeColonne et typeTranspose

		MPI_Type_vector(nLigne, 1, nColonne, MPI_INT, &typeColonne);
		MPI_Type_commit(&typeColonne);

		MPI_Type_create_hvector(nColonne, 1, sizeof(int), typeColonne, &typeTranspose);
		MPI_Type_commit(&typeTranspose);

		// Envoie de la matrice sur le rang 1

		MPI_Send(&a[0][0], 1, typeTranspose, 1, etiquette, MPI_COMM_WORLD);

		MPI_Type_free(&typeColonne);
		MPI_Type_free(&typeTranspose);
	}
	else {
		MPI_Recv(&at[0][0], nLigne * nColonne, MPI_INT, 0, etiquette, MPI_COMM_WORLD, &statut);

		printf("Matrice transposee at\n");
		for (int i = 0; i < nColonne; i++) {
			for (int j = 0; j < nLigne; j++) {
				printf("%4.d ", at[i][j]);
			}
			printf("\n");
		}
	}

	MPI_Finalize();
	}
