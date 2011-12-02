#include <upc.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <limits.h>

typedef enum _upctr_init_t {
    UPCTR_INIT_RAND,
    UPCTR_INIT_ZERO,
    UPCTR_INIT_ONE,
    UPCTR_INIT_IDENT,
    UPCTR_INIT_INDEX,
    UPCTR_INIT_NONE
} upctr_init_t;

double rand_double() {
    return (double) rand() / (double) INT_MAX;
}

shared [N] double * upctr_init_mat(upctr_init_t init_t) {
    shared [N] double * M = (shared [N] double *) upc_all_alloc(N*N, sizeof(double));

    int i, j;
    for (i = 0; i < N; i++) {
        upc_forall (j = 0; j < N; j++; &M[i*N+j]) {
            switch(init_t) {
                case UPCTR_INIT_RAND:  M[i*N+j] = rand_double();     break;
                case UPCTR_INIT_ZERO:  M[i*N+j] = 0.0;               break;
                case UPCTR_INIT_ONE:   M[i*N+j] = 1.0;               break;
                case UPCTR_INIT_IDENT: M[i*N+j] = (i==j) ? 1.0: 0.0; break;
                case UPCTR_INIT_INDEX: M[i*N+j] = i*N+j;            break;
                case UPCTR_INIT_NONE:                                 break;
                default: assert(!"Unreachable");
            }
        }
    }
    upc_barrier;

    return M;
}

void upctr_print_mat(char* name, shared [N] double * M) {
    printf("%s:\n", name);

    int i, j;
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            if (j != 0)
                printf(", ");
            printf("%2.0f", M[i*N+j]);
        }
        printf("\n");
    }
    printf("\n");
}
