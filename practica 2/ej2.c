#include <pthread.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>


int elementosThread;
int *arreglo;
unsigned long ocurrencias = 0;
int elementoABuscar;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

double dwalltime(){
    double sec;
    struct timeval tv;

    gettimeofday(&tv,NULL);
    sec = tv.tv_sec + tv.tv_usec/1000000.0;
    return sec;
}
void* contador(void *id){
    int idLocal = *(int*)id;
    int posInicial = idLocal*elementosThread;
    int posFinal = (idLocal+1)*elementosThread;
    int ocurrenciasLocal = 0;
    for (int x = posInicial;x<posFinal;x++){
        if(arreglo[x]==elementoABuscar) ocurrenciasLocal++;
    }
    pthread_mutex_lock(&mutex);
    ocurrencias = ocurrencias + ocurrenciasLocal;
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}


int main(int argc, char* argv[]){
    unsigned long N = atol(argv[1]);
    arreglo = (int*)malloc(sizeof(int)*N);
    elementoABuscar = atol(argv[2]);
    pthread_t T[4];
    int threads[4];
    srand(time(NULL));   
    for (int i=0; i<N; i++){
        arreglo[i]= rand() % 10; //Inicializa arreglo
     }
    elementosThread = N/2;
    for (int id=0;id<2; id++){  //crea 2 threads
        threads[id]=id;
        pthread_create(&T[id],NULL,&contador,(void*)&threads[id]);
    }
    pthread_join(T[0],NULL);
    pthread_join(T[1],NULL);
    printf("Cantidad de ocurrencias del elemento: %lu  \n",ocurrencias);
    ocurrencias = 0;
    elementosThread = N/4;
    for (int id=0;id<4; id++){  //crea 4 threads
        threads[id]=id;
        pthread_create(&T[id],NULL,&contador,(void*)&threads[id]);
    }
    pthread_join(T[0],NULL);
    pthread_join(T[1],NULL);
    pthread_join(T[2],NULL);
    pthread_join(T[3],NULL);
    printf("Cantidad de ocurrencias del elemento: %lu  \n",ocurrencias);
}