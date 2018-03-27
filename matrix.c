//compile : mpicc matrix.c -o mat -fopenmp
//run : mpirun -np SIZE mat


/* Encontra quantos elementos entre 9 e 19 tem em uma matriz usando OMP e MPI*/

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "mpi.h"
#include <time.h>

#define SIZE 5


int main (int argc, char** argv) {
	int mat[SIZE][SIZE];
	int line[SIZE], resp[SIZE];
	int i, j, count = 0;
	int rank = 0, numProc = 0, root = 0;

	MPI_Init (&argc, &argv);
	MPI_Comm_size (MPI_COMM_WORLD, &numProc);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);

	if (rank == 0) {
		srand (time(NULL));
		for (i = 0; i < SIZE; i ++) {
			for (j = 0; j < SIZE; j++) {
				mat[i][j] = rand() % 100;
			}
		}

		printf ("\n\n");
		for (i = 0; i < SIZE; i++) {
			for (j = 0; j < SIZE; j++) {
				printf ("%d ", mat[i][j]);
			}
			printf ("\n\n");
		}
	}

	MPI_Scatter (&mat, SIZE, MPI_INT, line, SIZE, MPI_INT, root, MPI_COMM_WORLD);

	#pragma parallel for reduce (+: count)
	//#pragma omp parallel for
	//#pragma omp reduce(+ : count) 
	for ( i = 0; i < SIZE; i++) {
		if (line[i] >= 9 && line[i] <= 19) {
			count++;
		}
	}

	
	MPI_Gather (&count, 1, MPI_INT, resp, 1, MPI_INT, root, MPI_COMM_WORLD);

	if (rank == 0) {
		int aux = 0;

		for (i = 0; i < SIZE; i++) {
			printf ("Resp[%d] = %d\n", i, resp[i]);
			aux += resp[i];
		}

		printf ("Ha %d numeros entre 9 e 19.\n", aux);
	}

	MPI_Finalize ();

	return 0;
}
