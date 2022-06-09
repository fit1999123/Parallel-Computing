#include <stdio.h>
#include <stdlib.h>
#include <cuda_runtime.h>
#define N 100000000
#define THREADBLOCKS 1024

__global__ void vectoradd(int *a,int *b,int *c){

    int tid = threadIdx.x;

    while(tid<N){
    
        c[tid] = a[tid] + b[tid];
        tid+=THREADBLOCKS;
    }
}

int main(){
    int *a = (int*)malloc(N*sizeof(int));
    int *b = (int*)malloc(N*sizeof(int));
    int *c = (int*)malloc(N*sizeof(int));
    int *dev_a, *dev_b, *dev_c;
    cudaMalloc( (void**)&dev_a, N * sizeof(int) );
    cudaMalloc( (void**)&dev_b, N * sizeof(int) );
    cudaMalloc( (void**)&dev_c, N * sizeof(int) );
    srand(time(NULL));
    for (int i=0; i<N; i++) {
        a[i] = rand()%256;
        b[i] = rand()%256;
    }
    cudaEvent_t start, stop;
    float time;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaMemcpy( dev_a, a, N * sizeof(int), cudaMemcpyHostToDevice );
    cudaMemcpy( dev_b, b, N * sizeof(int), cudaMemcpyHostToDevice );
    cudaEventRecord(start, 0);
    vectoradd<<<1,THREADBLOCKS>>>(dev_a,dev_b,dev_c);
    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&time, start, stop);
    cudaMemcpy( c, dev_c, N * sizeof(int), cudaMemcpyDeviceToHost );
    bool success = true;
    for (int i=0; i<N; i++) {
        if ((a[i] + b[i]) != c[i]) {
            printf( "Error:  %d + %d != %d\n", a[i], b[i], c[i] );
            success = false;
        }
    }
    if (success)  {
        printf( "We did it!\n" );
        printf ("Using one block and many threads spend times: %f s\n",time/1000);
    }
    cudaFree( dev_a );
    cudaFree( dev_b );
    cudaFree( dev_c );
    return 0;
}