//to compile: mpicc command.c -o children -lm -Wall -fopenmp
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "mpi.h"
#include <omp.h>

void print(int **matrix, int M, int N){	
	int i, j;
	for(i = 0; i < M; i++) {
		for(j = 0; j < N; j++)
			printf("%d ", matrix[i][j]);
		printf("\n");
	}
}
void printRow(double row[], int N){
	int i;
	for(i = 0; i < N; i++)
		printf("%.3f ", row[i]);
}

void copyArrayTo(double *source, double *dest, int N){	
	int i;
	for(i = 0; i < N; i++)
		dest[i] = source[i];
}

int main(int argc, char **argv)
{
	int my_rank, num_proc, order;
	int i, k, current_row = 0;	
	
	double pivo, value;
	MPI_Comm parent;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &num_proc);
	MPI_Comm_get_parent(&parent);
	
	MPI_Bcast(&order, 1, MPI_INT, 0, parent);

	double row_array[order+1], send_row[order+1];
	
	MPI_Scatter(NULL, order+1, MPI_DOUBLE, &row_array, order+1, MPI_DOUBLE, 0, parent);
	//printRow(row_array, order);
	//printf("\n");
	
	while(current_row < order){
		if(my_rank == current_row){
			//Divide linha do Pivo, pelo pivo
			pivo = row_array[current_row];
			
			#pragma omp parallel for shared(row_array, pivo) private(i)
			for(i = current_row; i < order+1; i++){
				row_array[i] = row_array[i]/pivo;	
			}
			copyArrayTo(row_array, send_row, order+1);
		}
		
		MPI_Bcast(&send_row, order+1, MPI_DOUBLE, current_row, MPI_COMM_WORLD);
		
		if(my_rank != current_row){
			//Aqui entra OpenMP
			value = row_array[current_row];
			if(value != 0){
				#pragma omp parallel for shared(row_array, send_row, value) private(k)			
				for(k = current_row; k < order+1; k++){
					row_array[k] = row_array[k] - value * send_row[k];
				}
			}			

		}
		
		current_row++;
	}
	
	MPI_Gather(&row_array[order+1 - 1], 1, MPI_DOUBLE, NULL, 1, MPI_DOUBLE, 0, parent);
	MPI_Finalize();
	return 0;
}


