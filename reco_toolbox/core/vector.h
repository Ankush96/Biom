/*
Authors: Matthijs Douze & Herve Jegou 
Contact: matthijs.douze@inria.fr  herve.jegou@inria.fr

*/

#ifndef __vector_h
#define __vector_h

#include <stdio.h>


/*---------------------------------------------------------------------------*/
/*! @addtogroup vector
 *  @{
 */



/*! @defgroup vector 
 * Vectors are represented as C arrays of basic elements. Functions
 * operating on them are prefixed with:
 *
 * ivec_: basic type is int
 *
 * fvec_: basic type is float
 *
 * Vector sizes are passed explicitly, as long int's to allow for
 * large arrays on 64 bit machines. Vectors can be free'd with free().
 *
 *
 * Arrays of vectors are stored contiguously in memory. An array of n
 * float vectors of dimension d is 
 * 
 *   float *fv
 *
 * The i'th element of vector j of vector array vf, where 0 <= i < d
 * and 0 <= j < n is
 * 
 *   vf[ j * d + i ]
 * 
 * It can also be seen as a column-major matrix of size d, n.
 *
 */ 


/*! Alloc a new aligned vector of floating point values -- to be
 *  de-allocated with free. Some operations may be faster if input
 *  arrays are allocated with this function (data is suitably
 *  aligned). */
float * fvec_new (long n);

/*! Alloc an int array -- to be de-allocated with free. */
int *ivec_new (long n);

/*! create a vector initialized with 0's. */
float *fvec_new_0 (long n);

/*! create a vector initialized with 0's. */
int *ivec_new_0 (long n);

/*! create a vector initialized with NaN (to trace errors) */
float *fvec_new_nan (long n);

/*!  create a vector initialized with a specified value. */
float *fvec_new_set (long n, float val);

/*!  create a vector initialized with a specified value. */
int *ivec_new_set (long n, int val);

/*!  create a vector initialized with uniformly drawn samples in [0,1) */
float *fvec_new_rand (long n);

/*!  new vector [a,a+1,...b-1] */
int * ivec_new_range (long a, long b);

/*!  new vector initialized with another vector */
int * ivec_new_cpy (const int * v, long n);

/*!  new vector initialized with another vector */
float * fvec_new_cpy (const float * v, long n);

/*!  random permutation of 0..n-1 */ 
int *ivec_new_random_perm (int n);


/*!  select k random indexes among n (without repetition) */ 
int *ivec_new_random_idx  (int n, int k);


/*!  count occurrences
   @param k is the range of the values that may be encountered (assuming start at 0)
   @param v is the vector of values to be histrogramized, of length n
*/
int * ivec_new_histogram (int k, int * v, long n);

/*!  compute a hash value for the vector */
int ivec_hash (const int * v, long n);

/*!  all occurences of a value by another in a vector */
void ivec_replace (int * v, long n, int val, int replace_val);

/*!  count occurences of a value in the vector */
long ivec_count_occurrences (const int * v, long n, int val);
long fvec_count_occurrences (const float * v, long n, float val);

/*!  count the number of values below a threshold */
long fvec_count_lt(const float * v, long n, float val);
long ivec_count_lt(const int * v, long n, int val);

/*!  count number of values above a threshold */
long fvec_count_gt(const float * v, long n, float val);
long ivec_count_gt(const int * v, long n, int val);


/*---------------------------------------------------------------------------*/
/* Input/Output functions                                                    */
/* I/O of a single vector is supported only if it is smaller than 2^31       */ 
/*---------------------------------------------------------------------------*/


/*!  Read the number of vectors in a file and their dimension 
  (vectors of same size). Output the number of bytes of the file. */
long fvecs_fsize (const char * fname, int *d_out, int *n_out);
long ivecs_fsize (const char * fname, int *d_out, int *n_out);



/*!  write a vector into an open file */
int ivec_fwrite(FILE *f, const int *v, int d);
int fvec_fwrite(FILE *f, const float *v, int d);

/*!  write a vector without the dimension header */
int ivec_fwrite_raw (FILE *f, const int *v, long d);
int fvec_fwrite_raw (FILE *f, const float *v, long d);

/*!  write a set of vectors into an open file */
int ivecs_fwrite(FILE *f, int d, int n, const int *v);
int fvecs_fwrite (FILE *fo, int d, int n, const float *vf);

/*!  several integer vectors of identifcal length into an file */
int ivecs_write (const char *fname, int d, int n, const int *v);
int fvecs_write (const char *fname, int d, int n, const float *vf);
int fvecs_write_txt (const char * fname, int d, int n, const float *vf);


/*!  load float vectors from file.
 *
 * Returns nb of vectors read, or <0 on error
 */
