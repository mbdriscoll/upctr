/*
 * This is the kernel from:
 * berkeley_upc-2.14.0/upc-tests/benchmarks/scale-poly.upc
 */

void scale_poly() {

    for (j = 0; j < sizeof(percent_ar) / sizeof (int); j++) {
        for (k = 0; k < iter; k++) {
            /* set up the index array */
            build_index(percent_ar[j] / 100.0);

            for (i = 0; i < ind_ar_size; i++) {
                /* one read + one update*/
                int l;
                double tmp = array[index_array[i]].x;
                res = 0.0;
                for (l = 0; l < poly_deg; l++) {
                    res += (poly_deg - l) * pow(tmp, l);
                }
                array[index_array[i]].y = res;
            }
        }
        upc_barrier;
    }

}
