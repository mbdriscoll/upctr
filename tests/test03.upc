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
#define N 1000

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
    for (i = 0; i < N; i++) {
        upc_forall (j = 0; j < N; j++; &C[i][j]) {
            for (k = 0; k < N; k++) {
                C[i][j] = C[i][j] + A[i][k] + B[k][j];
            }
        }
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

