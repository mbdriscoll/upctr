/*
 * This is the kernel from:
 * berkeley_upc-2.14.0/upc-tests/benchmarks/matmult/mat-naive.upc
 */

void laplacian() {

    for (k = 0; k < iter; k++) {
        for (i = 0; i < ROWS; i++) {
            upc_forall(j = 0; j < COLS; j++; &matrix[i][j]) {
                up    = (i == 0)        ? 0 : matrix[i-1][j];
                down  = (i == ROWS-1)   ? 0 : matrix[i+1][j];
                left  = (j == 0)        ? 0 : matrix[i][j-1];
                right = (j == COLS - 1) ? 0 : matrix[i][j+1];

                tmp[i][j] = 4 * matrix[i][j] - up - down - left - right;
            } /* j */
        }
        upc_barrier;
        copyData();
        upc_barrier;
    }

}
