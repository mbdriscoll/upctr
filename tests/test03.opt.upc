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
#define N 4

#include <upctr_util.h>

/*
 * Double precision general matrix multiply.
 * Matrix dimensions must agree.
 * Sets C = AB.
 */
void dgemm(shared [N] double ** C,
           shared [N] double ** A,
           shared [1] double ** B) {
    int i, j, k;

    upc_forall (i = 0; i < N; i++; &C[i][0]) {

        /* memget C */
        double C_local[N];
        bupc_memget_fstrided(
            &C_local, sizeof(double), sizeof(double), N,
            C[i], sizeof(double), sizeof(double), N
        );

#if 0
        /* memget A */
        double A_local[N];
        bupc_memget_fstrided(
            &A_local, sizeof(double), sizeof(double), N,
            &A[i], sizeof(double), sizeof(double), N
        );

        for (j = 0; j < N; j++) {

            /* memget B */
            double B_local[N];
            size_t dststridesB[] = {sizeof(double)};
            size_t srcstridesB[] = {sizeof(double)};
            size_t countB[] = {sizeof(double), N};
            size_t stridelevelsB = 1;
            bupc_memget_strided(B_local, dststridesB,
                                &B[0][j], srcstridesB,
                                countB, stridelevelsB);

            for (k = 0; k < N; k++) {
                C_local[k] = C_local[k] + A_local[k]* B_local[k];
            }
        }
#endif

        /* memput C */
        bupc_memput_fstrided(
            C[i], sizeof(double), sizeof(double), N,
            &C_local, sizeof(double), sizeof(double), N
        );
    }
}

/*
 * Main. Initializes matrices and runs dgemm.
 */
int main(int argc, char* argv[]) {
    shared [N] double ** A = (shared [N] double **) upc_all_alloc(N*N, sizeof(double));
    shared [1] double ** B = (shared [1] double **) upc_all_alloc(N*N, sizeof(double));
    shared [N] double ** C = (shared [N] double **) upc_all_alloc(N*N, sizeof(double));

    upctr_init((shared double **) A, UPCTR_INIT_INDEX);
    upctr_init((shared double **) B, UPCTR_INIT_IDENT);
    upctr_init((shared double **) C, UPCTR_INIT_ZERO);

    upc_barrier;

    START_TIMER;
    dgemm(C, A, B);
    END_TIMER;

    return 0;
}

