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
void dgemm(shared [N] double * C,
           shared [N] double * A,
           shared [N] double * B) {
    int i, j, k;
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {

            double A_local[N];

            size_t dststrides[] = {sizeof(double)};
            size_t srcstrides[] = {N*sizeof(double)};

            size_t count[] = {N*sizeof(double)};
            size_t stridelevels = 0;

            bupc_memget_strided(A_local, dststrides,
                                &A[i*N], srcstrides,
                                count, stridelevels);

            upc_forall (k = 0; k < N; k++; &C[i*N+j]) {
                C[i*N+j] = C[i*N+j] + A_local[k] * B[k*N+j];
            }
        }
    }
}

/*
 * Main. Initializes matrices and runs dgemm.
 */
int main(int argc, char* argv[]) {
    shared [N] double * A = upctr_init_mat(UPCTR_INIT_INDEX);
    shared [N] double * B = upctr_init_mat(UPCTR_INIT_IDENT);
    shared [N] double * C = upctr_init_mat(UPCTR_INIT_ZERO);

    dgemm(C, A, B);

    if (MYTHREAD == 0) {
        upctr_print_mat("A", A);
        upctr_print_mat("B", B);
        upctr_print_mat("C", C);
    }

    return 0;
}

