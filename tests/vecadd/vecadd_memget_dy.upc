#define	BLOCK_SIZE	1000
#define	N			BLOCK_SIZE*THREADS
#include "../upctr_util.h"

shared [BLOCK_SIZE] double a[N];
shared [BLOCK_SIZE] double b[N];
shared [BLOCK_SIZE] double c[N];

int main(){
	int i, base;
	double a_local[BLOCK_SIZE];
	double b_local[BLOCK_SIZE];
	double c_local[BLOCK_SIZE];

	if(MYTHREAD == 0){
		upctr_init_vec_static(a, N, UPCTR_INIT_RAND);
		upctr_init_vec_static(b, N, UPCTR_INIT_RAND);
	}
	upc_barrier;

	upctr_timer("init");
	base = MYTHREAD*BLOCK_SIZE;
	upc_memget(a_local, a+base, BLOCK_SIZE*sizeof(double));
	upc_memget(b_local, b+base, BLOCK_SIZE*sizeof(double));
	upc_forall(i=0;i<N;i++; &a[i])
		c_local[i-base] = a_local[i-base] + b_local[i-base];
	upc_memput(c+base, c_local, BLOCK_SIZE*sizeof(double));
	upc_barrier;

	upctr_timer("Vecadd [Memget/put - static alloc]: ");

	if(MYTHREAD == 0){
		for(i=0;i<N;i++)
			assert(c[i] == a[i] + b[i]);
		printf("Results are correct.\n");
	}
	return 0;
}


