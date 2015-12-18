/*
Authors: Matthijs Douze & Herve Jegou 
Contact: matthijs.douze@inria.fr  herve.jegou@inria.fr

*/

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "vector.h"
#include "kmeans.h"
#include "nn.h"
#include "machinedeps.h"


#if defined(__OS2__)  || defined(__WINDOWS__) || defined(WIN32) || defined(WIN64) || defined(_MSC_VER)
#undef rand()
#define SHR3   ( jsr ^= (jsr<<17), jsr ^= (jsr>>13), jsr ^= (jsr<<5) )
#define randint SHR3
#define rand() (0.5 + (signed)randint*2.328306e-10)
#ifdef __x86_64__
    typedef int UL;
#else
    typedef unsigned long UL;
#endif

static UL jsrseed = 31340134 , jsr;
#else
#endif



/********************************************************************************************************************************************/


static void nn_full (int d, int n, int nb, const float * v, const float *b, int nt, int * assign, float * dis)
{
  knn_full_thread (2, n, nb, d, 1, b, v, NULL, assign, dis, nt, NULL, NULL);
}

/********************************************************************************************************************************************/


static void random_init(long d, int n, int k, const float * v, int * sel) 
{
  int *perm = ivec_new_random_perm(n);
  ivec_cpy(sel,perm,k);
  free(perm);
}

/********************************************************************************************************************************************/


/* the kmeans++ initialization (see wikipedia) */
static void kmeanspp_init (long d, int n, int k, int verbose , const float * v, int * sel)
{
  

  /* select the first centroid and set the others unitialized*/

  int newsel;
  long i, j;
  double rd;
/*
  for(i=0;i<k;i++) sel[i]=i;

  return;


*/
  float * disbest = fvec_new_set (n, HUGE_VAL);
  float * distmp = fvec_new (n);


#if defined(__OS2__)  || defined(__WINDOWS__) || defined(WIN32) || defined(WIN64) || defined(_MSC_VER)
  randini();  
  sel[0] = (int)floor(rand()* k);
#else
  sel[0] = lrand48() % k;
#endif

  for (i = 1 ; i < k ; i++) 
  {
    newsel = sel[i - 1];

	if(verbose)
	{

#ifdef MATLAB_MEX_FILE

		if(i%10==0) 
		{
			mexPrintf("%d/%d\r",(int)i,k);
		}

#else
		if(i%10==0) 
		{
			printf("%d/%d\r",(int)i,k); fflush(stdout);
		}
#endif
	}


	if(0) 
	{ /* simple and slow */

		for (j = 0 ; j < n ; j++) 
		{
			distmp[j] = (float)fvec_distance_L2sqr (v + d * j, v + d * newsel, d);
			if (distmp[j] < disbest[j])
			{
				disbest[j] = distmp[j];
			}
		}
	} 
	else 
	{ /* complicated and fast */

/*
      compute_cross_distances(d,1,n,
                              v + d * newsel,
                              v,distmp);
*/
      compute_distances_1(d , n , v + d * newsel , v ,distmp);
      
	  for(j=0;j<n;j++) 
	  {
		  if(distmp[j]<disbest[j]) 
		  {
			  disbest[j]=distmp[j];
		  }
	  }
    }

    
    /* convert the best distances to probabilities */
    
	memcpy (distmp, disbest, n * sizeof (*distmp));

    fvec_normalize (distmp, n, 1);

/*
    {
      int *perm=ivec_new(n);
      fvec_sort_index(distmp,n,perm);
      double accu=0;
      for(j=n*9/10;j<n;j++) 
        accu+=distmp[perm[j]];
      printf("it %d/%d p 10%%=%.3f\n",i,k,accu);      
      
      free(perm);      
    }
*/    
 

#if defined(__OS2__)  || defined(__WINDOWS__) || defined(WIN32) || defined(WIN64) || defined(_MSC_VER)
	rd = rand();
#else
	rd = drand48();
#endif
    
	for (j = 0 ; j < n - 1 ; j++) 
	{
		rd -= distmp[j];
		if (rd < 0)
		{
			break;
		}
	}
    sel[i] = j;
  }
  if(verbose)
  {
#ifdef MATLAB_MEX_FILE
	  mexPrintf("\n");
#else
	  printf("\n");
#endif
  }
  
  free (disbest);
  free (distmp);
}


