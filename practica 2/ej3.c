#include <pthread.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>


int elementosThread;
int *arreglo;
int menor = 9999;
int mayor = -1;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

double dwalltime(){
    double sec;
    struct timeval tv;

    gettimeofday(&tv,NULL);
    sec = tv.tv_sec + tv.tv_usec/1000000.0;
    return sec;
}
void* encontrarMin(void *id){
    int idLocal = *(int*)id;
    int posInicial = idLocal*elementosThread;
    int posFinal = (idLocal+1)*elementosThread;
    int menorLocal = 9999;
	int mayorLocal = -1;
    for (int x = posInicial;x<posFinal;x++){
        if(arreglo[x]< menorLocal) menorLocal = arreglo[x];
		if(arreglo[x]> mayorLocal) mayorLocal = arreglo[x];
    }
    pthread_mutex_lock(&mutex);
	if (menorLocal < menor) menor = menorLocal;
	if (mayorLocal > mayor) mayor = mayorLocal;
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}


int main(int argc, char* argv[]){
    unsigned long N = atol(argv[1]);
    arreglo = (int*)malloc(sizeof(int)*N);
    pthread_t T[4];
    int threads[4];
    srand(time(NULL));   
    for (int i=0; i<N; i++){
        arreglo[i]= rand() % 10; //Inicializa arreglo
     }
    elementosThread = N/2;
    for (int id=0;id<2; id++){  //crea 2 threads
        threads[id]=id;
        pthread_create(&T[id],NULL,&encontrarMin,(void*)&threads[id]);
    }
    pthread_join(T[0],NULL);
    pthread_join(T[1],NULL);
    printf("Valor minimo: %d  \n",menor);
    elementosThread = N/4;
    for (int id=0;id<4; id++){  //crea 4 threads
        threads[id]=id;
        pthread_create(&T[id],NULL,&encontrarMin,(void*)&threads[id]);
    }
    pthread_join(T[0],NULL);
    pthread_join(T[1],NULL);
    pthread_join(T[2],NULL);
    pthread_join(T[3],NULL);
    printf("Valor maximo: %d  \n",mayor);
}
