#include<stdlib.h>
#include<stdio.h>
#include<cuda_runtime.h>
#define N 1024*1024

__global__ void dot( int *a, int *b, int *c, int *dot ){
    int tid = threadIdx.x;
    int tid_temp = tid;
	int i;
    int temp =0;
    
    while(tid_temp<N){

        temp += a[tid_temp]*b[tid_temp]; 
        tid_temp += blockDim.x;

    }

    c[tid] = temp;

	
    // synchronize threads in this block
    __syncthreads();
	
    i = blockDim.x/2;
	
    while (i != 0) {
        if (tid < i){
            c[tid] += c[tid + i];   
	}
	__syncthreads();
        i /= 2;
    }
	
    if(tid==0) *dot = c[tid];
}

int main( void ) {
    int *a, *b;
    int *dev_a, *dev_b, *dev_c, *dev_dot;
    int dotCPU = 0;
    int dotGPU;
	
    // allocate the memory on the CPU
    a = (int*)malloc( N * sizeof(int) );
    b = (int*)malloc( N * sizeof(int) );
	
    // allocate the memory on the GPU
    cudaMalloc( (void**)&dev_a, N * sizeof(int) );
    cudaMalloc( (void**)&dev_b, N * sizeof(int) );
    cudaMalloc( (void**)&dev_c, N * sizeof(int) );
    cudaMalloc( (void**)&dev_dot, sizeof(int) );

    // fill the arrays 'a' and 'b' on the CPU
    srand ( time(NULL) );
    for (int i=0; i<N; i++) {
        a[i] = rand()%256;
        b[i] = rand()%256;
    }
    // copy the arrays 'a' and 'b' to the GPU
    cudaMemcpy( dev_a, a, N * sizeof(int), cudaMemcpyHostToDevice );
    cudaMemcpy( dev_b, b, N * sizeof(int), cudaMemcpyHostToDevice );
	
    dot<<<1, N>>>( dev_a, dev_b, dev_c, dev_dot );
    
    // copy the array 'dev_dot' back from the GPU to the CPU
    cudaMemcpy( &dotGPU, dev_dot, sizeof(int), cudaMemcpyDeviceToHost );
	
    // verify that the GPU did the work we requested
    bool success = true;
    for (int i=0; i<N; i++) {
	dotCPU += a[i] * b[i];
    }
    if (dotCPU != dotGPU) {
        printf( "Error: dotCPU %d != dotGPU %d\n", dotCPU, dotGPU );
        success = false;
    }
    if (success)    printf( "Test pass!\n" );

    // free the memory allocated on the GPU
    cudaFree( dev_a );
    cudaFree( dev_b );
    cudaFree( dev_c );
    cudaFree( dev_dot );

    return 0;
}


