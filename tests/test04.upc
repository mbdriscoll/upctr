/*
 * This is the naive matmul kernel in:
 * berkeley_upc-2.14.0/upc-tests/benchmarks/matmult/mat-naive.upc
 */
upc_forall(i=0; i<N; i++; &a[i][0]) {
    for (j=0; j<M; j++) {
        sum = 0;
        for(l=0; l< P; l++)
            sum +=a[i][l]*b[l][j];
        c[i][j] += sum;
    }
}
