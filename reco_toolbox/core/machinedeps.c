/*
Authors: Matthijs Douze & Herve Jegou 
Contact: matthijs.douze@inria.fr  herve.jegou@inria.fr
*/

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>


#if defined(__OS2__)  || defined(__WINDOWS__) || defined(WIN32) || defined(WIN64) || defined(_MSC_VER)
#include "timeval.h"
#ifndef M_LN2
#define M_LN2          0.69314718055994530942  /* log_e 2 */
#endif
#else
#include <time.h>
#include <sys/types.h>
#endif


#include "machinedeps.h"




#if defined(__linux__) || defined(__linux64__)

/* #define __USE_GNU */
#include "/usr/include/sched.h"

int count_cpu (void)
{
  cpu_set_t set;
  sched_getaffinity (0, sizeof (cpu_set_t), &set);
  int i, count = 0;
  for (i = 0; i < CPU_SETSIZE; i++)
  {
    if (CPU_ISSET (i, &set))
	{
      count++;
	}
  }
  return count;
}


#elif defined(__APPLE__)

#include <sys/types.h>
#include <sys/sysctl.h>


int count_cpu (void) 
{
  int count=-1;
  size_t count_size=sizeof(count);
  sysctlbyname("hw.ncpu",&count,&count_size,NULL,0);
  return count;
}

#else

int count_cpu()
{
  return 1;
}


#endif

#ifndef __APPLE__

double log2(double x) 
{
  return log(x)/M_LN2;
}


#endif

#ifndef __linux__
void *memalign (size_t ignored, size_t nbytes)
{
  return malloc (nbytes);
}
#endif

#if defined(__APPLE__) && defined(_LP64)

#warning "warn: using bugfix sgemm for Mac 64 bit"

#define real float
#define integer int



int sgemm_bugfix (char *transa, char *transb, integer * pm, integer *pn, integer * pk, real * palpha, const real * a, integer * plda,
            const real * b, integer * pldb, real * pbeta, real * c,
            integer * pldc) 
{

	int na=*pm,nb=*pn,d=*pk;
	int lda=*plda,ldb=*pldb,ldc=*pldc;
	float alpha=*palpha,beta=*pbeta;
	double accu;

	int i,j,k;

	assert(transa[0]=='T' && transb[0]=='N');


	for(i=0;i<na;i++) 
	{
		for(j=0;j<nb;j++) 
		{
			accu=0;
			for(k=0;k<d;k++)
			{
				accu+=a[k+lda*i]*b[k+ldb*j];
			}
		}
		c[i+j*ldc]=beta*c[i+j*ldc]+alpha*accu;
	}  

	return 0;
}


#endif


#ifdef __linux__

typedef struct 
{
  void *ptr;
  size_t size;
} alloc_block_t;


static struct 
{
  int enabled;
  malloc_stats_t s;
  
  alloc_block_t *blocks;
  
  int n,na;  

  /* stored ref functions */

  void (*real_free) (void *__ptr, const __malloc_ptr_t);
  void *(*real_malloc) (size_t __size, const __malloc_ptr_t);
  void *(*real_realloc) (void *__ptr, size_t __size, const __malloc_ptr_t);
  void *(*real_memalign) (size_t __alignment, size_t __size, const __malloc_ptr_t);
  
} 
msc={0};


static void *collector_memalign (size_t alignment, size_t size, const __malloc_ptr_t f);
static void *collector_malloc (size_t size, const __malloc_ptr_t f);
static void collector_free (void *ptr, const __malloc_ptr_t f);
static void *collector_realloc (void *ptr_in, size_t size, const __malloc_ptr_t f);



#define SET_MALLOC_HOOK(fname) __##fname##_hook=collector_##fname
#define UNSET_MALLOC_HOOK(fname) __##fname##_hook=msc.real_##fname

#define SET_MALLOC_HOOKS \
  SET_MALLOC_HOOK(free);                        \
  SET_MALLOC_HOOK(malloc);                      \
  SET_MALLOC_HOOK(realloc);                     \
  SET_MALLOC_HOOK(memalign);


#define UNSET_MALLOC_HOOKS                      \
  UNSET_MALLOC_HOOK(free);                      \
  UNSET_MALLOC_HOOK(malloc);                    \
  UNSET_MALLOC_HOOK(realloc);                   \
  UNSET_MALLOC_HOOK(memalign);


static void *collector_memalign (size_t alignment, size_t size, const __malloc_ptr_t f) 
{
	UNSET_MALLOC_HOOKS;
	void *ptr; 

	if(alignment==1) 
	{
		ptr=malloc(size);
	} 
	else 
	{
		ptr=memalign(alignment,size);
	}  
	msc.s.n_alloc++;
	msc.s.delta_alloc+=size;
	if(msc.s.delta_alloc>msc.s.max_alloc) 
	{
		msc.s.max_alloc=msc.s.delta_alloc;
	}

	if(msc.n>=msc.na) 
	{
		msc.na=msc.na<8 ? 8 : msc.na*2;
		msc.blocks=realloc(msc.blocks,sizeof(*msc.blocks)*msc.na);
		assert(msc.blocks);
	}

	msc.blocks[msc.n].ptr=ptr;
	msc.blocks[msc.n].size=size;
	msc.n++;  

	SET_MALLOC_HOOKS;
	return ptr;
}

