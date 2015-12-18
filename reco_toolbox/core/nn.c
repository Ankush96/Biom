/*
Authors: Matthijs Douze & Herve Jegou 
Contact: matthijs.douze@inria.fr  herve.jegou@inria.fr

*/


#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>

#include "machinedeps.h"
#include "vector.h"
#include "nn.h"
#include "binheap.h"
#include "sorting.h"

/*
* Computations are done by blocks (nice for cache access, distance matrix must fit in mem and allows beautiful progress bar)
* blocks are BLOCK_NPT * BLOCK_CLUST
*/


#define BLOCK_N1 1024
#define BLOCK_N2 1024


#define NEWA(type,n) (type*)malloc((n)*sizeof(type))
#define MAX(a,b) ((a)>(b) ? (a) : (b))
#define MIN(a,b) ((a)<(b) ? (a) : (b))




/**********************************************************************************
* Distance functions  
*/


/*------------------ Blas subroutine ------------------*/

#if defined(__OS2__)  || defined(__WINDOWS__) || defined(WIN32) || defined(WIN64) || defined(_MSC_VER)
#define BLASCALL(f) f
#define FINTEGER int
#define INLINE _inline
#else
#define BLASCALL(f) f ##_
#define INLINE inline
#define FINTEGER int
#endif

#define real float
#define integer FINTEGER

void sgemm(char *, char *, const integer *, const integer *, const integer *, const real *, const real *, const integer *,const real *, const integer *, const real *, const real *, const integer *);

/* void sgemm(char *, char *, const int *, const int *, const int *, const float *, const float *, const int *, const float *, const int *, const float *, float *, const int *); */

void sgemv(char *, const integer *, const integer *, const real *, const real *, const integer *, const real *, const integer *, const real *, const real *, const integer *);

/* void sgemv(char *, const int *, const int *, const float *, const float *, const int *, const float *, const int *,  const float *, float *, const int *); */

#undef real
#undef integer


/*
* computes dist2 := dist2 - 2 * descs * clusters' 
* where 
*   dist2    is ndesc-by-nclust 
*   clusters is nclust-by-d
*   descs    is ndesc-by-d
* (all matrices stored by lines, à la C, and packed)
*/


static void add_matmul (FINTEGER d, FINTEGER na, FINTEGER nb,const float *a, FINTEGER lda, const float *b, FINTEGER ldb,float *dist2, FINTEGER ldd)
{
	/* ldd >= na */

	float minus_two = -2;
	float one = 1;

	BLASCALL(sgemm)("Transposed", "Not trans", &na, &nb, &d,&minus_two, a, &lda, b, &ldb, &one, dist2, &ldd);

}

static void add_matvecmul (FINTEGER d, FINTEGER nb,const float *a, const float *b, FINTEGER ldb,float *dist2)
{
	/* ldd >= na */

	float minus_two = -2;
	float one = 1;                               
	FINTEGER ione = 1;

	BLASCALL(sgemv)("Transposed", &d, &nb, &minus_two, b, &ldb, a, &ione, &one, dist2, &ione);

}



/* computes all distances between a line of a and a line of b. 
*   a(na,d) by lines
*   b(nb,d) by lines
*  dist2[i+na*j] = || a(i,:)-b(j,:) ||^2
*/
void compute_cross_distances (int d, int na, int nb,const float *a, const float *b,  float *dist2) 
{
	compute_cross_distances_nonpacked (d, na, nb, a, d, b, d, dist2, na);
}

void compute_cross_distances_nonpacked (int d, int na, int nb, const float *a, int lda, const float *b, int ldb, float *dist2, int ldd)
{
	long i, j;
	float s;
	const float *cl , *dl;
	float *d2l;
	double sum_d2;
	float *sum_c2 = (float *) malloc (sizeof (float) * na);

	for (i = 0; i < na; i++) 
	{
		s = 0;
		cl = a + lda * i;
		for (j = 0; j < d; j++)
		{
			s += cl[j] * cl[j];
		}
		sum_c2[i] = s;
	}

	for (i = 0; i < nb; i++) 
	{
		sum_d2 = 0.0;
		dl = b + ldb * i;
		for (j = 0; j < d; j++)
		{
			sum_d2 += dl[j] * dl[j];
		}
		d2l = dist2 + i * ldd;
		for (j = 0; j < na; j++)
		{
			d2l[j] = sum_d2 + sum_c2[j];
		}
	}

	add_matmul (d, na, nb, a, lda, b, ldb, dist2, ldd);

	free (sum_c2);
}


