#include <iostream>
#include <mpi.h>

	/***********************************
 * Created by Luc de Mercey, 01.2024
 * Sending simple message
	************************************/


int main(int argc, char argv[]){
	int const nbTest = 9, nbValeurMax = 5000;
	int nb_procs, rang;
	int etiquette = 10;
	int nbValeur[nbTest] = {0, 1, 10, 100, 1000, 2000, 3000, 4000, 5000};
	int valeur[nbValeurMax];
	MPI_Status statut;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nb_procs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rang);
	
	for (int i = 0; i < nbTest; i++) {
		
		cout << endl << endl;
		if (rang == 0) {
			for (int j = 0; j < nbValeurMax; j++) {
				valeur[j] = rand()/(RAND_MAX + 1.);
			}
			double tempsStart = MPI_Wtime();
			MPI_Send(valeur, nbValeur[i], MPI_INT, 1, etiquette, MPI_COMM_WORLD);
			MPI_Recv(&valeur, nbValeur[i], MPI_INT, 1, etiquette, MPI_COMM_WORLD, &statut);
			double tempsEnd = MPI_Wtime();

			cout << "..... " << nbValeur[i] << " ont ete envoyees et recues entre 0 et 1 " << endl;
			cout << "..... " << "L'envoie de 0 vers 1 a dure : " << tempsEnd - tempsStart << " seconde" << endl;
			cout << "..... " << "Le debit total est de : " << 2 * nbValeur[i] * 8 / 1000000. / (tempsEnd - tempsStart) << " Mo/s" << endl;
		}
		else if (rang == 1) {
			for (int j = 0; j < 1000; j++) {
				valeur[j] = rand() / (RAND_MAX + 1.);
			}
			double tempsStart = MPI_Wtime();
			MPI_Send(valeur, nbValeur[i], MPI_INT, 0, etiquette, MPI_COMM_WORLD);
			MPI_Recv(&valeur, nbValeur[i], MPI_INT, 0, etiquette, MPI_COMM_WORLD, &statut);
			double tempsEnd = MPI_Wtime();

			cout << nbValeur[i] << " ont ete envoyees et recues entre 0 et 1 " << endl;
			cout << "L'envoie de 0 vers 1 a dure : " << tempsEnd - tempsStart << " seconde" << endl;
			cout << "Le debit total est de : " << 2 * nbValeur[i] * 8 / 1000000. / (tempsEnd - tempsStart) << " Mo/s" << endl;
		}
	}
	MPI_Finalize();
}
