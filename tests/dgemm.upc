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