void compute_distances_1_nonpacked (int d, int nb, const float *a, const float *b, int ldb, float *dist2)
{
	long i, j;
	double sum_d2, sum_c2;
	const float *dl;

	sum_c2=0;
	for (j = 0; j < d; j++)
	{
		sum_c2 += a[j] * a[j];
	}

	for (i = 0; i < nb; i++) 
	{
		sum_d2 = 0.0;
		dl = b + ldb * i;
		for (j = 0; j < d; j++)
		{
			sum_d2 += dl[j] * dl[j];
		}
		dist2[i] = sum_d2 + sum_c2;
	}

	add_matvecmul (d, nb, a, b, ldb, dist2);

}

void compute_distances_1 (int d, int nb,const float *a, const float *b,float *dist2) 
{
	compute_distances_1_nonpacked(d,nb,a,b,d,dist2);  
}



static double sqr (double x)
{
	return x * x;
}

/* alternative distance functions (not optimized) */

void compute_cross_distances_alt_nonpacked (int distance_type, int d, int na, int nb,const float *a, int lda,const float *b, int ldb,float *dist2, int ldd) 
{
	int i,j,k;
	float av,bv,den;
	float *dline;
	const float *bline,*aline;
	double sum;


	for(j=0;j<nb;j++) 
	{
		dline=dist2+j*ldd;
		bline=b+ldb*j;
		aline=a;
		for(i=0;i<na;i++) 
		{
			sum=0;
			if(distance_type==1) 
			{
				for(k=0;k<d;k++) 
				{
					sum+=fabs(aline[k]-bline[k]);
				}
			}
			else if(distance_type==2) 
			{
				for(k=0;k<d;k++) 
				{
					sum+=sqr(aline[k]-bline[k]);
				}
			}
			else if(distance_type==3) 
			{
				for(k=0;k<d;k++) 
				{
					av   = aline[k];
					bv   = bline[k];
					sum += av+bv==0 ? 0 : sqr(av-bv)/(av+bv);
				}  
			}
			else if(distance_type==4)
			{
				for(k=0;k<d;k++) 
				{
					av   = aline[k];
					bv   = bline[k];
					den  = fabs(av+bv);
					sum += den==0 ? 0 : sqr(av-bv)/den;
				}
			}
			dline[i] = sum;
			aline   += lda;
		}
	}

}

void compute_cross_distances_alt (int distance_type, int d, int na, int nb,const float *a,const float *b, float *dist2) 
{
  compute_cross_distances_alt_nonpacked(distance_type, d, na, nb, a, d, b, d, dist2, na);
}




/**********************************************************************************
* Elementary cluster assignment 
*/




/* This function quantizes the vectors coords according to the codebook clusters, 
and sets the corresponding indexes in the index vector vw accordingly    
*/

