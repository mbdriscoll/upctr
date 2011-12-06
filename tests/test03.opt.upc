#include <upc.h>
#include <upc_collective.h>
#include <bupc_extensions.h>

/*
 * Knobs for controlling the dimensions of the matrices.
 * Just set N to work with square matrices. For non-
 * square matrices, set X, Y, and Z.
 * N =  512 =>  5 sec
 * N =  768 => 15 sec
 * N = 1024 => 36 sec
 */

/* (N x N) * (N x N) -> (N x N) */
#define N 2000

#include <upctr_util.h>

shared [N] double A[N][N];
shared [1] double B[N][N];
shared [N] double C[N][N];

/*
 * Double precision general matrix multiply.
 * Matrix dimensions must agree.
 * Sets C = AB.
 */
void dgemm() {

    int i, j, k;
    upc_forall (i = 0; i < N; i++; &C[i][0]) {
        for (j = 0; j < N; j++) {
            /* memget B */
            double B_local[N];
            bupc_memget_fstrided( &B_local, sizeof(double), sizeof(double), N,
                                  &B[0][j], sizeof(double), sizeof(double), N);

            for (k = 0; k < N; k++)
                C_local[k] = C_local[k] + A_local[k]* B_local[k];
        }
    }
}

/*
 * Main. Initializes matrices and runs dgemm.
 */
int main(int argc, char* argv[]) {
    upctr_init((shared double *) &A, UPCTR_INIT_INDEX);
    upctr_init((shared double *) &B, UPCTR_INIT_IDENT);
    upctr_init((shared double *) &C, UPCTR_INIT_ZERO);

    upc_barrier;

    START_TIMER;
    dgemm();
    END_TIMER;

#if 0
    if (MYTHREAD == 0) {
        upctr_print_mat("A", (shared double *) &A);
        upctr_print_mat("B", (shared double *) &B);
        upctr_print_mat("C", (shared double *) &C);
    }
#endif

    return 0;
}

