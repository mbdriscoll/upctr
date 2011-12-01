#include <upctr_util.h>

/*
 * Knobs for controlling the dimensions of the matrices.
 * Just set N to work with square matrices. For non-
 * square matrices, set X, Y, and Z.
 * N =  512 =>  5 sec
 * N =  768 => 15 sec
 * N = 1024 => 36 sec
 */

/* (N x N) * (N x N) -> (N x N) */
#define N 512

/* (X x Y) * (Y x Z) -> (X x Z) */
#define X N
#define Y N
#define Z N

/* (ax x ay) * (bx x by) -> (cx x cz) */
#define ax X
#define ay Y
#define bx Y
#define by Z
#define cx X
#define cy Z


/*
 * Double precision general matrix multiply.
 * Matrix dimensions must agree.
 * Sets C = AB.
 */
void dgemm(shared double* C, size_t Cx, size_t Cy,
           shared double* A, size_t Ax, size_t Ay,
           shared double* B, size_t Bx, size_t By) {
    assert(Ay == Bx);
    assert(Ax == Cx);
    assert(By == Cy);

    int i, j, k;
    for (i = 0; i < Ax; i++) {
        for (j = 0; j < Ay; j++) {
            upc_forall (k = 0; k < Ay; k++; &C[i*cx+j]) {
                C[i*cx+j] = C[i*cx+j] + A[i*ax+k] * B[k*bx+j];
            }
        }
    }
}

/*
 * Main. Initializes matrices and runs dgemm.
 */
int main(int argc, char* argv[]) {
    shared double* A = upctr_init_mat(ax, ay, UPCTR_INIT_INDEX);
    shared double* B = upctr_init_mat(bx, by, UPCTR_INIT_IDENT);
    shared double* C = upctr_init_mat(cx, cy, UPCTR_INIT_ZERO);

    dgemm(C, cx, cy, A, ax, ay, B, bx, by);

    return 0;
}