/* n1 = pts */
static void nn_single_full (int distance_type,int n1, int n2, int d,const float *mat2, const float *mat1,const float *vw_weights,int *vw, float *vwdis,void (*peek_fun) (void *arg, double frac),void *peek_arg)
{
	int step1 = MIN (n1, BLOCK_N1), step2 = MIN (n2, BLOCK_N2) , m1 , m2 , imin;
	long i1,i2,j1,j2;
	float dmin;

	float *dists = fvec_new (step1 * step2) , *dline;

	/* divide the dataset into sub-blocks to:
	* - not make a too big dists2 output array 
	* - call peek_fun from time to time
	*/

	for (i1 = 0; i1 < n1; i1 += step1) 
	{  
		m1 = MIN (step1, n1 - i1);

		/* clear mins */

		for (j1 = 0; j1 < m1; j1++) 
		{
			vw[j1+i1]=-1;
			vwdis[j1+i1]=1e30;
		}

		for (i2 = 0; i2 < n2 ; i2 += step2) 
		{     

			m2 = MIN (step2, n2 - i2);

			if(distance_type==2)   
			{
				compute_cross_distances (d, m2, m1, mat2+i2*d, mat1+i1*d, dists);
			}
			else
			{
				compute_cross_distances_alt (distance_type, d, m2, m1, mat2+i2*d, mat1+i1*d, dists);
			}

			if(vw_weights) 
			{
				for(j1=0;j1<m1;j1++) 
				{
					for (j2 = 0; j2 < m2; j2++)
					{
						dists[j1 * m2 + j2] *= vw_weights[j2 + i2];
					}
				}
			}

			/* update mins */

			for(j1=0;j1<m1;j1++) 
			{
				dline   = dists+j1*m2;
				imin    = vw[i1+j1];
				dmin    = vwdis[i1+j1];

				for(j2=0;j2<m2;j2++) 
				{
					if(dline[j2]<dmin) 
					{
						imin=j2+i2;
						dmin=dline[j2];
					}
				}

				vw[i1+j1]   = imin;
				vwdis[i1+j1] = dmin;
			}      
		}  

		if (peek_fun)
		{
			(*peek_fun) (peek_arg, i1 / (double) n1);
		}

	}

	free (dists);

	if (peek_fun)
	{
		(*peek_fun) (peek_arg, 1);
	}
}




void knn_full (int distance_type,int n1, int n2, int d, int k, const float *mat2, const float *mat1,const float *vw_weights,int *vw, float *vwdis,void (*peek_fun) (void *arg,double frac),void *peek_arg)
{
	int step1, step2 , m1 , m2;
	float *dists,*dline;
	long oneh;
	char *minbuf;
	long i1,i2,j1,j2;

	assert (k <= n2);

	if(k==1) 
	{
		nn_single_full(distance_type,n1, n2, d, mat2, mat1, vw_weights, vw, vwdis, peek_fun, peek_arg);
		return;
	}


	step1 = MIN (n1, BLOCK_N1);
	step2 = MIN (n2, BLOCK_N2);

	dists = fvec_new (step1 * step2);


	/* allocate all heaps at once */
	oneh   = fbinheap_sizeof(k);
	/* oneh=(oneh+7) & ~7; /* round up to 8 bytes */ 
	minbuf = malloc (oneh * step1);

#define MINS(i) ((fbinheap_t*)(minbuf + oneh * i))

	for (i1 = 0; i1 < n1; i1 += step1) 
	{  
		m1 = MIN (step1, n1 - i1);

		/* clear mins */
		for (j1 = 0; j1 < m1; j1++) 
		{
			fbinheap_init(MINS(j1),k);
		}


		for (i2 = 0; i2 < n2 ; i2 += step2) 
		{     

			m2 = MIN (step2, n2 - i2);

			if(distance_type==2)   
			{
				compute_cross_distances (d, m2, m1, mat2+i2*d, mat1+i1*d, dists);
			}
			else
			{
				compute_cross_distances_alt (distance_type, d, m2, m1, mat2+i2*d, mat1+i1*d, dists);   
			}

			if(vw_weights) 
			{
				for(j1=0;j1<m1;j1++) 
				{
					for (j2 = 0; j2 < m2; j2++)
					{
						dists[j1 * m2 + j2] *= vw_weights[j2 + i2]; 
					}
				}
			}

			/* update mins */

			for(j1=0;j1<m1;j1++) 
			{
				dline=dists+j1*m2; 
				fbinheap_addn_label_range(MINS(j1),m2,i2,dline);
			}      

		}  

		for (j1 = 0; j1 < m1; j1++) 
		{
			fbinheap_t *mh = MINS(j1);
			assert (mh->k == k);

			for (j2 = 0; j2 < k; j2++) 
			{
				vw[(i1+j1) * k + j2]    = mh->label[j2+1];
				vwdis[(i1+j1) * k + j2] = mh->val[j2+1];
			}
		}

		if (peek_fun)
		{
			(*peek_fun) (peek_arg, i1 / (double) n1);
		}

	}

#undef MINS
	free (minbuf);
	free(dists);

	if (peek_fun)
	{
		(*peek_fun) (peek_arg, 1);
	}

}


