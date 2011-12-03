#include "../upctr_util.h"
#define	N	BLOCK_SIZE*THREADS

shared [BLOCK_SIZE] double a[N];
shared [BLOCK_SIZE] double b[N];
shared [BLOCK_SIZE] double c[N];

int main(){
	int i;
	if(MYTHREAD == 0){
		upctr_init_vec_static(a, N, UPCTR_INIT_RAND);
		upctr_init_vec_static(b, N, UPCTR_INIT_RAND);
	}
	upc_barrier;

	upctr_timer("init");
	upc_forall(i=0;i<N;i++; &a[i])
		c[i] = a[i] + b[i];
	upc_barrier;

	upctr_timer("Vecadd [Naive - static alloc]: ");

	if(MYTHREAD == 0){
		for(i=0;i<N;i++)
			assert(c[i] == a[i] + b[i]);
		printf("Results are correct.\n");
	}
	return 0;
}


