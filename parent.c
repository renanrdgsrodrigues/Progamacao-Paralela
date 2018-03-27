/*
Bruno Bacelar Abe           9292858
Eduardo Kiyoshi Kamikabeya  9778536
Marcelo Tabacnik           	7573232
Renan Rodrigues          	9278132

to compile: mpicc command.c -o children -lm -Wall -fopenmp
to compile: mpicc parent.c -o main -lm -Wall
to run: mpirun -np 1 main
*/

#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"
#include <math.h>

FILE* openMatrixFile ();
void readMatrixFile (FILE*, double**, int);
void printMatrix (double**, int);
int getOrder (FILE*);
double** allocMatrix (int);
double** allocNewMatrix (int count);

FILE* openVectorFile ();
void readVectorFile (FILE*, double*, int);
void printVector (double*, int);
double* allocVector (int);

double **joinVectorMatrix(double **matrix, double *vector, int size);
double *joinVectorMatrix2(double **matrix, double *vector, int size);

int main(int argc, char **argv)
{
	MPI_Init(&argc, &argv);
	int my_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	if(my_rank == 0){
		FILE *fp = NULL;
		int i = 0, order = 0;
		double *vector = NULL, **matrixAux = NULL, *matrix = NULL;
		//double matrix[3][4] = {{5, 0, 2, 5}, {1, 2, 1, 6}, {6, 4, 3, 7}};
	
		fp = openMatrixFile ();
		order = getOrder(fp);
		order = (int)sqrt(order);
		matrixAux = allocMatrix (order);
		readMatrixFile (fp, matrixAux, order);	
		
		fp = openVectorFile ();
		vector = allocVector (order);
		readVectorFile (fp, vector, order);

		matrix = joinVectorMatrix2(matrixAux, vector, order);

		//printVector(matrix,order*(order+1));

		int err[order];
		double answear[order];

		MPI_Comm children;
		MPI_Comm_spawn("children", MPI_ARGV_NULL, order, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &children, err);
				
		MPI_Bcast(&order, 1, MPI_INT, MPI_ROOT, children);
		MPI_Scatter(matrix, order+1, MPI_DOUBLE, NULL, order+1, MPI_DOUBLE, MPI_ROOT, children);
		MPI_Gather(NULL, 1, MPI_DOUBLE, &answear, 1, MPI_DOUBLE, MPI_ROOT, children);
		

		fp = fopen("resultado.txt", "w");
		if(fp==NULL)
		{
			//printf("Deu ruim\n");
			exit(0);
		}

		for(i = 0; i < order; i++){
			fprintf(fp, "%.3f\n", answear[i]);
		}
		fclose(fp);
	}

	MPI_Finalize();
	return 0;
}

double **joinVectorMatrix(double **matrix, double *vector, int order)
{
	int i = 0, j = 0;
	double **newMatrix = NULL;
	
	newMatrix = allocNewMatrix(order+1);

	for(i = 0; i < order; i++){
		for(j = 0; j < order; j++){
			newMatrix[i][j] = matrix[i][j];		
		}
	}


	int neworder = order - 1;

	while(neworder >= 0){
		newMatrix[neworder][order] = vector[neworder];
		neworder--;	
	}
	return (newMatrix);
}

double *joinVectorMatrix2(double **matrix, double *vector, int order)
{
	int i = 0, j = 0;
	double *newMatrix = NULL;
	
	newMatrix = allocVector((order+1)*order);

	for(i = 0; i < order; i++){
		for(j = 0; j < order; j++){
			newMatrix[i*(order+1)+j] = matrix[i][j];		
		}
	}


	int neworder = order - 1;

	while(neworder >= 0){
		newMatrix[neworder*(order+1)+(order)] = vector[neworder];
		neworder--;	
	}
	return (newMatrix);
}

int getOrder (FILE* fp) 
{
	int aux, count;
	
	while (fscanf (fp, "%d", &aux) != EOF){		
		count++;
	}
	
	rewind (fp);

	return count;
}

void printMatrix (double** matrix, int count) 
{
	int i, j;

	for (i = 0; i < count; i++) {
		for (j = 0; j < count; j++) {
			printf("%lf ", matrix[i][j]);
		}
		printf("\n");
	}

	printf("\n\n");
}

void freeAll (FILE* fp, double** matrix, double* vector, int count) 
{
	int i = 0;

	fclose (fp);
	free (vector);
	
	for (i = 0; i < count; i++) free (matrix[i]);
		free (matrix);
}


void readMatrixFile (FILE* fp, double** matrix, int count)
{
	int i = 0, j = 0;
	char nop;

	for (i = 0; i < count; i++) {
		for (j = 0; j < count; j++) {
			fscanf (fp, "%lf", &matrix[i][j]);
			nop = fgetc (fp);
		}
	}	
}


FILE* openMatrixFile () 
{
	FILE* fp = NULL;

	fp = fopen ("matriz.txt", "r");
	if (fp == NULL) {
		//printf ("Erro ao abrir oarquivo 'matrix'.\n");
		exit(0);
	}

	return fp;
}


double** allocMatrix (int count)
 {
	double** matrix = NULL;
	int i;

	matrix = (double**) malloc(sizeof (double*) * count);
	if (matrix == NULL) {
		//printf ("Erro ao alocar a linha da matriz.\n");
		
		return (double**)0;
	}

	for (i = 0; i < count; i++) {
		matrix[i] = (double*) malloc(sizeof (double) * count);
		if (matrix[i] == NULL) {
			//printf ("Erro ao alocar a coluna (%d) da matriz.\n", i);
			
			return (double**)0;
		}
	}

	return matrix;
}

double** allocNewMatrix (int count)
 {
	double** matrix = NULL;
	int i;

	matrix = (double**) malloc(sizeof (double*) * count);
	if (matrix == NULL) {
		//printf ("Erro ao alocar a linha da matriz.\n");
		
		return (double**)0;
	}

	for (i = 0; i < count; i++) {
		matrix[i] = (double*) malloc(sizeof (double) * count+1);
		if (matrix[i] == NULL) {
			//printf ("Erro ao alocar a coluna (%d) da matriz.\n", i);
			
			return (double**)0;
		}
	}

	return matrix;
}
/*******************************************************/
void readVectorFile (FILE* fp, double* vector, int size) {
	int i;
	char nop;

	for (i = 0; i < size; i++) {
		fscanf (fp, "%lf", &vector[i]);
		nop = fgetc (fp);
	}
	
	fclose (fp);
}

double* allocVector (int size) 
{
	double* vector = NULL;

	vector = (double*) calloc(size, sizeof(double));
	if (vector == NULL) {
		//printf ("Erro ao alocar o 'vector'.\n");
		return (double*)0;
	}

	return vector;
}


FILE* openVectorFile () 
{
	FILE* fp = NULL;

	fp = fopen ("vetor.txt", "r");
	if (fp == NULL) {
		//printf ("Erro ao abrir o arquivo 'vector'.\n");
		exit (0);
	}

	return fp;
}

void printVector (double* vector, int size) 
{
	int i;

	for (i = 0; i < size; i++) {
		printf ("%lf ", vector[i]);
	}
	printf("\n\n");
}




