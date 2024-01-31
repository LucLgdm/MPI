#include <iostream>
#include <mpi.h>
	/************************************ 
	* Echange de type de données dérivés
	* Matrice triangulaire
	*************************************/

	/** On crée 1 matrice A sur les processus 0 et 1
	* On les initialise : nombre croissant positif sur 0,
	* nombre décroissant négatif sur 1.
	* On construit le type de donnée matrice triangulaire
	* supérieur pour 0 et inférieur pour 1
	* On envoie et échange les matrices triangulaires
	* Libère les ressources et quitte MPI.
	*/
int main(int argc, char argv[]){
	int rang, etiquette = 100, signe = 1;
	const int n = 8;
	float a[n][n];
	MPI_Status statut;
	MPI_Datatype typeTriangle;
	int longueurBloc[n], deplacement[n];

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rang);

	// Initialisation de la matrice sur chaque processus
	if (rang == 1) signe = -1;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			a[i][j] = signe * (i * n + j + 1);
		}
	}

	// Création des types de données
	if (rang == 1) {
		for (int i = 0; i < n; i++) {
			longueurBloc[i] = i;
			deplacement[i] = i * n;
		}
	}
	else {
		for (int i = 0; i < n; i++) {
			longueurBloc[i] = n - 1 - i;
			deplacement[i] = i * (n + 1) + 1;
		}
	}
	MPI_Type_indexed(n, longueurBloc, deplacement, MPI_FLOAT, &typeTriangle);
	MPI_Type_commit(&typeTriangle);

	// Echange des matrices
	MPI_Sendrecv_replace(a, 1, typeTriangle, (rang + 1) % 2, etiquette, (rang + 1) % 2, etiquette, MPI_COMM_WORLD, &statut);

	// Libération des types
	MPI_Type_free(&typeTriangle);
	MPI_Finalize();
}
	
