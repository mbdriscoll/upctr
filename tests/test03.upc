#include <upc.h>
#include <upc_collective.h>
#include <bupc_extensions.h>

#include <dgemm.upc>

#include <upctr_util.h>

/*
 * Main. Initializes matrices and runs dgemm.
 */
int main(int argc, char* argv[]) {
    upctr_init((shared double *) &A, UPCTR_INIT_INDEX);
    upctr_init((shared double *) &B, UPCTR_INIT_IDENT);
    upctr_init((shared double *) &C, UPCTR_INIT_ZERO);

    upc_barrier;

    START_TIMER;
    dgemm();
    END_TIMER;

    return 0;
}

