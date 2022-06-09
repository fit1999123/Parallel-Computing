#include<stdio.h>
#include<omp.h>
#include<stdlib.h>
#define N 20

int i,j,k;
int a[N][N];
int b[N][N];
int bt[N][N];
int c[N][N];
int c2[N][N];

int main(){

    for(i=0;i<N;i++){
            for(j=0;j<N;j++){
                a[i][j]=rand()%100;
                b[i][j]=rand()%100;
        }
    }
    
    for(i=0;i<N;i++){
            for(j=0;j<N;j++){
                for(k=0;k<N;k++){
                    c[i][j] += a[i][k]*b[k][j];
                   
                }
            }
        }

    //transpose
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            bt[j][i]=b[i][j];
        }
    }

    int sum = 0;
    double start_time =omp_get_wtime();
        #pragma omp parallel for private(j) num_threads(15)
        for(i=0;i<N;i++){
            for(j=0;j<N;j++){
            #pragma omp parallel for reduction(+:sum) private(k)    
                for(k=0;k<N;k++){
                    sum += a[i][k]*bt[j][k];
                    c2[i][j] = sum;
                }
            }
        }
    double end_time = omp_get_wtime();
    //check
    int pass =1;
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            if(c[i][j]!=c2[i][j]){
            pass = 0;
            }
        }
    }



    if(pass ==1){
        printf("correct!\n");
        printf("spend times = %4g s\n",end_time-start_time);
    }
    else{
        printf("failed\n");
    }
    return 0;
}
