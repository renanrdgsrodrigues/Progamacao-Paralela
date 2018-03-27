#include <stdio.h>
#include <stdlib.h>

#define SIZE 5

int main (int argc, char** argv) {
	int i, j, k;
	int mat[SIZE][SIZE] = { {2,2,2,2,2}, {2,2,2,2,2}, {2,2,2,2,2}, {2,2,2,2,2}, {2,2,2,2,2} };
	int mat2[SIZE][SIZE] = { {2,2,2,2,2}, {2,2,2,2,2}, {2,2,2,2,2}, {2,2,2,2,2}, {2,2,2,2,2} };
	int result[SIZE][SIZE];

	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++) {
			result[i][j] = 0;
			for (k = 0; k < SIZE; k++) {
				result[i][j] += mat[i][k] * mat2[k][j];			
			}
		}
	}	

	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++) {
			printf ("%d ", result[i][j]);
		}
		printf ("\n");
	}

	return 0;
}
