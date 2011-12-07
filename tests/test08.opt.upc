#include <upc.h>

#define N 16
#define T 1000000

// stencil radius. #elems = (2S+1)^2
#define S 3

shared [N*N/THREADS] double A[N][N];
shared [N*N/THREADS] double B[N][N];

int main() {

    int i, j, dj, di;
    upc_forall (i = 0; i < N; i++; &A[i][0]) {
        for (j = 0; j < N; j++) {
            A[i][j] = (i<S||j<S) ? 1.0 : 0.0;
        }
    }

    upc_barrier;

    int t;
    double tmp;
    double stencil[2*S+1];
    for (t = 0; t < T; t++) {

        upc_forall (i = S; i < N-S; i++; &A[i][0]) {
            for (j = S; j < N-S; j++) {
                B[i][j] = 0.0;
                for (di = -S; di <= S; di++) {
                    upc_memget(&stencil[0], &A[i+di][j-S], (2*S+1)*sizeof(double));
                    for (dj = -S; dj <= S; dj++)
                        B[i][j] += stencil[dj+S];
                }
            }
        }

        upc_barrier;

        upc_forall (i = S; i < N-S; i++; &A[i][0])
            for (j = S; j < N-S; j++)
                A[i][j] = B[i][j] / (double) ((2*S+1)*(2*S+1));
    }

#ifdef UPCTR_DEBUG
    if (MYTHREAD == 0) {
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                printf(" %f,", A[i][j]);
                //printf(" %d,", upc_threadof( &A[i][j]));
            }
            printf("\n");
        }
    }
#endif
}

