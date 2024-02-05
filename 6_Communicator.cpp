#include <iostream>
#include <cstdlib>
#include <mpi.h>

using namespace std;

#define _TAG 1

/**************************************************************
 * The goal is to create 2 communicators and to share
 * information in each from a "personal" process
 * WARNING : there must be 8 process
***************************************************************/

int main(int argc, char** argv) {
	int nbProcess, rank, w, color;
	int v[4], *key;
	MPI_Comm commEvenOdd = MPI_COMM_NULL;

	nbProcess = 8;
	key = (int*) malloc(sizeof(int) * nbProcess);

	for (int i = 0; i < nbProcess; i++) {
		key[i] = i;
	}


	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);


	for (int i = 0; i < 4; i++) {
		v[i] = 0;
	}
	/* In order not to use the modulo operation, we work with bytes */
	int p = rank << 63;
	if ( p == 0) {
		color = 0;
	}
	else {
		color = 1;
	}
	/* The partition is created */
	MPI_Comm_split(MPI_COMM_WORLD, color, key[rank], &commEvenOdd);
	MPI_Comm_rank(commEvenOdd, &rank);

	if (rank == 1) {
		for (int i = 0; i < 4; i++) {
			v[i] = i + 1;
		}
	}
	MPI_Bcast(v, 4, MPI_INT, 1, commEvenOdd);
	
	w = rank + 1;
	for (int j = 0; j < nbProcess; j++) {
		if (rank == j) {
			cout << "I'm process number : " << rank << "    w = " << w << "    v = ";
			for (int i = 0; i < 4; i++) {
				cout << v[i] << " ";
			}
		}
	}

	free(key);
	MPI_Finalize();
	return EXIT_SUCCESS;
}
