/*
Authors: Matthijs Douze & Herve Jegou 
Contact: matthijs.douze@inria.fr  herve.jegou@inria.fr

*/

#ifndef MACHINEDEPS_H_INCLUDED
#define MACHINEDEPS_H_INCLUDED

#ifdef __APPLE__
#define HAVE_QSORT_R
#endif

#if defined(__linux__)  || defined(__OS2__)  || defined(__WINDOWS__) || defined(WIN32) || defined(_MSC_VER)
/* #ifdef __linux__ */
#define HAVE_TLS
#else
#define __thread 
#endif

/*---------------------------------------------------------------------------*/
/*! @addtogroup machinedep
 *  @{
 */


/*! Return the number of cores. */
int count_cpu(void);

#ifndef __APPLE__

double log2(double x);

#endif

#ifdef __linux__
#include <malloc.h>
#else
#include <stdlib.h>

/*! allocate memory such that the pointer is aligned*/
void *memalign (size_t ignored, size_t nbytes);

#endif

#if defined(__APPLE__) && defined(_LP64)

#define sgemm_ sgemm_bugfix


#endif


/*! trace all mallocs between two function calls. Intended to replace
 * struct mallinfo that does not seem to work. Implemented only for
 * Linux. Includes inefficient code that should not be relied on while
 * profiling. */

typedef struct {
  int n_alloc,n_free,n_realloc; /* nb of operations of each type */
  size_t delta_alloc;           /* total allocated minus deallocated 
                                   (can only trace deallocs that were allocated during the tracing) */ 
  size_t max_alloc;             /* max of delta_alloc during the tracing */
  int n_untracked_frees;        /* nb of frees of objects that were not allocated during the tracing 
                                   (cannot be taken into accout in delta_alloc) */
} malloc_stats_t;


void malloc_stats_begin (void);

malloc_stats_t malloc_stats_end (void);



/*! return a timestamp, which is useful to measure elapsed time */
double getmillisecs();

/*! exectutes a set of tasks in parallel using a thread pool 
 *
 * @param n            number of tasks to execute
 * @param nthread      number of threads that will run the tasks
 * @param task_fun     this callback will be called with 
 *              -  arg = task_arg
 *              -  tid = identifier of the thread in 0..nthread-1
 *              -  i = call number in 0..n-1
*/
void compute_tasks (int n, int nthread,
                    void (*task_fun) (void *arg, int tid, int i),
                    void *task_arg);


/*! @} */
#endif