void knn_reorder_shortlist(int n, int nb, int d, int k,const float *b, const float *v,int *assign,float *dists) 
{
	float *disti,*subb=fvec_new(k*d);
	float *diststmp=fvec_new(k);
	int *assigni,*perm=ivec_new(k);
	int *assigntmp=ivec_new(k);  
	int i,j,ki;

	for(i=0;i<n;i++) 
	{
		assigni = assign+i*k;
		disti  = dists+i*k;

		if(1) 
		{

			for(j=0;j<k;j++) 
			{
				if(assigni[j]<0) break;
				memcpy(subb+j*d,b+assigni[j]*d,sizeof(*subb)*d);
			}

			ki=j;

		} 
		else 
		{
			for(j=0;j<k;j++) 
			{
				if(assigni[j]<0) 
				{
					break;
				}
			}
			ki=j;
			ivec_sort(assigni,ki); /* to improve access locality */
			for(j=0;j<ki;j++) 
			{
				memcpy(subb+j*d,b+assigni[j]*d,sizeof(*subb)*d);
			} 
		}


		compute_distances_1(d,ki,v+i*d,subb,diststmp);

		fvec_sort_index(diststmp,ki,perm);

		memcpy(assigntmp,assigni,sizeof(*assigni)*ki);

		for(j=0;j<ki;j++) 
		{
			disti[j]   = diststmp[perm[j]];
			assigni[j] = assigntmp[perm[j]];
		}

	}
	free(assigntmp);
	free(diststmp);
	free(subb);
	free(perm);
}



/**********************************************************************************
* Simple call versions
*/


void nn (int npt, int nclust, int d,const float *codebook, const float *coords, int *vw,void (*peek_fun) (void *arg, double frac),void *peek_arg) 
{

	/* The distances to centroids that will be returned */
	float *vwdis = fvec_new(npt);

	knn_full (2, npt, nclust, d, 1, codebook, coords, NULL, vw, vwdis, peek_fun, peek_arg);

	free(vwdis);
}

float *knn (int npt, int nclust, int d, int k,
			const float *codebook, const float *coords,
			int *vw, void (*peek_fun) (void *arg,
			double frac),
			void *peek_arg)
{
	/* The distances to centroids that will be returned */
	float *vwdis = fvec_new(npt * k);

	knn_full (2, npt, nclust, d, k, codebook, coords, NULL, vw, vwdis, peek_fun, peek_arg);

	return vwdis;
}



/**********************************************************************************
* Threaded versions
*/

#if defined(__OS2__)  || defined(__WINDOWS__) || defined(WIN32) || defined(WIN64) || defined(_MSC_VER)
#include "pthread_win.h"
#else
#include <pthread.h>
#endif

/* a common function dispatches the calls */
typedef struct 
{

	/* input */

	int distance_type;  

	int nclust, d, k;
	const float *codebook;

	int npt;
	const float *points;
	const float *vw_weights;

	/* output */
	int *vw;
	float *vwdis;

	/* bookkeeping */
	int n_thread;
	void (*peek_fun) (void *arg, double frac);
	void *peek_arg;
	pthread_mutex_t peek_mutex;
} 
nn_input_t;

static void nn_peek (void *arg, double frac)
{
	nn_input_t *t = arg;
	if (pthread_mutex_trylock (&t->peek_mutex) != 0)      /* don't insist if another thread is peeking */
		return;
	(*t->peek_fun) (t->peek_arg, frac);
	pthread_mutex_unlock (&t->peek_mutex);
}


static void nn_task (void *arg, int tid, int i)
{
	nn_input_t *t = arg;

	long n0 = t->npt * (long)i / t->n_thread;
	long n1 = t->npt * (long)(i + 1) / t->n_thread;

	void (*peek_fun) (void *arg, double frac) = t->peek_fun ? &nn_peek : NULL;

	knn_full (t->distance_type, n1 - n0, t->nclust, t->d, t->k, t->codebook,t->points + n0 * t->d, t->vw_weights,t->vw + n0 * t->k, t->vwdis + n0 * t->k,peek_fun, t);
}

/********** frontends */

