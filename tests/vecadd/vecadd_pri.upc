#define	BLOCK_SIZE	1000
#define	N			BLOCK_SIZE*THREADS
#include "../upctr_util.h"

shared [BLOCK_SIZE] double a[N];
shared [BLOCK_SIZE] double b[N];
shared [BLOCK_SIZE] double c[N];

int main(){
	int i, base;
	double *a_priv, *b_priv, *c_priv;

	if(MYTHREAD == 0){
		upctr_init_vec_static(a, N, UPCTR_INIT_RAND);
		upctr_init_vec_static(b, N, UPCTR_INIT_RAND);
	}
	upc_barrier;

	upctr_timer("init");
	base = MYTHREAD*BLOCK_SIZE;
	a_priv = (double *) &a[base];
	b_priv = (double *) &b[base];
	c_priv = (double *) &c[base];
	upc_forall(i=0;i<N;i++; &a[i]){
		*c_priv = *a_priv + *b_priv;
		a_priv++; b_priv++; c_priv++;
//		c_priv[i-base] = a_priv[i-base] + b_priv[i-base];
	}

	upc_barrier;

	upctr_timer("Vecadd [Privatization - static alloc]: ");

	if(MYTHREAD == 0){
		for(i=0;i<N;i++)
			assert(c[i] == a[i] + b[i]);
		printf("Results are correct.\n");
	}
	return 0;
}


