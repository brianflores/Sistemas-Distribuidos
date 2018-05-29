#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

int elementosThread;
unsigned long Total,N;
double *A, *At, *B, *C, *D, *E, *F, *L, *U, *AA, *AAC, *LB, *LBE, *DU, *DUF, *TOTAL;
double promedioB=0, promedioU=0, promedioL=0;
pthread_barrier_t barrera,barrera1, barrera2;
pthread_mutex_t miMutex;

double dwalltime(){
  double sec;
  struct timeval tv;
  gettimeofday(&tv,NULL);
  sec = tv.tv_sec + tv.tv_usec/1000000.0;
  return sec;
}

void* multiplicacion(void *id){
    int i,j,k;
    int idLocal = *(int*)id;
    int posInicial = idLocal*elementosThread;
    int posFinal = (idLocal+1)*elementosThread;
    double temp, auxB=0,auxL=0,auxU=0;;

 
    for(i=posInicial;i<posFinal;i++){   //Calcula los promedios
        for(j=0;j<N;j++){
                auxB+= B[i*N+j];
                auxL+= L[i*N+j];
                auxU+= U[i*N+j];
        }
    }
        pthread_mutex_lock(&miMutex);
            promedioB+=auxB;
            promedioL+=auxL;
            promedioU+=auxU;
        pthread_mutex_unlock(&miMutex);
    
    //Genera matriz transpuesta
	for(i=posInicial;i<posFinal;i++){
	  for(j=0;j<N;j++){
	    	temp = At[i*N+j];
			At[i*N+j]= At[j*N+i];
			At[j*N+i]= temp;
	  }
	}
    
    pthread_barrier_wait(&barrera); 

    if(idLocal==0){   //el hilo 0 calcula los promedios
        promedioB = promedioB / Total;
        promedioL = promedioL / Total;
        promedioU = promedioU / Total;
        promedioL = promedioL * promedioU; //En promedio L queda el promedio de L por el de U.
    }

    for(int i=posInicial;i<posFinal;i++){
 	    for(int j=0;j<N;j++){
            AA[i*N+j]=0;
            for(int k=0;k<N;k++){
               AA[i*N+j]= AA[i*N+j] + A[i*N+k]*At[k+j*N]; //AA=A*At
            }
        }
    }

    for(int i=posInicial;i<posFinal;i++){
 	    for(int j=0;j<N;j++){
            LB[i*N+j]=0;
            for(int k=0;k<=j;k++){
		        LB[i*N+j]= LB[i*N+j] + B[i*N+k]*L[k+j*N];//LB= L*B
	        }
        }
    }

    for(int i=posInicial;i<posFinal;i++){
 	    for(int j=0;j<N;j++){
            LBE[i*N+j]=0;
            for(int k=j;k<N;k++){
		        DU[i*N+j]= DU[i*N+j] + D[i*N+k]*U[k+j*N];//DU= D*U		
	        }
        }    
     }


    pthread_barrier_wait(&barrera1); 


    for(int i=posInicial;i<posFinal;i++){
 	    for(int j=0;j<N;j++){
           AAC[i*N+j]=0;
           for(int k=0;k<N;k++){
		        AAC[i*N+j]= AAC[i*N+j] + AA[i*N+k]*C[k+j*N]*promedioL;//AAC=(AA*C)*promedio de L y U
	        }
        }
    }

    for(int i=posInicial;i<posFinal;i++){
 	    for(int j=0;j<N;j++){
            LBE[i*N+j]=0;
            for(int k=0;k<N;k++){
		       LBE[i*N+j]= LBE[i*N+j] + LB[i*N+k]*E[k+j*N]*promedioB;//LBE= (LB*E)*promedio B
	        }
        }    
    }

    for(int i=posInicial;i<posFinal;i++){
 	    for(int j=0;j<N;j++){
            DUF[i*N+j]=0;
            for(int k=0;k<N;k++){
		        DUF[i*N+j]= DUF[i*N+j] + DU[i*N+k]*F[k+j*N]*promedioB;	//DUF= (DU*F)*promedio B	
	        }
        }     
    }

    pthread_barrier_wait(&barrera2);
    for(i=posInicial;i<posFinal;i++){
        for(j=0;j<N;j++)
            TOTAL[i*N+j]= AAC[i*N+j] + LBE[i*N+j] + DUF[i*N+j]; //TOTAL= AAC + LBE + DUF
    }
    pthread_exit(NULL);
}


int main(int argc,char*argv[]){
    if (argc < 3){
        printf("Faltan argumentos \n");
        return 0;
    }
    int check=1,i,j,k;
    double timetick;
    N = atol(argv[1]);
    int numthread=atol(argv[2]);;
    pthread_t T[numthread];
    int threads[numthread];
    Total = N*N;

    pthread_mutex_init(&miMutex, NULL);
    
    pthread_barrier_init(&barrera, NULL, numthread);
    pthread_barrier_init(&barrera1, NULL, numthread);
    pthread_barrier_init(&barrera2, NULL, numthread);

    A=(double*)malloc(sizeof(double)*N*N);
    At=(double*)malloc(sizeof(double)*N*N);
    B=(double*)malloc(sizeof(double)*N*N);
    C=(double*)malloc(sizeof(double)*N*N);
    D=(double*)malloc(sizeof(double)*N*N);
    E=(double*)malloc(sizeof(double)*N*N);
    F=(double*)malloc(sizeof(double)*N*N);
    L=(double*)malloc(sizeof(double)*N*N);
    U=(double*)malloc(sizeof(double)*N*N);
    AA=(double*)malloc(sizeof(double)*N*N); //AA=A*At
    AAC=(double*)malloc(sizeof(double)*N*N); //AAC=(AA*C)*promedio de L y U
    LB=(double*)malloc(sizeof(double)*N*N); //LB= L*B
    LBE=(double*)malloc(sizeof(double)*N*N); //LBE= (LB*E)*promedio B
    DU=(double*)malloc(sizeof(double)*N*N); //DU= D*U
    DUF=(double*)malloc(sizeof(double)*N*N); //DUF= (DU*F)*promedio B
    TOTAL=(double*)malloc(sizeof(double)*N*N); //TOTAL= AAC + LBE + DUF

    for(i=0;i<N;i++){       //Crea matrices
       for(j=0;j<N;j++){
           A[i*N+j]=1.0;
	       At[i*N+j]=1.0;
           B[i*N+j]=1.0;
           C[i*N+j]=1.0;
           D[i*N+j]=1.0;
           E[i*N+j]=1.0;
           F[i*N+j]=1.0;
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
    elementosThread = N/numthread;
    
    for (int id=0;id<numthread; id++){
        threads[id]=id;
        pthread_create(&T[id],NULL,&multiplicacion,(void*)&threads[id]);
    }

    for (int id=0;id<numthread; id++){
        pthread_join(T[id],NULL);
    }

    printf("Tiempo en segundos %f \n", dwalltime() - timetick);
    
   
    //chequea resultado
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

    pthread_mutex_destroy(&miMutex);    

    pthread_barrier_destroy(&barrera);
    pthread_barrier_destroy(&barrera1);
    pthread_barrier_destroy(&barrera2);

    free(A);
    free(B);
    free(C);
    free(D);
    free(E);
    free(F);
    free(L);
    free(U);
    free(AA);
    free(AAC);
    free(LB);
    free(LBE);
    free(DU);
    free(DUF);
    free(TOTAL);

    return 0;
}