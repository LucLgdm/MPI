#include <iostream>
#include <mpi.h>

/****************************************************************
* Calcul de Pi par intégration numérique : méthode des rectangles
* Pi = 4 * intégrale de 0 à 1 de 1/(1 + x^2)
*****************************************************************/
int main(int argc, char argv[]){

	long long n;
	double h;

	// Interval number
	n = 3 * 1000 * 1000LL * 100; // LL indique qu'il faut considérer le nombre qui précède comme un long long
	// Interval width
	h = 1.0 / n;
	// MPI data
	MPI_Init(&argc, &argv);

	int nb_procs, rang;

	MPI_Comm_size(MPI_COMM_WORLD, &nb_procs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rang);

	int localN = n / nb_procs;
	int start = rang * localN;
	int end = (rang + 1) * localN;

	double localSum = 0.0;
	for (int i = start; i < end; i++) {
		localSum += h * 4.0 / (1 + h * h * (i + 0.5) * (i + 0.5));
	}

	double globalSum;
	MPI_Reduce(&localSum, &globalSum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	// Affichage du résultat par le processus destinataire
	if (rang == 0) {
		cout << "Here is process number 0, Pi is equal to : " << globalSum << endl;
	}

	MPI_Finalize();
}