float kmeans (int di, int n, int k, int niter, const float * v, int flags, int seed, int redo, int verbose , float * centroids_out, float * dis_out, int * assign_out, int * nassign_out)
{
	double qerr, qerr_old, qerr_best = HUGE_VAL;
	long i, run, iter, iter_tot = 0, d=di;
	int nt=flags & 0xffff;
	int nsubset , isout_centroids , isout_dis , isout_assign , isout_nassign;
	/*	int verbose = !(flags & KMEANS_QUIET); */

	float *centroids , *dis , *norms;
	int   *assign, *nassign , *selected;

	niter            = (niter == 0 ? 10000 : niter);

	/* look at which variables have to be returned / allocated */
	isout_centroids  = (centroids_out == NULL ? 0 : 1);
	isout_dis        = (dis_out == NULL ? 0 : 1);
	isout_assign     = (assign_out == NULL ? 0 : 1);
	isout_nassign    = (nassign_out == NULL ? 0 : 1);

	/* the centroids */
	centroids        = fvec_new (k * (size_t) d);

	/* store the distances from points to their nearest centroids */
	dis              = fvec_new (n);

	/* the centroid indexes to which each vector is assigned */
	assign           = ivec_new (n);

	/* the number of points assigned to a cluster */
	nassign          = ivec_new (k);

	/* the total quantization error */

	/* for the initial configuration */
	selected         = ivec_new (k);

	for (run = 0 ; run < redo ; run++) 
	{
do_redo: 

		if(verbose)
		{
#ifdef MATLAB_MEX_FILE

			mexPrintf("<><><><> kmeans / run %d <><><><><>\n", (int)run);

#else
			fprintf (stderr, "<><><><> kmeans / run %d <><><><><>\n", (int)run);

#endif
		}


		if( flags& KMEANS_INIT_RANDOM ) 
		{
			random_init(d,n,k,v,selected);
		} 
		else 
		{

			nsubset=n;

			if(n>k*8 && n>8192) 
			{
				nsubset=k*8;
				if(verbose)
				{
#ifdef MATLAB_MEX_FILE
					mexPrintf("Restricting k-means++ initialization to %d points\n",nsubset);
#else
					printf("Restricting k-means++ initialization to %d points\n",nsubset);
#endif
				}
			}
			kmeanspp_init (d, nsubset, k, verbose , v, selected);
		}


		for (i = 0 ; i < k ; i++) 
		{
			fvec_cpy (centroids + i * d, v + selected[i] * d, d);
		}

		/* the quantization error */
		qerr = HUGE_VAL;

		for (iter = 1 ; iter <= niter ; iter++) 
		{
			ivec_0 (nassign, k);
			iter_tot++;

			/* Assign point to cluster and count the cluster size */
			nn_full (d, n, k, v, centroids, nt, assign, dis);

			for (i = 0 ; i < n ; i++) 
				nassign[assign[i]]++;

			for (i = 0 ; i < k ; i++) 
			{
				if(nassign[i]==0) 
				{
					if(verbose)
					{
#ifdef MATLAB_MEX_FILE
						mexPrintf("WARN nassign %d is 0, redoing!\n",(int)i);

#else
						fprintf(stderr,"WARN nassign %d is 0, redoing!\n",(int)i);
#endif
					}
					goto do_redo;
				}
			}


			/* update the centroids */

			if(flags & KMEANS_NORMALIZE_SOPHISTICATED)
			{

				norms = fvec_new(k);

				fvec_0(centroids, d * k);

				for (i = 0 ; i < n ; i++) 
				{
					fvec_add (centroids + assign[i] * d, v + i * d, d);
					norms[assign[i]]+=(float)fvec_norm(v + i * d,d,2);
				}

				for (i = 0 ; i < k ; i++) 
				{          
					fvec_normalize(centroids + i * d, d,2);
					fvec_mul_by (centroids + i * d, d, norms[i] / nassign[i]);
				}

				free(norms);

			} 
			else 
			{
				fvec_0 (centroids, d * k);

				for (i = 0 ; i < n ; i++)
				{
					fvec_add (centroids + assign[i] * d, v + i * d, d);
				}

				/* normalize */

				for (i = 0 ; i < k ; i++) 
				{          
					fvec_mul_by(centroids + i * d, d, 1.0 / nassign[i]);
				}

				if(flags & KMEANS_NORMALIZE_CENTS) 
				{
					for (i = 0 ; i < k ; i++) 
					{
						fvec_normalize(centroids + i * d, d, 2.0);
					}
				}
			}

			assert(qerr>=0);

			/* compute the quantization error */
			qerr_old  = qerr;
			qerr      = fvec_sum(dis, n);

			if (qerr_old == qerr)
			{
				break;
			}

			if(verbose)
			{
#ifdef MATLAB_MEX_FILE
				mexPrintf (" -> %.3f", qerr / n);

#else
				fprintf (stderr, " -> %.3f", qerr / n);

#endif
			}
		}
		if(verbose)    
		{
#ifdef MATLAB_MEX_FILE

			mexPrintf (stderr, "\n");
#else
			fprintf (stderr, "\n");

#endif
		}

		/* If this run is the best encountered, save the results */
		if (qerr < qerr_best) 
		{
			qerr_best = qerr;

			if (isout_centroids)
			{
				memcpy (centroids_out, centroids, k * d * sizeof (*centroids));
			}
			if (isout_dis)
			{
				memcpy (dis_out, dis, n * sizeof (*dis));
			}
			if (isout_assign)
			{
				memcpy (assign_out, assign, n * sizeof (*assign));
			}
			if (isout_nassign)
			{
				memcpy (nassign_out, nassign, k * sizeof (*nassign));
			}
		}
	}

	if(verbose)
	{
#ifdef MATLAB_MEX_FILE
		mexPrintf ("Total number of iterations: %d\n", (int)iter_tot);
#else
		fprintf (stderr, "Total number of iterations: %d\n", (int)iter_tot);

#endif
	}

	/* printf("unbalanced factor of last iteration: %g\n",ivec_unbalanced_factor(nassign,k)); */

	/* free the variables that are not returned */
	free (selected);
	free (centroids);
	free (dis);
	free (assign);
	free (nassign);



	return (float)(qerr_best / n); 
}

/********************************************************************************************************************************************/

#if defined(__OS2__)  || defined(__WINDOWS__) || defined(WIN32) || defined(WIN64) || defined(_MSC_VER)

void randini(void)
{
	jsrseed  = (UL) time( NULL );
	jsr     ^= jsrseed;
}

#endif
