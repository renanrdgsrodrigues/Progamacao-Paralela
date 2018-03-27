#include "main.h"

int main (int argc, char** argv) {
	FILE* fp = NULL;
	double** matrix = NULL;
	double* result = NULL;
	int count = 0;
	
	double* row_array = NULL;
	double* send_row = NULL;

/*************************** FUNÇÕES PARA O ARQUIVO DA MATRIZ ************************/
	fp = openFile ("matrix.txt");
	getSize (fp, &count);

	matrix = allocMatrix (count);
	if (!matrix) exit (0);

	readMatrixFile (fp, matrix, count);

	//printMatrix (matrix, count);

/************************** FUNÇÕES PARA O ARQUIVO DO VETOR **************************/
	fp = openFile ("vector.txt");	

	readVectorFile (fp, matrix, count);



/************************* FUNÇÕES PARA O ARQUIVO DO RESULTADO ***********************/
	fp = openFile ("result.txt");

	result = allocResult (count);
	if (!result) exit (0);

/*************************************************************************************/
	row_array = allocVectorColumn (count + 1);
	send_row = allocVectorColumn (count + 1);

	mpiProcess (argc, argv, matrix, result, row_array, send_row, count);


	writeResultFile (fp, result, count);

/**************************************************************************************/
	freeAll (fp, matrix, result, row_array, send_row, count);

	return 0;
}


