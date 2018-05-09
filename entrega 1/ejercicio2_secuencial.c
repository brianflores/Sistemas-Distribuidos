#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

double dwalltime()
{
  double sec;
  struct timeval tv;

  gettimeofday(&tv,NULL);
  sec = tv.tv_sec + tv.tv_usec/1000000.0;
  return sec;
}

double *A, *At, *B, *C, *D, *E, *F, *L, *U, *AA, *AAC, *LB, *LBE, *DU, *DUF, *TOTAL;

int main(int argc,char*argv[]){
    if (argc < 2){
        printf("Faltan argumentos \n");
        return 0;
    }
    int i, j, k;
    double promedioB, promedioU, promedioL, timetick, temp;
    unsigned long N = atol(argv[1]);
    unsigned long Total = N*N;
    A=(double*)malloc(sizeof(double)*N*N);
    //At=(double*)malloc(sizeof(double)*N*N);
    B=(double*)malloc(sizeof(double)*N*N);
    C=(double*)malloc(sizeof(double)*N*N);
    D=(double*)malloc(sizeof(double)*N*N);
    E=(double*)malloc(sizeof(double)*N*N);
    F=(double*)malloc(sizeof(double)*N*N);
    L=(double*)malloc(sizeof(double)*N*N);
    U=(double*)malloc(sizeof(double)*N*N);
    for(i=0;i<N;i++){       //Crea matrices
       for(j=0;j<N;j++){
           A[i*N+j]=1.0;
	       //At[i*N+j]=1.0;
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
    promedioB = 0;
    promedioL = 0;
    promedioU = 0;
    
    timetick = dwalltime();
    
    for(i=0;i<N;i++){   //Calcula los promedios
       for(j=0;j<N;j++){
           promedioB+= B[i*N+j];
           promedioL+= L[i*N+j];
           promedioU+= U[i*N+j];
       }
    }
    promedioB = promedioB / Total;
    promedioL = promedioL / Total;
    promedioU = promedioU / Total;
    promedioL = promedioL * promedioU; //En promedioL queda el promedio de L por el de U.
    //Genera matriz transpuesta
    /* for(i=0;i<N;i++){
	   for(j=i+1;j<N;j++){
			temp = At[i*N+j];
			At[i*N+j]= At[j*N+i];
			At[j*N+i]= temp;
	   }
	}  */
    AA=(double*)malloc(sizeof(double)*N*N); //AA=A*At
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            AA[i*N+j]=0;
            for(k=0;k<N;k++){
	            AA[i*N+j]= AA[i*N+j] + A[i*N+k]*A[k+j*N];
            }
        }
    }
    AAC=(double*)malloc(sizeof(double)*N*N); //AAC=(AA*C)*promedio de L y U
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            AAC[i*N+j]=0;
            for(k=0;k<N;k++){
	            AAC[i*N+j]= AAC[i*N+j] + AA[i*N+k]*C[k+j*N]*promedioL;
            }
        }
    }
    LB=(double*)malloc(sizeof(double)*N*N); //LB= L*B
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            LB[i*N+j]=0;
            for(k=0;k<=j;k++){
	            LB[i*N+j]= LB[i*N+j] + B[i*N+k]*L[k+j*N];
            }
        }
    }
    LBE=(double*)malloc(sizeof(double)*N*N); //LBE= (LB*E)*promedio B
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            LBE[i*N+j]=0;
            for(k=0;k<N;k++){
	            LBE[i*N+j]= LBE[i*N+j] + LB[i*N+k]*E[k+j*N]*promedioB;
            }
        }
    }
    DU=(double*)malloc(sizeof(double)*N*N); //DU= D*U
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            DU[i*N+j]=0;
            for(k=j;k<N;k++){
	            DU[i*N+j]= DU[i*N+j] + D[i*N+k]*U[k+j*N];
            }
        }
    }
    DUF=(double*)malloc(sizeof(double)*N*N); //DUF= (DU*F)*promedio B
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            DUF[i*N+j]=0;
            for(k=0;k<N;k++){
	            DUF[i*N+j]= DUF[i*N+j] + DU[i*N+k]*U[k+j*N]*promedioB;
            }
        }
    }
    TOTAL=(double*)malloc(sizeof(double)*N*N); //TOTAL= AAC + LBE + DUF
    for(i=0;i<N;i++){
        for(j=0;j<N;j++)
        TOTAL[i*N+j]= AAC[i*N+j] + LBE[i*N+j] + DUF[i*N+j];
    }

    printf("Tiempo en segundos %f \n", dwalltime() - timetick);

    
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

