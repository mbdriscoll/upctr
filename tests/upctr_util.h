#include <upc.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

typedef enum upctr_init {
    UPCTR_INIT_RAND,
    UPCTR_INIT_ZERO,
    UPCTR_INIT_ONE,
    UPCTR_INIT_IDENT,
    UPCTR_INIT_INDEX,
    UPCTR_INIT_NONE
} upctr_init_t;

double rand_double() {
    return (double) rand();
}

shared double* upctr_init_mat(int dx, int dy, upctr_init_t init_t) {
    shared double* M = (shared double*) upc_all_alloc(dx*dy, sizeof(double));

    int i, j;
    for (i = 0; i < dx; i++) {
        upc_forall (j = 0; j < dy; j++; &M[i*dx+j]) {
            switch(init_t) {
                case UPCTR_INIT_RAND:  M[i*dx+j] = rand_double();     break;
                case UPCTR_INIT_ZERO:  M[i*dx+j] = 0.0;               break;
                case UPCTR_INIT_ONE:   M[i*dx+j] = 1.0;               break;
                case UPCTR_INIT_IDENT: M[i*dx+j] = (i==j) ? 1.0: 0.0; break;
                case UPCTR_INIT_INDEX: M[i*dx+j] = i*dx+j;            break;
                case UPCTR_INIT_NONE:                                 break;
                default: assert(!"Unreachable");
            }
        }
    }

    return M;
}

void upctr_print_mat(char* name, shared double* M, int dx, int dy) {
    printf("%s:\n", name);

    int i, j;
    for (i = 0; i < dx; i++) {
        for (j = 0; j < dy; j++) {
            if (j != 0)
                printf(", ");
            printf("%2.0f", M[i*dx+j]);
        }
        printf("\n");
    }
    printf("\n");
}
