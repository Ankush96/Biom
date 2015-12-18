/*
Authors: Matthijs Douze & Herve Jegou 
Contact: matthijs.douze@inria.fr  herve.jegou@inria.fr

*/

/*---------------------------------------------------------------------------*/

#ifndef NN_H_INCLUDED
#define NN_H_INCLUDED

/*---------------------------------------------------------------------------*/
/*! @addtogroup knearestneighbors
 *  @{
 */


/*! @defgroup knearestneighbors
 * Nearest-neighbor (NN) functions   
 *
 * All matrices are stored in column-major order (like Fortran) and
 * indexed from 0 (like C, unlike Fortran). The declaration:
 *
 *     a(m, n) 
 * 
 * means that element a(i, j) is accessed with a[ i * m + j ] where
 *
 *     0 <= i < m and 0 <= j < n
 *
 */


/*!  Finds nearest neighbors of vectors in a base 
 * 
 * @param    distance_type  2 = L2 distance (see compute_cross_distances_alt for distance_type's)
 * @param    n              number of vectors to assign 
 * @param    nb             number of base vectors to assign to
 * @param    k              number of neighbors to return
 * @param    v(d, n)        query vectors
 * @param    b(d, nb)       base vectors
 * @param    assign(k, n)   on output, NNs of vector i are assign(:, i) (not sorted!)
 * @param    b_weights(nb)  multiply squared distances by this for each base vector (may be NULL)
 * @param    dis(k, n)      squared distances of i to its NNs are dis(0, i) to dis(k-1, i)
 * @param    peek_fun, peek_arg  the function calls peek_fun with frac set
 *                          to the fraction of the computation performed so far (for
 *                          progress bars), peek_fun needs not to be reentrant. 
 * 
 */

void knn_full (int distance_type,
               int n, int nb, int d, int k,
               const float *b, const float *v,
               const float *b_weights,
               int *assign, float *dis,                                             
               void (*peek_fun) (void *arg,double frac),
               void *peek_arg);

/*! multi-threaded version 
 */
void knn_full_thread (int distance_type,
                      int n, int nb, int d, int k,
                      const float *b, const float *v,
                      const float *b_weights,
                      int *assign, float *dis,
                      int n_thread,
                      void (*peek_fun) (void *arg,double frac),
                      void *peek_arg);




/* next functions are simplified calls of the previous */


/*! single NN */
void nn (int n, int nb, int d, 
         const float *b, const float *v,
         int *assign,                                              
         void (*peek_fun) (void *arg,double frac),
         void *peek_arg);

/*! single NN, multithread */
void nn_thread (int n, int nb, int d, 
                const float *b, const float *v,
                int *assign,    
                int n_thread,
                void (*peek_fun) (void *arg,double frac),
                void *peek_arg);


/*! also returns distances to centroids (alloc'ed with malloc) */
float* knn (int n, int nb, int d, int k,
            const float *b, const float *v,
            int *assign,                                             
            void (*peek_fun) (void *arg,double frac),
            void *peek_arg);


float* knn_thread (int n, int nb, int d, int k,
                   const float *b, const float *v,
                   int *assign,    
                   int n_thread,
                   void (*peek_fun) (void *arg,double frac),
                   void *peek_arg);


/*! 
 * Computes a subset of L2 distances between b and v. 
 *
 *  @param n                  nb of vectors in v
 *  @param nb                 nb of vectors in b
 *  @param k                  nb of distances per v vector
 *  @param assign(k, n)       assign(:, i) is the set of vectors of b for 
 *                            which we have to compute distances to v(:,i). 
 *                            On output, assign(:,i) is reordered
 *  @param dists(k,n)         On output, distances corresponding to the assign array.
 */
void knn_reorder_shortlist(int n, int nb, int d, int k,
                           const float *b, const float *v,
                           int *assign,
                           float *dists);


/*! Computes all distances between 2 sets of vectors 
 *
 * @param a(d, na)       set of vectors  
 * @param b(d, nb)       set of vectors
 * @param dist2(na, nb)  distances between all vectors of a and b. On output, 
 *
 *       dist2(i, j) = || a(:, i) - b(:, j) ||^2 = dist2[ i + na * j ]
 *
 * where 0 <= i < na and 0 <= j < nb
 */
void compute_cross_distances (int d, int na, int nb,
                              const float *a,
                              const float *b, float *dist2);

/*! compute_cross_distances for non-packed matrices 
 * 
 * @param lda            size in memory of one vector of a
 * @param ldb            size in memory of one vector of b
 * @param ldd            size in memory of one vector of dist2
 */
void compute_cross_distances_nonpacked (int d, int na, int nb,
                                        const float *a, int lda,
                                        const float *b, int ldb, 
                                        float *dist2, int ldd);
/*! compute_cross_distances with threads */
void compute_cross_distances_thread (int d, int na, int nb,
                                     const float *a,
                                     const float *b, float *dist2,
                                     int nt);



/*! alternative distances. 
 *
 * @param distance_type    type of distance to compute: 
 *    - 1: L1
 *    - 2: L2 (use compute_cross_distances for optimized version!) 
 *    - 3: symmetric chi^2  
 *    - 4: symmetric chi^2  with absolute value
 */
void compute_cross_distances_alt (int distance_type, int d, int na, int nb,
                                  const float *a,
                                  const float *b, float *dist2);


void compute_cross_distances_alt_thread (int distance_type,int d, int na, int nb,
                                         const float *a,
                                         const float *b, float *dist2,
                                         int nt);


/*! version of compute_cross_distances where na==1 */
void compute_distances_1 (int d, int nb,
                          const float *a, 
                          const float *b,                         
                          float *dist2); 

void compute_distances_1_nonpacked (int d, int nb,
                                    const float *a, 
                                    const float *b, int ldb, 
                                    float *dist2);




/*! @} */

#endif

/*---------------------------------------------------------------------------*/

