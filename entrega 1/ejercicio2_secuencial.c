
double *A, *B, *C, *D, *E, *F, *L, *U, *G, *H;

int main(int argc,char*argv[]){
    int i, j, k; 
    double promedioB, promedioU, promedioL;
    unsigned long N = atol(argv[1]);
    A=(double*)malloc(sizeof(double)*N*N);
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
           B[i*N+j]=1.0;
           C[i*N+j]=1.0;
           D[i*N+j]=1.0;
           E[i*N+j]=1.0;
           F[i*N+j]=1.0;
           if(i==j){
               L[i*N+j]= 1.0;
               U[i*N+j]= 1.0;
           } else if(i>j){
               L[i*N+j]= 1.0;
               U[i*N+j]= 0.0;
           } else {
               L[i*N+j]= 0.0;
               U[i*N+j]= 1.0;
           }
       }
    }
    promedioB =0;
    promedioL = 0;
    promedioU = 0;
    for(i=0;i<N;i++){
       for(j=0;j<N;j++){
           promedioB+= B[i*N+j];
           promedioL+= L[i*N+j];
           promedioU+= U[i*N+j];
       }
    }
    promedioB = promedioB / (N*N);
    promedioL = promedioL / (N*N);
    promedioU = promedioU / (N*N);

    G=(int*)malloc(sizeof(int)*N*N); //G=A*A
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            G[i*N+j]=0;
            for(k=0;k<N;k++){
	            G[i*N+j]= G[i*N+j] + A[i*N+k]*A[k+j*N];
            }
        }
    }
    free(A);
    H=(int*)malloc(sizeof(int)*N*N); //H=(G*C)*promedio de L y U
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            H[i*N+j]=0;
            for(k=0;k<N;k++){
	            H[i*N+j]= H[i*N+j] + G[i*N+k]*C[k+j*N];
                H[i*N+j] =  H[i*N+j] * promedioL * promedioU;   
            }
        }
    }
    free(C);
        



}