int fvecs_new_read (const char *fname, int *d_out, float **vf);

int fvecs_new_fread_max (FILE *f, int *d_out, float **vf, long nmax);

/*! reads sparse vectors and return them as dense. d must be known */
int fvecs_new_read_sparse (const char *fname, int d, float **vf);

/*!  load float vector without allocating memory 
 *
 * Fills n*d array with as much vectors read from fname as possible.
 * Returns nb of vectors read, or <0 on error.
 */
int fvecs_read (const char *fname, int d, int n, float *v);

/*!  read a vector from a text file (one line per vector) */
int fvecs_read_txt (const char *fname, int d, int n, float *v);

/*!  read a single vector from a file

 * Fill a with a single float vector from fname offset o_f into file a
 * Returns <0 on error
 */
int fvec_read (const char *fname, int d, float *a, int o_f);

/*!  load float vectors from an open file. Return the dimension */
int fvec_fread (FILE * f, float * v);


float *fvec_fread_raw(FILE * f, long n);


/*!  read and allocate a an integer vector file */
int * ivec_new_read(const char *fname, int *d_out);

/*!  read an integer vector file from an open file and return the dimension */
int ivec_fread (FILE *f, int * v);

/*!  read several integer vectors from an ivec file. Return number read */
int ivecs_new_read (const char *fname, int *d_out, int **vi);

/*! load a few of ivecs */
int ivecs_new_fread_max (FILE *f, int *d_out, int **vi, long nmax);


/*!  display a float vector */
void fvec_print (const float * v, int n);
void fvec_fprintf (FILE * f, const float *v, int n, const char *fmt);

/*!  display an integer vector */
void ivec_print (const int * v, int n);
void ivec_fprintf (FILE * f, const int *v, int n, const char *fmt);

/*!  find first index of val (return -1 if not found) */
long ivec_index (const int * v, long n,int val);


/*---------------------------------------------------------------------------*/
/* Vector manipulation and elementary operations                             */
/*---------------------------------------------------------------------------*/

/*!  Set all the components of the vector v to 0 */
void fvec_0 (float * v, long n);
void ivec_0 (int * v, long n);

void fvec_nan (float * v, long n);


/*!  are all values 0? */
int fvec_all_0 (float * v, long n);
int ivec_all_0 (int * v, long n);

/*!  are all values finite? */
int fvec_all_finite (float * v, long n);

/*!  Set all the components of the vector v to the value val */
void fvec_set (float * v, long n, float val);
void ivec_set (int * v, long n, int val);

/*!  copy the vector from v2 to v1 */
void ivec_cpy (int * vdest, const int * vsource, long n);
void fvec_cpy (float * vdest, const float * vsource, long n);

/*!  Increment or decrement a vector by a scalar value */
void fvec_incr (float * v, long n, double scal);
void fvec_decr (float * v, long n, double scal);

/*!  Multiply or divide a vector by a scalar */
void fvec_mul_by (float * v, long n, double scal);
void fvec_div_by (float * v, long n, double scal);
/* resciprocal division */
void fvec_rdiv_by (float * v, long n, double scal);

/*!  Add or subtract two vectors. The result is stored in v1. */
void fvec_add (float * v1, const float * v2, long n);
void fvec_sub (float * v1, const float * v2, long n);

/*! v1 := v2-v1 */
void fvec_rev_sub (float * v1, const float * v2, long n);

/*! v1:= v2 * scal */
void fvec_add_mul (float * v1, const float * v2, long n, double scal);

/*!  Component-wise multiplication or division of two vectors (result in v1) */
void fvec_mul (float * v1, const float * v2, long n);
void fvec_div (float * v1, const float * v2, long n);

/*!  Normalize the vector for the given Minkowski norm. 
  The function return the norm of the original vector. 
  If the vector is all 0, it will be filled with NaNs. 
  This case can be identified when the return value is 0. */
double fvec_normalize (float * v, long n, double norm);

/*!  This function normalize a set of n d-dimensional vectors. 
  It returns the number of vectors whose norms was 0 (for which 
  the normalization has put some NaN values). */
int fvecs_normalize (float * v, long n, long d, double norm);

void fvec_round (float * v, long n);
void fvec_sqrt (float * v, long n);
void fvec_sqr (float * v, long n);
void fvec_exp (float * v, long n);
void fvec_log (float * v, long n);
void fvec_neg (float * v, long n);

/*!  signed square-root: y = sign(x)*sqrt(abs(x)) */
void fvec_ssqrt (float * v, long n);


void ivec_add (int * v1, const int * v2, long n);
void ivec_sub (int * v1, const int * v2, long n);
void ivec_mul_by (int * v1,long n, int scal);
void ivec_mod_by (int * v1,long n, int scal);
void ivec_add_scalar (int * v, long n, int scal);


