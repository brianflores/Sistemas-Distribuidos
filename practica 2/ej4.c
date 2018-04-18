#include <pthread.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>


int elementosThread;
int *arreglo;

double dwalltime(){
    double sec;
    struct timeval tv;
    gettimeofday(&tv,NULL);
    sec = tv.tv_sec + tv.tv_usec/1000000.0;
    return sec;
}
void* ordenar(void *id){
    int idLocal = *(int*)id;
	int *arregloLocal = (int*)malloc(sizeof(int)*elementosThread);
	int i;
	for (i=0; i<elementosThread; i++) arregloLocal[i] = 999;
    int posInicial = idLocal*elementosThread;
    int posFinal = (idLocal+1)*elementosThread;
	int x; 
    for (x= posInicial;x<posFinal;x++){
		i=0;
		while(arreglo[x] >= arregloLocal[i]) i++;
		arregloLocal[i] = arreglo[x];
    }
	i=0;
    for (x= posInicial;x<posFinal;x++){
		arreglo[x]= arregloLocal[i];
		i++;
    }
	free(arregloLocal);
    pthread_exit(NULL);
}


int main(int argc, char* argv[]){	
	printf("test");
    unsigned long N = atol(argv[1]);	
    arreglo = (int*)malloc(sizeof(int)*N);
    pthread_t T[4];
    int threads[4];
    srand(time(NULL));   
    for (int i=0; i<N; i++){
        arreglo[i]= rand() %10; //Inicializa arreglo
    }
    elementosThread = N/2;
    for (int id=0;id<2; id++){  //crea 2 threads
        threads[id]=id;
        pthread_create(&T[id],NULL,&ordenar,(void*)&threads[id]);
    }
    pthread_join(T[0],NULL);
    pthread_join(T[1],NULL);
	elementosThread = N;
	pthread_create(&T[0],NULL,&ordenar,(void*)&threads[0]);
    pthread_join(T[0],NULL);
	printf("Ordenamiento 2 hilos: ");
	for (int x=0;x<N;x++) printf("%d ",arreglo[x]);
	printf("\n");
    elementosThread = N/4;
    for (int id=0;id<4; id++){  //crea 4 threads
        threads[id]=id;
        pthread_create(&T[id],NULL,&ordenar,(void*)&threads[id]);
    }
    pthread_join(T[0],NULL);
    pthread_join(T[1],NULL);
    pthread_join(T[2],NULL);
    pthread_join(T[3],NULL);
    elementosThread = N/2;
	for (int id=0;id<2; id++){  //crea 2 threads
        threads[id]=id;
        pthread_create(&T[id],NULL,&ordenar,(void*)&threads[id]);
    }
    pthread_join(T[0],NULL);
    pthread_join(T[1],NULL);
	elementosThread = N;
	pthread_create(&T[0],NULL,&ordenar,(void*)&threads[0]);
    pthread_join(T[0],NULL);
	printf("Ordenamiento 4 hilos: ");
	for (int x=0;x<N;x++) printf("%d ",arreglo[x]);
	printf("\n");
}
