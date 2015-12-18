/*
Authors: Matthijs Douze & Herve Jegou 
Contact: matthijs.douze@inria.fr  herve.jegou@inria.fr
*/

#ifndef KMEANS_H_INCLUDED
#define KMEANS_H_INCLUDED


/*---------------------------------------------------------------------------*/
/*! @addtogroup clustering
 *  @{  */


/* flags for kmeans */
#define KMEANS_QUIET           0x10000
#define KMEANS_INIT_RANDOM     0x20000
#define KMEANS_NORMALIZE_CENTS 0x40000
#define KMEANS_NORMALIZE_SOPHISTICATED 0x80000


/*! Compute the k-means centroids. 
 *
 * @param v(d, n)           vectors to cluster
 * @param centroids(d, k)   output centroids
 * @param flags             a set of computation parameters: 
 *                     - flags & 0xffff : use this many threads to compute 
 *                     - flags & KMEANS_QUIET: suppress kmeans output
 *                     - flags & KMEANS_INIT_RANDOM: random initialization 
 *                     - flags & KMEANS_NORMALIZE_CENTS: normalize centroids to L2=1 after they are computed
 *                     - flags & KMEANS_NORMALIZE_SOPHISTICATED: ditto, more sophisticated
 * @param seed              random seed for intialization
 * @param redo              perform clustering this many times and keep clusters with smallest quantization error
 * @param dis(n)            squared distance to assigned centroid of each input vector (may be NULL)
 * @param assign(n)         index of assigned centroid in 0..k-1 (may be NULL) 
 * @param nassign(k)        nb of vectors assigned to each centroid (may be NULL)
 *
 * @return final quantization error 
 */

void randini(void);


float kmeans (int d, int n, int k, int niter, const float * v, int flags, int seed, int redo, int verbose , float * centroids, float * dis, int * assign, int * nassign);


/*----------- Following functions are used for forward compatibility -----------*/

/*! simplified call */
float* clustering_kmeans (int n, int d,
                          const float *points, int k, int nb_iter_max, 
                          double normalize);

/*! Same as kmeans, but generate in addition the assignment
 *  performed on the input set
 */
float* clustering_kmeans_assign (int n, int d, const float *points, int k, int nb_iter_max, double normalize, int ** clust_assign_out);



float* clustering_kmeans_assign_with_score (int n, int d,const float *points, int k, int nb_iter_max,double normalize, int n_thread, double *score_out,int ** clust_assign_out);


/*! @} */
#endif
