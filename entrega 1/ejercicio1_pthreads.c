#include <pthread.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>

//Dimension por defecto de las matrices
int i,j,k;
int N, elementosThread;
double *A, *At,*R;
pthread_barrier_t barrera;


//Para calcular tiempo
double dwalltime(){
  double sec;
  struct timeval tv;
  gettimeofday(&tv,NULL);
  sec = tv.tv_sec + tv.tv_usec/1000000.0;
  return sec;
}

void* multiplicacion(void *id){
  int idLocal = *(int*)id;
  int posInicial = idLocal*elementosThread;
  int posFinal = (idLocal+1)*elementosThread;
  double temp;
	
//Genera matriz transpuesta
      
	for(i=posInicial;i<posFinal;i++){
	  for(j=i+1;j<N;j++){
	  	temp = At[i*N+j];
			At[i*N+j]= At[j*N+i];
			At[j*N+i]= temp;
	  }
	}   
  pthread_barrier_wait(&barrera);

  for(int i=posInicial;i<posFinal;i++){
 	for(int j=0;j<N;j++){
        R[i*N+j]=0;
        for(int k=0;k<N;k++){
            R[i*N+j]+=A[i*N+k]*At[k+j*N];
        }
 	  }
  }
  pthread_exit(NULL);
}


int main(int argc,char*argv[]){

//Controla los argumentos al programa
if (argc < 3){
  printf("\n Faltan argumentos \n");
  return 0;
}

N= atoi(argv[1]);
int numthread= atoi(argv[2]);

int check=1;
double timetick;
pthread_t T[numthread];
int threads[numthread];
pthread_barrier_init(&barrera, NULL, numthread);

//Aloca memoria para las matrices
A=(double*)malloc(sizeof(double)*N*N);
At=(double*)malloc(sizeof(double)*N*N);
R=(double*)malloc(sizeof(double)*N*N);
 

//Inicializa las matrices A y B en 1, el resultado sera una matriz con todos sus valores en N
for(i=0;i<N;i++){
  for(j=0;j<N;j++){
  	A[i*N+j]=1;
	At[i*N+j]=A[i*N+j];
  }
}  

timetick = dwalltime();

//crea threads
elementosThread = N/numthread;

for (int id=0;id<numthread; id++){  
  threads[id]=id;
  pthread_create(&T[id],NULL,&multiplicacion,(void*)&threads[id]);
}

for (int id=0;id<numthread; id++){ 
  pthread_join(T[id],NULL);
 }

printf("Tiempo en segundos %f\n", dwalltime() - timetick);

pthread_barrier_destroy(&barrera);

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
free(At);
free(R);
return(0);
}


