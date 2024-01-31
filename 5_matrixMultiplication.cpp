#include <iostream>
#include <cstdlib>
#include <mpi.h>

using namespace std;

#define _Row_tag 0
#define _Column_tag 1
#define _C_diag_tag 2


// Global variables used in parallelization
int myRank;                                 // MPI : Process number
int nProcesses;                             // MPI : number of total process
MPI_Status status;                          // MPI : store status for MPI_Recv
MPI_Datatype columnType;                    // MPI : new type of data

int main(int argc, char** argv) {

    if (argv[1] == NULL) {
        cout << "ERROR: The program must be executed in the following way  \n\n  \t \"mpirun -n NumberOfProcesses mpi N \"  \n\n where NumberOfProcesses = N + 1. \n \n " << endl;
        return 1;
    }

    int n = atoi(argv[1]);
    int* A, * B, * C;

    // Memory allocation for the matrix
    A = (int*)malloc(n * n * sizeof(int*));
    B = (int*)malloc(n * n * sizeof(int*));
    C = (int*)malloc(n * n * sizeof(int*));


    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    MPI_Type_vector(n, 1, n, MPI_INT, &columnType);
    MPI_Type_commit(&columnType);

    // We send to each process a row of the matrix A and a coloumn of the matrix B
    if (myRank == 0) { // 0 is the master, it initializes work
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                A[i * n + j] = i + j;
                B[i * n + j] = i + j + 1;
                C[i * n + j] = 0;
            }
        }

        for (int i = 1; i < nProcesses; i++) {
            // Send the of the i-row of the matrix A to the i-process
            MPI_Send(&A[(i - 1) * n], n, MPI_INT, i, _Row_tag, MPI_COMM_WORLD);

            // Send the of the i-column of the matrix B to the i-process
            MPI_Send(&B[i - 1], 1, columnType, i, _Column_tag, MPI_COMM_WORLD);
        }
    }

    if (myRank > 0) {
        // Recieve the corresponding row
        MPI_Recv(&A[(myRank - 1) * n], n, MPI_INT, 0, _Row_tag, MPI_COMM_WORLD, &status);

        // Recieve the corresponding colum
        MPI_Recv(&B[myRank - 1], 1, columnType, 0, _Column_tag, MPI_COMM_WORLD, &status);

        // Calculation of the diagonal terme in C
        C[(myRank - 1) * n + (myRank - 1)] = 0;

        for (int k = 0; k < n; k++) {
            C[(myRank - 1) * n + (myRank - 1)] += A[(myRank - 1) * n + k] * B[k * n + (myRank - 1)];
        }

        MPI_Send(&C[(myRank - 1) * n + myRank - 1], 1, MPI_INT, 0, _C_diag_tag, MPI_COMM_WORLD);
    }
    if (myRank == 0) {
        for (int i = 1; i < nProcesses; i++) {
            MPI_Recv(&C[(i - 1) * n + i - 1], 1, MPI_INT, i, _C_diag_tag, MPI_COMM_WORLD, &status);
            MPI_Send(&A[0], n * n, MPI_INT, i, 100, MPI_COMM_WORLD);
            /** At first I wanted to use MPI_Allgather() :
            * Each processus would have send their row of A to every other process.
            * Unfortunatly I didn't succeed...
            **/
        }
    }

    if (myRank > 0) {
        MPI_Recv(&A[0], n * n, MPI_INT, 0, 100, MPI_COMM_WORLD, &status);

        for (int i = 0; i < n; i++) {
            if (i != (myRank - 1)) { // Diagonal terme is already calculated
                C[i * n + myRank - 1] = 0;
                for (int k = 0; k < n; k++) {
                    C[i * n + myRank - 1] += A[i * n + k] * B[k * n + myRank - 1];
                }
            }
        }
        MPI_Send(&C[myRank - 1], 1, columnType, 0, _C_diag_tag, MPI_COMM_WORLD);
    }

    if (myRank == 0) {
        for (int i = 1; i < nProcesses; i++) {
            MPI_Recv(&C[i - 1], 1, columnType, i, _C_diag_tag, MPI_COMM_WORLD, &status);
        }

        cout << "\n\nMatrix A" << endl;
        for (int i = 0; i < n; i++) {
            cout << endl << endl;
            for (int j = 0; j < n; j++) {
                cout << A[i * n + j] << " ";
            }
        }
        cout << "\n\n\n\nMatrix B" << endl;
        for (int i = 0; i < n; i++) {
            cout << endl << endl;
            for (int j = 0; j < n; j++) {
                cout << B[i * n + j] << " ";
            }
        }
        cout << "\n\n\n\nMultiplied Matrix C" << endl;
        for (int i = 0; i < n; i++) {
            cout << endl << endl;
            for (int j = 0; j < n; j++) {
                cout << C[i * n + j] << " ";
            }
        }
    }
    MPI_Type_free(&columnType);

    MPI_Finalize();
    free(A);
    free(B);
    free(C);

    return EXIT_SUCCESS;
}
