#include <stdio.h>

__global__ void helloFromGPU (int n)
{
	printf("Hello from GPU with grid %d and thread %d\n", n, threadIdx.x);
	//printf("From:%d, %d ", n, blockIdx.x);
}

int main (void)
{
	helloFromGPU<<<1,10>>>(1);
	cudaDeviceSynchronize();

	helloFromGPU<<<1,10>>>(2);
	cudaDeviceSynchronize();

	printf("Hello CPU\n");

//	cudaDeviceSynchronize();
	
	return 0;
}
