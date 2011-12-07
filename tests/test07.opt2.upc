#include <upc.h>

#define N 64
#define T 100000

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

            double A_local_im[N];
            upc_memget(&A_local_im, &A[i-1][0], N*sizeof(double));

            double A_local_ip[N];
            upc_memget(&A_local_ip, &A[i+1][0], N*sizeof(double));

            for (j = 1; j < N-1; j++) {

                double c = A[i][j];
                double e = A[i][j+i];
                double w = A[i][j-1];
                double n = A_local_ip[j];
                double s = A_local_im[j];
                double ne = A_local_ip[j+1];
                double nw = A_local_ip[j-1];
                double se = A_local_im[j+1];
                double sw = A_local_im[j-1];

                upc_barrier;

                A[i][j] = (n+s+e+w+c+ne+nw+se+sw) / 9.0;
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

