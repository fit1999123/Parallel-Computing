#include <stdio.h>
#include <stdlib.h>
#include <cuda_runtime.h>
#define N 1024*1024
#define THREADBLOCKS 16
__global__ void vector_add( int *a, int *b, int *c ){
    int tid = threadIdx.x;
    while(tid<N){
        c[tid] = a[tid] + b[tid];
        tid += THREADBLOCKS ;
        }
}

int main( void ) {
    int *a, *b, *c;
    int *dev_a, *dev_b, *dev_c;
	
    // allocate the memory on the CPU
    a = (int*)malloc( N * sizeof(int) );
    b = (int*)malloc( N * sizeof(int) );
    c = (int*)malloc( N * sizeof(int) );
	
    // allocate the memory on the GPU
    cudaMalloc( (void**)&dev_a, N * sizeof(int) );
    cudaMalloc( (void**)&dev_b, N * sizeof(int) );
    cudaMalloc( (void**)&dev_c, N * sizeof(int) );

    // fill the arrays 'a' and 'b' on the CPU
	srand ( time(NULL) );
    for (int i=0; i<N; i++) {
        a[i] = rand()%256;
        b[i] = rand()%256;
    }
    printf("a = %d",&a);
    printf("b = %d",&b);
    // copy the arrays 'a' and 'b' to the GPU
    cudaMemcpy( dev_a, a, N * sizeof(int), cudaMemcpyHostToDevice );
    cudaMemcpy( dev_b, b, N * sizeof(int), cudaMemcpyHostToDevice );
    
	
    vector_add<<<1, THREADBLOCKS>>>( dev_a, dev_b, dev_c );
    
	
    
    // copy the array 'c' back from the GPU to the CPU
    cudaMemcpy( c, dev_c, N * sizeof(int), cudaMemcpyDeviceToHost );
	
    // verify that the GPU did the work we requested
    bool success = true;
    for (int i=0; i<N; i++) {
        if ((a[i] + b[i]) != c[i]) {
            printf( "Error:  %d + %d != %d\n", a[i], b[i], c[i] );
            success = false;
        }
    }
    if (success)    printf( "We did it!\n" );

    // free the memory allocated on the GPU
    cudaFree( dev_a );
    cudaFree( dev_b );
    cudaFree( dev_c );

    return 0;
}