/*! Replace the "Not a number" values by a given value */
int fvec_purge_nans(float * v, long n, float replace_value);


/*!  Shrink the vector, removing "Not a number" and inf values. 
  Returns new size */
long fvec_shrink_nonfinite(float * v, long n);

/*!  find 1st occurrence of a non-finite element */
long fvec_index_nonfinite (float * v, long n);

/*---------------------------------------------------------------------------*/
/* Vector measures and statistics                                            */
/*---------------------------------------------------------------------------*/

/*!  compute the sum of the value of the vector */
double fvec_sum (const float * v, long n);
long long ivec_sum (const int * v, long n);

double fvec_product (const float * v, long n);
long long ivec_product (const int * v, long n);

/*!  sum of squared components */
double fvec_sum_sqr (const float * v, long n);
long long ivec_sum_sqr (const int * v, long n);


/*!  compute the norm of a given vector */
double fvec_norm (const float * v, long n, double norm);

/*!  compute squared norm 2 */
double fvec_norm2sqr (const float * v, long n);

/*!  count the number of non-zeros elements */
long fvec_nz (const float * v, long n);
long ivec_nz (const int * v, long n);


/*!  compute the positions of the non-null positions.
  return the number of non-zeros positions. */
int fvec_find (const float *v, int n, int ** nzpos_out);
int ivec_find (const int *v, int n, int ** nzpos_out);

/*!  perform a random permutation on the elements of the vector */
void ivec_shuffle (int *v, long n);

/*!  entropy of the probability mass function represented by the vector */
double fvec_entropy (const float *pmf, int n);

/*!  entropy of a binary variable */
double binary_entropy (double p);

double ivec_unbalanced_factor(const int *hist, long n);

/*---------------------------------------------------------------------------*/
/* Distances and similarities                                                */
/*---------------------------------------------------------------------------*/

/*!  Return the Hamming distance (i.e., the number of different elements) */
long ivec_distance_hamming (const int * v1, const int * v2, long n);

/*!  Return the L2 distance between vectors */
double fvec_distance_L2 (const float * v1, const float * v2, long n);

/*!  Return the L1 distance between vectors */
double fvec_distance_L1 (const float * v1, const float * v2, long n);

/*!  Return the square L2 distance between vectors */
double fvec_distance_L2sqr (const float * v1, const float * v2, long n);

/*!  inner product between two vectors */
double fvec_inner_product (const float * v1, const float * v2, long n);

/*---------------------------------------------------------------------------
 * Sparse vector handling
 *
 * sparse vectors are represented with: int *idx, float *val, int nz.
 * 
 * Element idx[i] of vector is val[i], for i=0..nz-1
 *
 * for i=1..nz-1,  idx[i-1]<idx[i]
 *---------------------------------------------------------------------------*/


/*!  convert a vector to a sparse vector. 
  Return the number of non-zeros positions  */
int fvec_to_spfvec (float * v, int n, int ** idx_out, float ** v_out);
int ivec_to_spivec (int * v, int n, int ** idx_out, int ** v_out);


/*!  convert a sparse vector into a full vector */
float * spfvec_to_fvec (int * idx, float * v, int nz, int n);
int * spivec_to_ivec (int * idx, int * v, int nz, int n);

/*!  inner product between two sparse vectors */
float spfvec_inner_product (int *idx1, float *val1, int nz1, 
			    int *idx2, float *val2, int nz2);


/*---------------------------------------------------------------------------*/
/* Elaborate vector manipulations                                            */
/*---------------------------------------------------------------------------*/

/*! on output,
 *
 * sl_out[0] =               v[      0] + ... + v[sl[0]-1]
 *
 * sl_out[i] = sl_out[i-1] + v[sl[i-1]] + ... + v[sl[i]-1] for 0<i<n
 */
void ivec_accumulate_slices(const int *v,int *sl,int n); 


/*!  mapping operator: dest[i]:=src[map[i]] for i=0..n-1 */
void fvec_map (const float *src,const int *map,int n,float *dest);

/*!  inverse mapping operator: dest[imap[i]]:=src[i] for i=0..n-1 */
void fvec_imap(const float *src,const int *imap,int n,float *dest);

/*! 
 * for i=0..n-1, do 
 *    accu[assign[i]] += a[i]
 */ 
void fvec_splat_add(const float *a,int n,
                    const int *assign,float *accu); 

/*! 
 * for i=0..n-1, do 
 *    accu[i] += a[assign[i]]
 */ 
void fvec_isplat_add(const float *a,int n,
                     const int *assign,float *accu); 

/*! @} */
#endif
