#include <upc.h>

#define N 100
#define T 10000

shared [N] double A[N][N];

int main() {

    int i, j;
    upc_forall (i = 0; i < N; i++; &A[i][0]) {
        for (j = 0; j < N; j++) {
            A[i][j] = (i==0||j==0) ? 1.0 : 0.0;
        }
    }

    upc_barrier;

    int t;
    for (t = 0; t < T; t++) {
        upc_forall (i = 1; i < N-1; i++; &A[i][0]) {

            double A_local_i[N];
            bupc_memget_fstrided( &A_local_i, sizeof(double), sizeof(double), N,
                                  &A[i], sizeof(double), sizeof(double), 3);


            for (j = 1; j < N-1; j++) {

                double A_local_j[3];
                bupc_memget_fstrided( &A_local_j, sizeof(double), sizeof(double), 3,
                                      &A[i][j-1], sizeof(double), N*sizeof(double), 3);

                double n = A_local_j[i-1];
                double s = A_local_j[i+1];
                double e = A_local_i[j+1];
                double w = A_local_i[j-1];
                double c = A_local_i[j];

                upc_barrier;

                A[i][j] = (n+s+e+w+c) / 5.0;
            }
        }
        upc_barrier;
    }

#ifdef DEBUG

    upc_barrier;

    if (MYTHREAD == 0) {
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                printf(" %1.3f,", A[i][j]);
            }
            printf("\n");
        }
    }
#endif
}

