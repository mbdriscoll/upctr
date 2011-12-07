#include <upc.h>

#define N 512
#define T 1000

shared [N*N/THREADS] double A[N][N];

int main() {

    int i, j;
    upc_forall (i = 0; i < N; i++; &A[i][0]) {
        for (j = 0; j < N; j++) {
            A[i][j] = (i==0||j==0) ? 1.0 : 0.0;
        }
    }

    upc_barrier;

    int t;
    double n, s, e, w, c, nw, ne, sw, se;
    for (t = 0; t < T; t++) {
        upc_forall (i = 1; i < N-1; i++; &A[i][0]) {
            for (j = 1; j < N-1; j++) {
                n = A[i-1][j];
                s = A[i+1][j];
                e = A[i][j+1];
                w = A[i][j-1];
                c = A[i][j];
                ne = A[i+1][j+1];
                nw = A[i+1][j-1];
                se = A[i-1][j+1];
                sw = A[i-1][j-1];
                upc_barrier;
                A[i][j] = (n+s+e+w+c+nw+ne+sw+se) / 9.0;
            }
        }
        upc_barrier;
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

