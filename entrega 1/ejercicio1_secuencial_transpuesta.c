#include<stdio.h>
#include<stdlib.h>
#include <sys/time.h>

//Variable para tiempo
double dwalltime();

//Funcion para calcular el tiempo
double dwalltime(){
	double sec;
	struct timeval tv;
	gettimeofday(&tv,NULL);
	sec = tv.tv_sec + tv.tv_usec/1000000.0;
	return sec;
}

int main(int argc,char*argv[]){
 
  //Controla los argumentos al programa
  if (argc < 2){
    printf("\n Faltan argumentos \n");
    return 0;
  }
  //variables  
  double *A, *At,*R;
  double temp; 
  int i,j,k,N;
  int check=1;
  double timetick;
  N=atoi(argv[1]);
 
  //Aloca memoria para las matrices
  A=(double*)malloc(sizeof(double)*N*N);
  At=(double*)malloc(sizeof(double)*N*N);
  R=(double*)malloc(sizeof(double)*N*N);

  //Inicializa la matriz A en 1, el resultado sera una matriz R con todos sus valores en N
  for(i=0;i<N;i++){
    for(j=0;j<N;j++){
      A[i*N+j]=1;
      At[i*N+j]=A[i*N+j];
    }
  }

  
  timetick = dwalltime(); 
  
  //Genera matriz transpuesta
  for(i=0;i<N;i++){
    for(j=0;j<N;j++){
      temp = At[i*N+j];
      At[i*N+j]= At[j*N+i];
      At[j*N+i]= temp;
    }
  }   

  //Realiza la multiplicacion

  for(i=0;i<N;i++){
    for(j=0;j<N;j++){
        R[i*N+j]=0;
        for(k=0;k<N;k++){
            R[i*N+j]= R[i*N+j] + A[i*N+k]*At[k+j*N];
          }
      }
  }  
  printf("Tiempo en segundos %f \n", dwalltime() - timetick);

  //Verifica el resultado
  for(i=0;i<N;i++){
    for(j=0;j<N;j++){
        check=check&&(R[i*N+j]==N);
    }
  }  
  if(check){
    printf("Multiplicacion de matrices resultado correcto\n");
    }else{
      printf("Multiplicacion de matrices resultado erroneo\n");
    }
  free(A);
  free(R);
  free(At);
 
   return(0);
}