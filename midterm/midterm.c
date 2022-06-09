#include<stdio.h>
#include<omp.h>
#include<pthread.h>
#include<stdlib.h>
#define N 2000
int a[N][N];
int b[N][N];
int bt[N][N];
int c[N][N];
int c2[N][N];
int c3[N][N];
int c4[N][N];

void *runner(void *param); 
int main(){
    pthread_t tid[4];       //Thread ID
    pthread_attr_t attr[4]; //Set of thread attributes
    int i,j,k,*data;

    for(i = 0;i<N;i++){
        for(j = 0;j<N;j++){
            a[i][j]=rand()%100;
            b[i][j]=rand()%100;
        }
    }
     for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            bt[j][i]=b[i][j];
        }
    }
     double start_time = omp_get_wtime();
     for(i =0;i<N;i++){
         for(j =0;j<N;j++){
             c[i][j]=0;
             for(k =0;k<N;k++){
                 c[i][j]+=a[i][k]*b[k][j];
             }
         }
     }
    double endtime=omp_get_wtime();
    printf("spend times %4g s\n",(endtime-start_time));
    printf("-----------------------------------------\n");   
    double start_time2 = omp_get_wtime();
        #pragma omp parallel for private(j,k) num_threads(4)
        for( i =0;i<N;i++){
            for( j =0;j<N;j++){
               c2[i][j]=0;
             for( k =0;k<N;k++){
                #pragma omp atomic
                c2[i][j]+=a[i][k]*bt[j][k];
                }
            }
         }
    double endtime2=omp_get_wtime();

    int pass =1;
         for (i=0;i<N;i++){
            for(j=0;j<N;j++){
                if(c[i][j]!=c2[i][j]){
                    pass =0;
                }
            }
        }
      
    if (pass ==1){
        printf("Using atomic spend times %4g s\n",(endtime2-start_time2));
        printf("correct\n");

    }
    else{
        printf("wrong\n");
    }
        printf("-----------------------------------------\n");

    int axbt = 0;
    double start_time3 = omp_get_wtime();
        for(i =0;i<N;i++){
            #pragma omp parallel for private(j) num_threads(4)
            for(j =0;j<N;j++){
                #pragma omp parallel for reduction(+:axbt) private(k)
                for(k =0;k<N;k++){
                    axbt += a[i][k]*bt[j][k];
                    c3[i][j]=axbt;
            }
        }
    }
    double endtime3=omp_get_wtime();

    int pass2 =1;
        for (i=0;i<N;i++){
            for(j=0;j<N;j++){
                if(c[i][j]!=c3[i][j]){
                    pass2 =0;
                }
            }
        }
      
    if (pass2 ==1){
        printf("Using reduction spend times %4g s\n",(endtime3-start_time3));
        printf("correct\n");

    }
    else{
        printf("wrong\n");
    }
    printf("-----------------------------------------\n");
    double start_time4 = omp_get_wtime();
    for(i = 0; i < 4; i++) { 
                data = (int*)malloc(sizeof(int));
                *data = i;
                pthread_attr_init(&attr[i]);
                pthread_create(&tid[i],&attr[i],runner,data);
                }
        for(i = 0; i < 4; i++) {
                pthread_join(tid[i], NULL);      
        }
    double endtime4=omp_get_wtime();
    int pass3 =1;
        for (i=0;i<N;i++){
            for(j=0;j<N;j++){
                if(c[i][j]!=c4[i][j]){
                    pass3 =0;
                }
            }
        }
      
    if (pass3 ==1){
        printf("Using pthread spend times %4g ms\n",(endtime4-start_time4));
        printf("correct\n");

    }
    else{
        printf("wrong\n");
    }


    return 0;

}
void *runner(void *param){
        int*data = param;
        int n = 0;
        if(*data==0){    
                for(int i = 0;i<N;i++){
                        for(int j = 0;j<0.25*N;j++){ 
                                for(n = 0; n< N; n++){
                                c4[i][j] += a[i][n] * bt[j][n];
                                        }
                        }
                }                                       
        }
         if(*data==1){    
                for(int i=0;i<N;i++){
                        for(int j = 0.25*N;j<0.5*N;j++){ 
                                for(n = 0; n< N; n++){
                                c4[i][j] += a[i][n] * bt[j][n];
                                        }
                        }
                } 
                                              }
         if(*data==2){    
                for(int i=0;i<N;i++){
                        for(int j = 0.5*N;j<N*0.75;j++){ 
                                for(n = 0; n< N; n++){
                                c4[i][j] += a[i][n] * bt[j][n];
                                        }
                        }
                }                 
        }
         if(*data==3){    
                for(int i= 0;i<N;i++){
                        for(int j = 0.75*N;j<N;j++){ 
                                for(n = 0; n< N; n++){
                                c4[i][j]  += a[i][n] * bt[j][n];
                                       }
                        }
                } 
                                      
        }
        pthread_exit(0);
}



