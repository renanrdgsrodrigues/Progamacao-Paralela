#include "main.h"

/*******************************************************************************************************************/
FILE* openFile (char* name) {
	FILE* fp = NULL;

	fp = fopen (name, "r");
	if (fp == NULL) {
		printf ("Erro ao abrir oarquivo 'matrix'.\n");
		exit(0);
	}

	return fp;
}

void getSize (FILE* fp, int* count) {
	int aux;
	char nop;

	do{
		fscanf (fp, "%d", &aux);
		(*count)++;

		nop = fgetc (fp);
	}while (nop != '\n');

	rewind (fp);
}

double** allocMatrix (int count) {
	double** matrix = NULL;
	int i;

	matrix = (double**) malloc(sizeof (double*) * count);
	if (matrix == NULL) {
		printf ("Erro ao alocar a linha da matriz.\n");
		
		return (double**)0;
	}

	for (i = 0; i < count; i++) {
		matrix[i] = (double*) malloc(sizeof (double) * (count + 1)); //1 coluna para o 'vector'
		if (matrix[i] == NULL) {
			printf ("Erro ao alocar a coluna (%d) da matriz.\n", i);
			
			return (double**)0;
		}
	}

	return matrix;
}

void readMatrixFile (FILE* fp, double** matrix, int count) {
	int aux, i = 0, j = 0;
	char nop;

	for (i = 0; i < count; i++) {
		for (j = 0; j < count; j++) {
			fscanf (fp, "%lf", &matrix[i][j]);
			nop = fgetc (fp);
		}
	}	

	fclose (fp);
}

/******************************************************************************************************************************/
void readVectorFile (FILE* fp, double** matrix, int count) {
	int i;
	int nop;

	for (i = 0; i < count; i++) {
		fscanf (fp, "%lf", &matrix[i][count]); 
		nop = fgetc (fp);
	}	

	fclose (fp);
}

/****************************************************************************************************************************/
double* allocResult (int count) {
	double* result = NULL;

	result = (double*)malloc(sizeof(double) * count);
	if (result == NULL) {
		printf ("Erro ao alocar o 'result'.\n");
		return (double*)0;
	}

	return result;
}

void writeResultFile (FILE* fp, double* result, int count) {
	int i;

	for (i = 0; i < count; i++) {
		fprintf (fp, "%.3lf\n", result[i]);
	}
	printf("\n\n");
}

/*****************************************************************************************************************************/
double* allocVectorColumn (int count) {
	double* vector = NULL;

	vector = (double*)malloc(sizeof(double) * count);
	if (vector = NULL) {
		printf ("Erro ao alocar o 'vector column'.\n");
		return (double*)0;
	}

	return vector;
}

void mpiProcess (int argc, char** argv, double** matrix, double* result, double* row_array, double* send_row, int count) {
	int current_row = 0;
	int pivo, value, num_proc, my_rank;
	int i;

	MPI_Init (&argc, &argv);
	MPI_Comm_rank (MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size (MPI_COMM_WORLD, &num_proc);

	MPI_Scatter (&matrix, (count + 1), MPI_DOUBLE, &row_array, (count + 1), MPI_DOUBLE, 0, MPI_COMM_WORLD);

	while (current_row < count) {
		if (my_rank == current_row) {
			pivo = row_array[current_row];
			for (i = current_row; i < (count + 1); i++) row_array[i] /= pivo;
			
			copyArrayTo (row_array, send_row, count);
		}	

		MPI_Barrier (MPI_COMM_WORLD);
		MPI_Bcast (&send_row, (count + 1), MPI_DOUBLE, current_row, MPI_COMM_WORLD);

		if (my_rank != current_row) {
			value = row_array[current_row];
			for (i = current_row; i < (count + 1); i++) row_array[i] -= value * send_row[i];
		}

		current_row++;
	}

	printf ("\n");
	for (i = 0; i < (count + 1); i++) printf ("%.3lf processo: %d\n", row_array[i], my_rank);

	MPI_Gather (&row_array[count], 1, MPI_DOUBLE, &result, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	
	if (my_rank == 0) {
		for (i = 0; i < count; i++) printf ("%.3lf\n", result[i]);
	}
}

void copyArrayTo (double* source, double* dest, int count) {
	int i;

	for (i = 0; i < (count + 1); i++) dest[i] = source[i];
}

/****************************************************************************************************************************/
void printMatrix (double** matrix, int count) {
	int i, j;

	for (i = 0; i < count; i++) {
		for (j = 0; j < (count + 1); j++) {
			printf("[%.2lf] ", matrix[i][j]);
		}
		printf("\n");
	}

	printf("\n\n");
}

void printVector (double* vector, int count) {
	int i;

	for (i = 0; i < count; i++) {
		printf ("[%.2lf] ", vector[i]);
	}
	printf("\n\n");
}

void freeAll (FILE* fp, double** matrix, double* result, double* row_array, double* num_proc, int count) {
	int i;

	fclose (fp);
	free (result);
	free (row_array);
	free (num_proc);

	for (i = 0; i < count; i++) free (matrix[i]);
	free (matrix);
}
