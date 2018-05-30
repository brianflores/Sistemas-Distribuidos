#include <mpi.h>
#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

/*
CONSULTAS:
-ver si no es mejor mover todo lo que se repite en root/workers a una funcion (por ejemplo las multiplicaciones)
-como calcular las metricas, donde empezar a tomar los tiempos
-overhead (???)
-se toman tiempos en los workers tambien?
-preguntar como se hace bien el machine file
*/

double dwalltime()
{
  double sec;
  struct timeval tv;

  gettimeofday(&tv,NULL);
  sec = tv.tv_sec + tv.tv_usec/1000000.0;
  return sec;
}

void root(int N, int cantProcesos); //funcion para el proceso 0
void workers(int ID, int N, int cantProcesos); //funcion para los otros procesos

int main(int argc, char** argv) {
    if (argc < 2){
        printf("Faltan argumentos \n");
        return 0;
    }
    int N = atol(argv[1]);
    int ID, cantProcesos;
    omp_set_num_threads(cantProcesos);
    MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &ID);
	MPI_Comm_size(MPI_COMM_WORLD, &cantProcesos);
    if (ID == 0) root(N,cantProcesos);
    else if (ID > 0) workers(ID,N,cantProcesos);
    MPI_Finalize();
    return 0;
}

void root(int N, int cantProcesos){
    double *A, *B, *C, *D, *L, *U, *a, *c, *d, *AB, *LC, *DU, *TOTAL, *total;
    double promedioL, promedioU, resultadoL, resultadoU, timetick;
    int i,j,k;
    int filas = N/cantProcesos; //filas por proceso
    A=(double*)malloc(sizeof(double)*N*N);
    B=(double*)malloc(sizeof(double)*N*N);
    C=(double*)malloc(sizeof(double)*N*N);
    D=(double*)malloc(sizeof(double)*N*N);
    L=(double*)malloc(sizeof(double)*N*N);
    U=(double*)malloc(sizeof(double)*N*N);
    a=(double*)malloc(sizeof(double)*filas*N);
    c=(double*)malloc(sizeof(double)*filas*N);
    d=(double*)malloc(sizeof(double)*filas*N);
    AB=(double*)malloc(sizeof(double)*filas*N);
    LC=(double*)malloc(sizeof(double)*filas*N);
    DU=(double*)malloc(sizeof(double)*filas*N);
    TOTAL=(double*)malloc(sizeof(double)*N*N);
    total=(double*)malloc(sizeof(double)*filas*N);
    
    for(i=0;i<N;i++){       //Crea matrices
        for(j=0;j<N;j++){
            A[i*N+j]=1.0;
            B[i*N+j]=1.0;
            C[i*N+j]=1.0;
            D[i*N+j]=1.0;
            if(i==j){
                L[i*N+j]= 1.0;
                U[i*N+j]= 1.0;
            } else if(i>j){
                U[i*N+j]= 1.0;
                L[i*N+j]= 0.0;
            } else {
                U[i*N+j]= 0.0;
                L[i*N+j]= 1.0;
            }
        }
    }
    timetick = dwalltime(); //Esto va aca?????? o despues de dividir todo

    MPI_Scatter(A, N*filas, MPI_DOUBLE, a, N*filas, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(B,N*N, MPI_DOUBLE,0,MPI_COMM_WORLD);
    MPI_Scatter(C, N*filas, MPI_DOUBLE, c, N*filas, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(L,N*N, MPI_DOUBLE,0,MPI_COMM_WORLD);
    MPI_Scatter(D, N*filas, MPI_DOUBLE, d, N*filas, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(U,N*N, MPI_DOUBLE,0,MPI_COMM_WORLD);
    MPI_Scatter(TOTAL, N*filas, MPI_DOUBLE, total, N*filas, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    promedioL = 0;
    promedioU = 0;
    #pragma omp parallel 
    {       
        #pragma omp for collapse(2) reduction(+:promedioL,promedioU)
        for(i=0;i<filas;i++){   //Calcula los promedios
            for(j=0;j<N;j++){
                promedioL+= L[i*N+j];
                promedioU+= U[i*N+j];
            }
        }
        #pragma omp single
        {
            MPI_Allreduce(&promedioL, &resultadoL, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
            MPI_Allreduce(&promedioU, &resultadoU, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
            promedioL = resultadoL/(N*N);
            promedioU = resultadoU/(N*N);
            promedioL = promedioL*promedioU; //en promedioL queda el producto de ambos promedios
        }

        #pragma omp for collapse(2) private(k) 
        for(i=0;i<filas;i++){   //AB = a*B
            for(j=0;j<N;j++){
                    AB[i*N+j]=0;
                    for(k=0;k<N;k++){
                        AB[i*N+j]= AB[i*N+j] + a[i*N+k]*B[k+j*N];
                    }
            }
        }
        #pragma omp for collapse(2) private(k) 
        for(i=0;i<filas;i++){   //LC = c*L
            for(j=0;j<N;j++){
                    LC[i*N+j]=0;
                    for(k=0;k<N;k++){
                        LC[i*N+j]= LC[i*N+j] + c[i*N+k]*L[k+j*N];
                    }
            }
        }
        #pragma omp for collapse(2) private(k) 
        for(i=0;i<filas;i++){   //DU = d*U
            for(j=0;j<N;j++){
                    DU[i*N+j]=0;
                    for(k=0;k<N;k++){
                        DU[i*N+j]= DU[i*N+j] + d[i*N+k]*U[k+j*N];
                    }
            }
        }
        #pragma omp for collapse(2)
        for(i=0;i<filas;i++){   //total = (AB+LC+DU)*promedioL
            for(j=0;j<N;j++){
                    total[i*N+j]= (AB[i*N+j] + LC[i*N+j] + DU[i*N+j])*promedioL;
            }
        }
    }
    MPI_Gather(total, filas*N, MPI_DOUBLE, TOTAL, filas*N, MPI_DOUBLE, 0, MPI_COMM_WORLD); //Cada proceso envia su pedacito de matriz, las recibe el proceso root en TOTAL    
    printf("Tiempo en segundos %f \n", dwalltime() - timetick);

    int check = 1;
    double resultado = TOTAL[0];
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
        check = check && (TOTAL[i*N+j]==resultado);
        }
    }

    if(check){
        printf("Multiplicacion de matriz correcta\n");
    }else{
        printf("Multiplicacion de matriz erroneo\n");
    }
    free(A);
    free(B);
    free(C);
    free(D);
    free(L);
    free(U);
    free(a);
    free(c);
    free(d);
    free(AB);
    free(LC);
    free(DU);
    free(TOTAL);
    free(total);
}

void workers(int ID, int N, int cantProcesos){
    double *A, *B, *C, *D, *L, *U, *a, *c, *d, *AB, *LC, *DU, *TOTAL, *total;
    double promedioL, promedioU, resultadoL, resultadoU, timetick;
    int i,j,k;
    int filas = N/cantProcesos; //filas por proceso
    B=(double*)malloc(sizeof(double)*N*N);
    L=(double*)malloc(sizeof(double)*N*N);
    U=(double*)malloc(sizeof(double)*N*N);
    a=(double*)malloc(sizeof(double)*filas*N);
    c=(double*)malloc(sizeof(double)*filas*N);
    d=(double*)malloc(sizeof(double)*filas*N);
    AB=(double*)malloc(sizeof(double)*filas*N);
    LC=(double*)malloc(sizeof(double)*filas*N);
    DU=(double*)malloc(sizeof(double)*filas*N);
    total=(double*)malloc(sizeof(double)*filas*N);

    MPI_Scatter(A, N*filas, MPI_DOUBLE, a, N*filas, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(B,N*N, MPI_DOUBLE,0,MPI_COMM_WORLD);
    MPI_Scatter(C, N*filas, MPI_DOUBLE, c, N*filas, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(L,N*N, MPI_DOUBLE,0,MPI_COMM_WORLD);
    MPI_Scatter(D, N*filas, MPI_DOUBLE, d, N*filas, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(U,N*N, MPI_DOUBLE,0,MPI_COMM_WORLD);
    MPI_Scatter(TOTAL, N*filas, MPI_DOUBLE, total, N*filas, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    promedioL = 0;
    promedioU = 0;
    #pragma omp parallel 
    {       
        #pragma omp for collapse(2) reduction(+:promedioL,promedioU)
        for(i=0;i<filas;i++){   //Calcula los promedios
            for(j=0;j<N;j++){
                promedioL+= L[i*N+j];
                promedioU+= U[i*N+j];
            }
        }
        #pragma omp single
        {
            MPI_Allreduce(&promedioL, &resultadoL, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
            MPI_Allreduce(&promedioU, &resultadoU, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
            promedioL = resultadoL/(N*N);
            promedioU = resultadoU/(N*N);
            promedioL = promedioL*promedioU; //en promedioL queda el producto de ambos promedios
        }

        #pragma omp for collapse(2) private(k) 
        for(i=0;i<filas;i++){   //AB = a*B
            for(j=0;j<N;j++){
                    AB[i*N+j]=0;
                    for(k=0;k<N;k++){
                        AB[i*N+j]= AB[i*N+j] + a[i*N+k]*B[k+j*N];
                    }
            }
        }
        #pragma omp for collapse(2) private(k) 
        for(i=0;i<filas;i++){   //LC = c*L
            for(j=0;j<N;j++){
                    LC[i*N+j]=0;
                    for(k=0;k<N;k++){
                        LC[i*N+j]= LC[i*N+j] + c[i*N+k]*L[k+j*N];
                    }
            }
        }
        #pragma omp for collapse(2) private(k) 
        for(i=0;i<filas;i++){   //DU = d*U
            for(j=0;j<N;j++){
                    DU[i*N+j]=0;
                    for(k=0;k<N;k++){
                        DU[i*N+j]= DU[i*N+j] + d[i*N+k]*U[k+j*N];
                    }
            }
        }
        #pragma omp for collapse(2)
        for(i=0;i<filas;i++){   //total = (AB+LC+DU)*promedioL
            for(j=0;j<N;j++){
                    total[i*N+j]= (AB[i*N+j] + LC[i*N+j] + DU[i*N+j])*promedioL;
            }
        }
    }
    MPI_Gather(total, filas*N, MPI_DOUBLE, TOTAL, filas*N, MPI_DOUBLE, 0, MPI_COMM_WORLD); //Cada proceso envia su pedacito de matriz, las recibe el proceso root en TOTAL
    free(B);
    free(L);
    free(U);
    free(a);
    free(c);
    free(d);
    free(AB);
    free(LC);
    free(DU);
    free(total);
}
