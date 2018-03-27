#ifndef main_h
#define main_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include "mpi.h"

/********************************** MATRIZ ***************************/
FILE* openFile (char*);

void getSize (FILE*, int*);

double** allocMatrix (int);

void readMatrixFile (FILE*, double**, int);

/******************************** RESULTADO ***************************/
FILE* openResultFile ();

double* allocResult (int);

void writeResultFile (FILE*, double*, int);

/*********************************************************************/
double* allocVectorColumn (int);

void mpiProcess (int, char**, double**, double*, double*, double*, int);

void copyArrayTo (double*, double*, int);
/***************************** AUXILIARES ****************************/
void printMatrix (double**, int);

void printVector (double*, int);

void freeAll (FILE*, double**, double*, double*, double*, int);

void readVectorFile (FILE*, double**, int);

#endif
