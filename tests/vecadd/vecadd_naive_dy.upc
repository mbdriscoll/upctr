#define	BLOCK_SIZE	1000
#define	N			BLOCK_SIZE*THREADS
#include "../upctr_util.h"

int main(){
	int i;
	shared double *a = upctr_init_vec(N, UPCTR_INIT_RAND);
	shared double *b = upctr_init_vec(N, UPCTR_INIT_RAND);
	shared double *c = upctr_init_vec(N, UPCTR_INIT_ZERO);

	upctr_timer("init");
	upc_forall(i=0;i<N;i++; &a[i])
		c[i] = a[i] + b[i];
	upc_barrier;

	upctr_timer("Vecadd [Naive - dynamic alloc]: ");

	if(MYTHREAD == 0){
		for(i=0;i<N;i++)
			assert(c[i] == a[i] + b[i]);
		printf("Results are correct.\n");
	}
	return 0;
}


