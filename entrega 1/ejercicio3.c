#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

int *arreglo;

double dwalltime(){
    double sec;
    struct timeval tv;
    gettimeofday(&tv,NULL);
    sec = tv.tv_sec + tv.tv_usec/1000000.0;
    return sec;
}

int main(int argc, char* argv[]){
	if(argc<3){
		printf("Error: faltan argumentos \n");
		return 0;	
	}
    unsigned long N = atol(argv[1]);
    arreglo = (int*)malloc(sizeof(int)*N);
    int numThreads = atol(argv[2]);
    omp_set_num_threads(numThreads);
    int pares = 0;
    srand(time(NULL));   
    for (int i=0; i<N; i++){
        arreglo[i]= rand() % 10; //Inicializa arreglo
    }
    // ****** Versión secuencial ******
    double timetick = dwalltime();
    for(int i=0;i<N;i++) if((arreglo[i] & 1) == 0) pares++;
    printf("Tiempo: %f \n", dwalltime() - timetick);
    printf("Cantidad de numeros pares en el arreglo: %d \n", pares);
    pares = 0;
    // ****** Versión openMP ******
	timetick = dwalltime();
    #pragma omp parallel for reduction(+ : pares)
        for(int i=0;i<N;i++) if((arreglo[i] & 1) == 0) pares++;
    free(arreglo);
    printf("Tiempo: %f \n", dwalltime() - timetick);
    printf("Cantidad de numeros pares en el arreglo: %d \n", pares);

    return 0;
}
