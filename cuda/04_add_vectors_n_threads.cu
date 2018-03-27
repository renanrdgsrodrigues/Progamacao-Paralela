#include <stdlib.h>
#include <stdio.h>

#define N   10

__global__ void add( int *a, int *b, int *c ) {
    /**
     * threadIdx.x contém o Id da thread (no bloco) a ser executada
     */

	int tid = threadIdx.x;    // this thread handles the data at its thread id
    if (tid < N)
        c[tid] = a[tid] + b[tid];
}

int main( void ) {
    int a[N], b[N], c[N];
    int *dev_a, *dev_b, *dev_c;

    // allocate the memory on the GPU
    cudaMalloc( (void**)&dev_a, N * sizeof(int));
    cudaMalloc( (void**)&dev_b, N * sizeof(int));
    cudaMalloc( (void**)&dev_c, N * sizeof(int));

    // fill the arrays 'a' and 'b' on the CPU
    for (int i=0; i<N; i++) {
        a[i] = -i;
        b[i] = i * i;
    }

    // copy the arrays 'a' and 'b' to the GPU
    cudaMemcpy( dev_a, a, N * sizeof(int),cudaMemcpyHostToDevice);
    cudaMemcpy( dev_b, b, N * sizeof(int),cudaMemcpyHostToDevice);

    /**
     *  N é o no. de threads a ser executada em paralelo pela GPU
     */
    add<<<1,N>>>(dev_a,dev_b,dev_c );

    // copy the array 'c' back from the GPU to the CPU
    cudaMemcpy(c, dev_c, N * sizeof(int),cudaMemcpyDeviceToHost);

    // display the results
    for (int i=0; i<N; i++) {
        printf( "[%d] %d + %d = %d\n",i, a[i], b[i], c[i] );
    }

    // free the memory allocated on the GPU
    cudaFree(dev_a);
    cudaFree(dev_b);
    cudaFree(dev_c);

	getchar();

    return 0;
}