static void *collector_malloc (size_t size, const __malloc_ptr_t f) 
{
  return collector_memalign(1,size,f);
}


static void collector_free (void *ptr, const __malloc_ptr_t f) 
{

	int i;

	UNSET_MALLOC_HOOKS;

	free(ptr);

	msc.s.n_free++;

	/* find where the block is */
	for(i=msc.n-1;i>=0;i--) 
		if(msc.blocks[i].ptr==ptr) break;

	if(i<0) {
		msc.s.n_untracked_frees++;
	} else {
		msc.s.delta_alloc-=msc.blocks[i].size;
		msc.blocks[i]=msc.blocks[msc.n-1];
		msc.n--;
	}

	SET_MALLOC_HOOKS;

}



static void *collector_realloc (void *ptr_in, size_t size, const __malloc_ptr_t f) 
{

	UNSET_MALLOC_HOOKS

		int i;

	void *ptr=realloc(ptr_in,size);

	/* find where the block is */
	for(i=msc.n-1;i>=0;i--) 
	{
		if(msc.blocks[i].ptr==ptr_in) 
		{
			break;
		}
	}

	msc.s.n_realloc++;

	if(i<0) 
	{
		msc.s.n_untracked_frees++;
	} 
	else 
	{
		msc.s.delta_alloc-=msc.blocks[i].size+size;

		if(msc.s.delta_alloc>msc.s.max_alloc)
		{
			msc.s.max_alloc=msc.s.delta_alloc;
		}

		msc.blocks[i].ptr=ptr;
	}

	SET_MALLOC_HOOKS

		return ptr;
}


#define GET_MALLOC_HOOK(fname) msc.real_##fname=__##fname##_hook; SET_MALLOC_HOOK(fname)



void malloc_stats_begin() 
{
	assert(!msc.enabled || "malloc_stats_begin: collector enabled already");
	msc.enabled=1;

	memset(&msc.s,0,sizeof(malloc_stats_t));
	msc.n=msc.na=0;
	msc.blocks=NULL;

	printf("initial hooks: %p %p %p %p\n",__free_hook,__malloc_hook,__realloc_hook,__memalign_hook);

	GET_MALLOC_HOOK(free);
	GET_MALLOC_HOOK(malloc);
	GET_MALLOC_HOOK(realloc);
	GET_MALLOC_HOOK(memalign);

}


malloc_stats_t malloc_stats_end() 
{
  assert(msc.enabled || "malloc_stats_begin: collector not enabled");
  msc.enabled=0;
  
  UNSET_MALLOC_HOOKS;
  
  free(msc.blocks);

  return msc.s;
}

#else

void malloc_stats_begin() 
{
  /* not implemented */
}

malloc_stats_t malloc_stats_end() 
{
  malloc_stats_t s;
  memset(&s,0,sizeof(malloc_stats_t));
  return s;
}


#endif 


double getmillisecs() 
{
  struct timeval tv;
  gettimeofday (&tv,NULL);
  return tv.tv_sec*1e3 +tv.tv_usec*1e-3;
}


/***********************************************************************
 *           Implementation of the threading part
 *
 * generic thread stuff */

#if defined(__OS2__)  || defined(__WINDOWS__) || defined(WIN32) || defined(WIN64) || defined(_MSC_VER)
#include "pthread_win.h"
#else
#include <pthread.h>
#endif


typedef struct 
{
  pthread_mutex_t mutex;
  int i, n, tid;
  void (*task_fun) (void *arg, int tid, int i);
  void *task_arg;
} context_t;



static void *start_routine (void *cp)
{
  context_t *context = cp;
  int tid , item;
  pthread_mutex_lock (&context->mutex);
  tid = context->tid++;
  pthread_mutex_unlock (&context->mutex);

  for (;;) 
  {
	  pthread_mutex_lock (&context->mutex);
	  item = context->i++;
	  pthread_mutex_unlock (&context->mutex);
	  if (item >= context->n)
	  {
		  break;
	  }
	  else
	  {
		  context->task_fun (context->task_arg, tid, item);
	  }
  }

  return NULL;
}


void compute_tasks (int n, int nthread, void (*task_fun) (void *arg, int tid, int i), void *task_arg)
{
  int i;
  context_t context;

  assert(nthread>0 || !"sombody has to do the job");

  context.i = 0;
  context.n = n;
  context.tid = 0;
  context.task_fun = task_fun;
  context.task_arg = task_arg;

  pthread_mutex_init(&context.mutex, NULL);

  if(nthread==1) 
  {
    start_routine(&context);   
  }
  else 
  {
    pthread_t *threads = malloc (sizeof (pthread_t) * n);
      
    for (i = 0; i < nthread; i++) 
	{
      pthread_create(&threads[i], NULL, &start_routine, &context);
	}
      
    /* all computing */
      
    for (i = 0; i < nthread; i++)
	{
      pthread_join (threads[i], NULL);
	}
    
    free (threads);
  }
}
