
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
    unsigned long N = atol(argv[1]);
    arreglo = (int*)malloc(sizeof(int)*N);
    srand(time(NULL));   
    for (int i=0; i<N; i++){
        arreglo[i]= rand() % 10; //Inicializa arreglo
    }
    int pares = 0;
    double timetick = dwalltime();
    for(int i=0;i<N;i++) if((arreglo[i] % 2) == 0) pares++;
    free(arreglo);
    printf("Tiempo: %f \n", dwalltime() - timetick);
    printf("Cantidad de numeros pares en el arreglo: %d \n", pares);
}