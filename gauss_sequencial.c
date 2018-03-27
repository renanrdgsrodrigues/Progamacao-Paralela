// to compile: mpicc mpi_scatter.c -o main -lm -Wall
// to run: mpirun -np 3 main

#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<math.h>
#include"mpi.h"
#include <omp.h>

#define M 3
#define N 4
void print(int matrix[M][N]){   
    int i, j;
    for(i = 0; i < M; i++)
        for(j = 0; j < N; j++)
            printf("%d ", matrix[i][j]);
        printf("\n");
}
void printRow(double row[]){
    int i;
    for(i = 0; i < N; i++)
        printf("%.3f ", row[i]);
}

void copyArrayTo(double *source, double *dest){ 
    int i;
    for(i = 0; i < N; i++)
        dest[i] = source[i];
}
int main(int argc, char **argv){
    int my_rank, num_proc;//, order;
    int i, k, current_row = 0;
    double matrix[M][N] = {{5, 0, 2, 5}, {1, 2, 1, 6}, {6, 4, 3, 7}};
    double row_array[N], send_row[N];
    double answear[M];
    double pivo, value;

    /*
    MPI_Comm *row_comm, *column_comm, intercomm, cij_comm;
    MPI_Status status;
    MPI_Request *mpirequest_mr, *mpirequest_rc, *mpirequest_cc;
    */
    /*if (argc != 2) {
        printf("Wrong number of arguments. Please try again with mpirun -np order*order matrix <order> \n");
        fflush(0);
        exit(0);
    }//end-if
    
    order = atoi(argv[1]);
    */
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_proc);
    
    MPI_Scatter(&matrix, N, MPI_DOUBLE, &row_array, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    //printRow(row_array);
    //for(i = 0; i < N; i++)
    //  printf("%.3f from proc: %d\n", row_array[i], my_rank);
    
    while(current_row < M){
        if(my_rank == current_row){
            //Divide linha do Pivo, pelo pivo
            pivo = row_array[current_row];
            for(i = current_row; i < N; i++){
                row_array[i] = row_array[i]/pivo;   
            }
            copyArrayTo(row_array, send_row);
        }   
        MPI_Barrier(MPI_COMM_WORLD);    
        MPI_Bcast(&send_row, N, MPI_DOUBLE, current_row, MPI_COMM_WORLD);
        
        if(my_rank > current_row){
            //Aqui entra OpenMP
            value = row_array[current_row];
            //#pragma omp parallel for shared(row_array, send_row) private(k, value)            
            for(k = current_row; k < N; k++){
                row_array[k] = row_array[k] - value * send_row[k];
            }   

        }
        
        current_row++;
    }
    /*
    current_row = M - 1;
    while(current_row >= 0){
        if(my_rank == current_row){
            //Divide linha do Pivo, pelo pivo
            pivo = row_array[current_row];
            for(i = current_row; i < N; i++){
                row_array[i] = row_array[i]/pivo;   
            }
            copyArrayTo(row_array, send_row);
        }   
        MPI_Barrier(MPI_COMM_WORLD);    
        MPI_Bcast(&send_row, N, MPI_DOUBLE, current_row, MPI_COMM_WORLD);
        
        if(my_rank < current_row){
            //Aqui entra OpenMP
            value = row_array[current_row];
            //#pragma omp parallel for shared(row_array, send_row) private(k, value)            
            for(k = current_row; k < N; k++){
                row_array[k] = row_array[k] - value * send_row[k];
            }   

        }
        
        current_row--;
    }*/
    
    // Mostra matriz toda, espero q na forma reduzida
    printf("\n");
    for(i = 0; i < N; i++)
            printf("%.3f processo:%d\n", row_array[i], my_rank);
    
    MPI_Gather(&row_array[N - 1], 1, MPI_DOUBLE, &answear, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    if(my_rank == 0){       
        for(i = 0; i < M; i++){
            printf("%.3f\n", answear[i]);
        }
    }
    MPI_Finalize();
    return 0;
}

