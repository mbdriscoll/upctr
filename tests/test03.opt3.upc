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
#define N 16

//#define S 128
#define S 4
//#define S 32
//#define S 16
//#define S 8
//#define S 4
//#define S 1

// Use powers of two for S <= N <= THREADS

#include <upctr_util.h>

shared [N] double A[N][N];
shared [1] double B[N][N];
shared [N] double C[N][N];

#define SWAP(a, b)  { a ^= b; b ^= a; a ^= b; }

/*
 * Double precision general matrix multiply.
 * Matrix dimensions must agree.
 * Sets C = AB.
 */
void dgemm() {
    int i, j, k, s;
    upc_forall (i = 0; i < N; i++; &C[i][0]) {
        for (j = 0; j < N; j++) {
            double Bla[S];
            double Blb[S];
            double *B_full = Bla;
            double *B_filling = Blb;
            double *tmp;
            bupc_handle_t ha;
            bupc_handle_t hb;
            size_t elem_size = sizeof(double);
            size_t local_stride = elem_size;
            size_t total_size = N*N*elem_size;
            size_t remote_stride = upc_affinitysize( total_size, sizeof(double), upc_threadof(&B[0][j]) ) / N;

            /* start filling the pipeline */
            ha = bupc_memget_fstrided_async(
                    B_full, elem_size, local_stride, S,
                    &B[k][j], elem_size, remote_stride, S);

            for (k = 0; k < N; k += S) {
                remote_stride = upc_affinitysize( total_size, sizeof(double), upc_threadof(&B[k][j]) ) / N;
                hb = bupc_memget_fstrided_async(
                        B_filling, elem_size, local_stride, S,
                        &B[k][j], elem_size, remote_stride, S);

                bupc_waitsync(ha);
                tmp = B_full;
                B_full = B_filling;
                B_filling = B_full;

                for (s = 0; s < S; s++) {
                    C[i][j] += A[i][k+s] * B_full[s];
                }

                ha = hb;
            }
        }
    }
}

/*
 * Main. Initializes matrices and runs dgemm.
 */
int main(int argc, char* argv[]) {
    assert( N % S == 0 );

    upctr_init((shared double *) &A, UPCTR_INIT_INDEX);
    upctr_init((shared double *) &B, UPCTR_INIT_IDENT);
    upctr_init((shared double *) &C, UPCTR_INIT_ZERO);

    upc_barrier;

    START_TIMER;
    dgemm();
    END_TIMER;

    upc_barrier;

#if UPCTR_DEBUG
    if (MYTHREAD == 0) {
        upctr_print_mat("A", (shared double *) &A);
        upctr_print_mat("B", (shared double *) &B);
        upctr_print_mat("C", (shared double *) &C);
    }
#else
    if (MYTHREAD == upc_threadof( &C[N-1][N-1] )) {
        assert(C[N-1][N-1] == N*N);
    }
#endif

    return 0;
}

