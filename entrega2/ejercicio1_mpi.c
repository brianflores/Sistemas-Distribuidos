#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>


double dwalltime(){
  double sec;
  struct timeval tv;
  gettimeofday(&tv,NULL);
  sec = tv.tv_sec + tv.tv_usec/1000000.0;
  return sec;
}

/*
Consultas:
Hay que almacenar solo los elementos no nulos de la matriz U? (triangular superior almacenada por filas)
Como calculo eso para hacer el malloc?
*/


void root(int N, int cantProcesos); //funcion para el proceso 0
void workers(int ID, int N, int cantProcesos); //funcion para los otros procesos

int main(int argc, char** argv) {
    if (argc < 2){
        printf("Faltan argumentos \n");
        return 0;
    }
    int N = atol(argv[1]);
    int ID, cantProcesos;
    MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &ID);
	MPI_Comm_size(MPI_COMM_WORLD, &cantProcesos);
    if (ID == 0) root(N,cantProcesos);
    else if (ID > 0) workers(ID,N,cantProcesos);
    MPI_Finalize();
    return 0;
}

void root(int N, int cantProcesos){
    double *A, *B, *C, *D, *L, *U, *a, *l, *d, *AB, *LC, *DU, *TOTAL, *total;
    double promedioL, promedioU, resultadoL, resultadoU, timetick, timetick2, timetick3;
    int i,j,k;
    int filas = N/cantProcesos; //filas por proceso
    A=(double*)malloc(sizeof(double)*N*N);
    B=(double*)malloc(sizeof(double)*N*N);
    C=(double*)malloc(sizeof(double)*N*N);
    D=(double*)malloc(sizeof(double)*N*N);
    L=(double*)malloc(sizeof(double)*N*N);
    U=(double*)malloc(sizeof(double)*N*N);
    a=(double*)malloc(sizeof(double)*filas*N);
    l=(double*)malloc(sizeof(double)*filas*N);
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
    timetick = dwalltime();

    MPI_Scatter(A, N*filas, MPI_DOUBLE, a, N*filas, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(B,N*N, MPI_DOUBLE,0,MPI_COMM_WORLD);
    MPI_Scatter(L, N*filas, MPI_DOUBLE, l, N*filas, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(C,N*N, MPI_DOUBLE,0,MPI_COMM_WORLD);
    MPI_Scatter(D, N*filas, MPI_DOUBLE, d, N*filas, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(U,N*N, MPI_DOUBLE,0,MPI_COMM_WORLD);
    MPI_Scatter(TOTAL, N*filas, MPI_DOUBLE, total, N*filas, MPI_DOUBLE, 0, MPI_COMM_WORLD);
   
    printf("Tiempo en segundos de las comunicaciones 1: %f \n", dwalltime() - timetick);

    promedioL = 0;
    promedioU = 0;
    for(i=0;i<filas;i++){   //Calcula los promedios
       for(j=0;j<N;j++){
           promedioL+= l[i*N+j];
           promedioU+= U[i*N+j];
       }
    }
	
    timetick2 = dwalltime();
	
    MPI_Allreduce(&promedioL, &resultadoL, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    MPI_Allreduce(&promedioU, &resultadoU, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
	
    printf("Tiempo en segundos de las comunicaciones 2: %f \n", dwalltime() - timetick2);
	
    promedioL = resultadoL/(N*N);
    promedioU = resultadoU/(N*N);
    promedioL = promedioL*promedioU; //en promedioL queda el producto de ambos promedios


    for(i=0;i<filas;i++){   //AB = a*B
       for(j=0;j<N;j++){
            AB[i*N+j]=0;
            for(k=0;k<N;k++){
	            AB[i*N+j]= AB[i*N+j] + a[i*N+k]*B[k+j*N];
            }
       }
    }
    for(i=0;i<filas;i++){   //LC = l*C
       for(j=0;j<N;j++){
            LC[i*N+j]=0;
            for(k=0;k<N;k++){
	            LC[i*N+j]= LC[i*N+j] + l[i*N+k]*C[k+j*N];
            }
       }
    }
    for(i=0;i<filas;i++){   //DU = d*U
       for(j=0;j<N;j++){
            DU[i*N+j]=0;
            for(k=j;k<N;k++){
	            DU[i*N+j]= DU[i*N+j] + d[i*N+k]*U[k+j*N];
            }
       }
    }
    for(i=0;i<filas;i++){   //total = (AB+LC+DU)*promedioL
       for(j=0;j<N;j++){
	        total[i*N+j]= (AB[i*N+j] + LC[i*N+j] + DU[i*N+j])*promedioL;
       }
    }
	
    timetick3 = dwalltime();
	
    MPI_Gather(total, filas*N, MPI_DOUBLE, TOTAL, filas*N, MPI_DOUBLE, 0, MPI_COMM_WORLD); //Cada proceso envia su pedacito de matriz, las recibe el proceso root en TOTAL
   
    printf("Tiempo en segundos de las comunicaciones 3: %f \n", dwalltime() - timetick3);
    
    printf("Tiempo en segundos del root: %f \n", dwalltime() - timetick);


    //Lo comento porque da erroneo, cuando lo resolvi a mano me daba que los valores no eran todos iguales - preguntar
    /* int check = 1;
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
    } */
    free(A);
    free(B);
    free(C);
    free(D);
    free(L);
    free(U);
    free(a);
    free(l);
    free(d);
    free(AB);
    free(LC);
    free(DU);
    free(TOTAL);
    free(total);
}

void workers(int ID, int N, int cantProcesos){
    double *A, *B, *C, *D, *L, *U, *a, *l, *d, *AB, *LC, *DU, *TOTAL, *total;
    double promedioL, promedioU, resultadoL, resultadoU, timetick;
    int i,j,k;
    int filas = N/cantProcesos; //filas por proceso
    B=(double*)malloc(sizeof(double)*N*N);
    C=(double*)malloc(sizeof(double)*N*N);
    U=(double*)malloc(sizeof(double)*N*N);
    a=(double*)malloc(sizeof(double)*filas*N);
    l=(double*)malloc(sizeof(double)*filas*N);
    d=(double*)malloc(sizeof(double)*filas*N);
    AB=(double*)malloc(sizeof(double)*filas*N);
    LC=(double*)malloc(sizeof(double)*filas*N);
    DU=(double*)malloc(sizeof(double)*filas*N);
    total=(double*)malloc(sizeof(double)*filas*N);

    MPI_Scatter(A, N*filas, MPI_DOUBLE, a, N*filas, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(B,N*N, MPI_DOUBLE,0,MPI_COMM_WORLD);
    MPI_Scatter(L, N*filas, MPI_DOUBLE, l, N*filas, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(C,N*N, MPI_DOUBLE,0,MPI_COMM_WORLD);
    MPI_Scatter(D, N*filas, MPI_DOUBLE, d, N*filas, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(U,N*N, MPI_DOUBLE,0,MPI_COMM_WORLD);
    MPI_Scatter(TOTAL, N*filas, MPI_DOUBLE, total, N*filas, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    timetick = dwalltime();

    promedioL = 0;
    promedioU = 0;
    for(i=0;i<filas;i++){   //Calcula los promedios
       for(j=0;j<N;j++){
           promedioL+= l[i*N+j];
           promedioU+= U[i*N+j];
       }
    }
    MPI_Allreduce(&promedioL, &resultadoL, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    MPI_Allreduce(&promedioU, &resultadoU, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    promedioL = resultadoL/(N*N);
    promedioU = resultadoU/(N*N);
    promedioL = promedioL*promedioU; //en promedioL queda el producto de ambos promedios

    for(i=0;i<filas;i++){   //AB = a*B
       for(j=0;j<N;j++){
            AB[i*N+j]=0;
            for(k=0;k<N;k++){
	            AB[i*N+j]= AB[i*N+j] + a[i*N+k]*B[k+j*N];
            }
       }
    }
    for(i=0;i<filas;i++){   //LC = l*C
       for(j=0;j<N;j++){
            LC[i*N+j]=0;
            for(k=0;k<N;k++){
	            LC[i*N+j]= LC[i*N+j] + l[i*N+k]*C[k+j*N];
            }
       }
    }
    for(i=0;i<filas;i++){   //DU = d*U
       for(j=0;j<N;j++){
            DU[i*N+j]=0;
            for(k=j;k<N;k++){
	            DU[i*N+j]= DU[i*N+j] + d[i*N+k]*U[k+j*N];
            }
       }
    }
    for(i=0;i<filas;i++){   //total = (AB+LC+DU)*promedioL
       for(j=0;j<N;j++){
	        total[i*N+j]= (AB[i*N+j] + LC[i*N+j] + DU[i*N+j])*promedioL;
       }
    }

    printf("Tiempo en segundos del worker %d:  %f \n", ID, dwalltime() - timetick);

    MPI_Gather(total, filas*N, MPI_DOUBLE, TOTAL, filas*N, MPI_DOUBLE, 0, MPI_COMM_WORLD); //Cada proceso envia su pedacito de matriz, las recibe el proceso root en TOTAL
    
    free(B);
    free(C);
    free(U);
    free(a);
    free(l);
    free(d);
    free(AB);
    free(LC);
    free(DU);
    free(total);
}