void knn_full_thread (int distance_type, int npt, int nclust, int d, int k,const float *codebook, const float *coords,const float *vw_weights,int *vw, float *vwdis2,int n_thread,void (*peek_fun) (void *arg,double frac),void *peek_arg) 
{

	nn_input_t task = 
	{ 
		distance_type,
		nclust, d, k, codebook, 
		npt, coords, vw_weights, vw, vwdis2,
		n_thread, peek_fun, peek_arg, PTHREAD_MUTEX_INITIALIZER
	};

	if (npt < n_thread || n_thread == 1) 
	{        /* too few pts */
		return knn_full (distance_type, npt, nclust, d, k, codebook, coords, vw_weights,vw, vwdis2, peek_fun, peek_arg);
	}


	compute_tasks (n_thread, n_thread, &nn_task, &task);

}


/***************** simplified calls */

float *knn_thread (int npt, int nclust, int d, int k,const float *codebook, const float *coords,int *vw, int n_thread,void (*peek_fun) (void *arg,double frac),void *peek_arg) 
{
	float *vwdis2=fvec_new(k*npt);

	knn_full_thread (2, npt, nclust, d, k, codebook, coords, NULL, vw, vwdis2, n_thread, peek_fun, peek_arg);

	return vwdis2;
}



void nn_thread (int npt, int nclust, int d,const float *codebook, const float *coords,int *vw, int n_thread,void (*peek_fun) (void *arg, double frac),void *peek_arg)
{
	float *vwdis2=fvec_new(npt);

	knn_full_thread (2, npt, nclust, d, 1, codebook, coords, NULL, vw, vwdis2, n_thread, peek_fun, peek_arg);

	free(vwdis2);
}

/***************** cross distances */

typedef struct 
{
	int distance_type; /* <0 mreans optimized L2 distance */
	int d, na, nb;
	const float *a,*b;
	float *dist2;
	int nt;
	int split_a;  
} 
cross_distances_params_t;

/*Core funcs*/

void compute_cross_distances_task(void *arg, int tid, int i) 
{

	long i0 , i1;

	cross_distances_params_t *t=arg;
	if(t->split_a) 
	{
		i0 = t->na*(long)i/t->nt;
		i1 = t->na*(long)(i+1)/t->nt;
		if(t->distance_type<0) 
		{
			compute_cross_distances_nonpacked(t->d,i1-i0,t->nb,t->a+i0*t->d,t->d,t->b,t->d,t->dist2+i0,t->na);
		}
		else 
		{
			compute_cross_distances_alt_nonpacked(t->distance_type,t->d,i1-i0,t->nb,t->a+i0*t->d,t->d,t->b,t->d,t->dist2+i0,t->na);
		}
	} 
	else 
	{
		i0 = t->nb*(long)i/t->nt;
		i1 = t->nb*(long)(i+1)/t->nt;
		if(t->distance_type<0) 
		{
			compute_cross_distances_nonpacked(t->d,t->na,i1-i0,t->a,t->d,t->b+i0*t->d,t->d,t->dist2+i0*t->na,t->na); 
		}
		else
		{
			compute_cross_distances_alt_nonpacked(t->distance_type,t->d,t->na,i1-i0,t->a,t->d,t->b+i0*t->d,t->d,t->dist2+i0*t->na,t->na);     
		}
	}  

}

/*main funcs*/

void compute_cross_distances_thread (int d, int na, int nb,const float *a,const float *b, float *dist2,int nt) 
{
	int n=MAX(na,nb);

	cross_distances_params_t t={-1,d,na,nb,a,b,dist2,nt};


	if(n<nt) /* too small, no threads */
	{
		compute_cross_distances(d,na,nb,a,b,dist2);
	}
	else 
	{ 
		t.split_a=na>nb;    
		compute_tasks(nt,nt,&compute_cross_distances_task,&t);
	} 
}


void compute_cross_distances_alt_thread (int distance_type,int d, int na, int nb,const float *a,const float *b, float *dist2,int nt) 
{

	int n=MAX(na,nb);

	cross_distances_params_t t={distance_type,d,na,nb,a,b,dist2,nt};


	if(n<nt) /* too small, no threads */
	{
		compute_cross_distances_alt(distance_type,d,na,nb,a,b,dist2);
	}
	else 
	{ 
		t.split_a=na>nb;    
		compute_tasks(nt,nt,&compute_cross_distances_task,&t);
	} 
}
