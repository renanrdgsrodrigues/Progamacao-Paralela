//compile: mpicc tokenring.c -o token -fopenmp
//run: mpirun -np 3 token

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "mpi.h"


int main (int argc, char** argv) {
	int rank = 0, numProc = 0, recvNum = 0, num = 0;

	
	MPI_Init (&argc, &argv);
	MPI_Comm_size (MPI_COMM_WORLD, &numProc);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);

	if (rank == 0) {
		num = 4;
		MPI_Send (&num, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);	
		MPI_Recv (&recvNum, 1, MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf ("num = %d\trank = %d\n", recvNum, rank);
	}

	if (rank == 1) {
		MPI_Recv (&recvNum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		#pragma parallel omp num_threads (rank)
			recvNum += 4;
		printf ("num = %d\trank = %d\n", recvNum, rank);

		MPI_Send (&recvNum, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
	}

	if (rank == 2) {
		MPI_Recv (&recvNum, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		#pragma parallel omp num_threads (rank)
			recvNum += 4;
		printf ("num = %d\trank = %d\n", recvNum, rank);

		MPI_Send (&recvNum, 1, MPI_INT, 3, 0, MPI_COMM_WORLD);
	}

	if (rank == 3) {
		MPI_Recv (&recvNum, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		#pragma parallel omp num_threads (rank)
			recvNum += 4;
		printf ("num = %d\trank = %d\n", recvNum, rank);

		MPI_Send (&recvNum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}

	//if (rank == 0 ) printf ("\nFINAL = %d\n", recvNum);
	
	MPI_Finalize ();

	return 0;
}
