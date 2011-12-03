#include <upc.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <limits.h>

#define START_TIMER bupc_tick_t start; \
    if (MYTHREAD == 0) start = bupc_ticks_now();

#define END_TIMER \
    if (MYTHREAD == 0) { \
        bupc_tick_t end = bupc_ticks_now(); \
        int ticks = (int) bupc_ticks_to_us(end-start); \
        printf("Time: %d milliseconds\n", ticks/1000); \
    }

typedef enum _upctr_init_t {
    UPCTR_INIT_RAND,
    UPCTR_INIT_ZERO,
    UPCTR_INIT_ONE,
    UPCTR_INIT_IDENT,
    UPCTR_INIT_INDEX,
    UPCTR_INIT_THREAD,
    UPCTR_INIT_NONE
} upctr_init_t;

double rand_double() {
    return (double) rand() / (double) INT_MAX;
}

void upctr_init(shared double * M, upctr_init_t init_t) {
    int i, j;
    for (i = 0; i < N; i++) {
        upc_forall (j = 0; j < N; j++; &M[i*N+j]) {
            switch(init_t) {
                case UPCTR_INIT_RAND:   M[i*N+j] = rand_double();     break;
                case UPCTR_INIT_ZERO:   M[i*N+j] = 0.0;               break;
                case UPCTR_INIT_ONE:    M[i*N+j] = 1.0;               break;
                case UPCTR_INIT_THREAD: M[i*N+j] = MYTHREAD;          break;
                case UPCTR_INIT_INDEX:  M[i*N+j] = i*N+j+1;           break;
                case UPCTR_INIT_IDENT:  M[i*N+j] = (i==j) ? 1.0:0.0;  break;
                case UPCTR_INIT_NONE:                                 break;
                default: assert(!"Unreachable");
            }
        }
    }
}

void upctr_print_mat(char* name, shared double * M) {
    printf("%s:\n", name);

    int i, j;
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            if (j != 0)
                printf(", ");
            printf("%3.0f", M[i*N+j]);
        }
        printf("\n");
    }
    printf("\n");
}
