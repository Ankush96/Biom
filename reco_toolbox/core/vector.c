/*
Authors: Matthijs Douze & Herve Jegou 
Contact: matthijs.douze@inria.fr  herve.jegou@inria.fr

*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "vector.h"


#ifdef __linux__
#include <malloc.h>
#else
static void *memalign(size_t ignored,size_t nbytes) {
  return malloc(nbytes); 
} 
#endif

#if defined(__OS2__)  || defined(__WINDOWS__) || defined(WIN32) || defined(WIN64) || defined(_MSC_VER)
#define LM_FINITE _finite /* MSVC */
#define LM_ISNAN _isnan /* MSVC */
#elif defined(__ICC) || defined(__INTEL_COMPILER) || defined(__GNUC__) 
#define LM_FINITE finite /* ICC, GCC */
#define LM_ISNAN  isnan
#else 
#define LM_FINITE finite /* other than MSVC, ICC, GCC, let's hope this will work */
#define LM_ISNAN  isnan
#endif  



float *fvec_new (long n)
{
  float *ret = (float *) memalign (16, sizeof (*ret) * n);
  if (!ret) 
  {
    fprintf (stderr, "fvec_new %ld : out of memory\n", n);
    abort();
  }
  return ret;
}

int *ivec_new (long n)
{
  int *ret = (int *) malloc (sizeof (*ret) * n);
  if (!ret) 
  {
    fprintf (stderr, "ivec_new %ld : out of memory\n", n);
    abort();
  }
  return ret;
}

float *fvec_new_0 (long n)
{
  float *ret = (float *) calloc (sizeof (*ret), n);
  if (!ret) 
  {
    fprintf (stderr, "fvec_new_0 %ld : out of memory\n", n);
    abort();
  }
  return ret;
}

float *fvec_new_nan (long n)
{
  float *ret = fvec_new(n);
  fvec_nan(ret,n);
  return ret;
}

float *fvec_new_rand (long n) 
{
  float *f=fvec_new (n);
  long i;
#if defined(__OS2__)  || defined(__WINDOWS__) || defined(WIN32) || defined(WIN64) || defined(_MSC_VER)
  for(i=0;i<n;i++) 
  {
	  f[i] = rand();
  }
#else
  for(i=0;i<n;i++) 
  {
	  f[i] = drand48();
  }
#endif
  return f;
}



int *ivec_new_0 (long n)
{
  int *ret = (int *) calloc (sizeof (*ret), n);
  if (!ret) 
  {
    fprintf (stderr, "ivec_new_0 %ld : out of memory\n", n);
    abort();
  }
  return ret;
}


float *fvec_new_set (long n, float val)
{
  int i;
  float *ret = (float *) calloc (sizeof (*ret), n);
  if (!ret) 
  {
    fprintf (stderr, "fvec_new_set %ld : out of memory\n", n);
    abort();
  }

  for (i = 0 ; i < n ; i++)
  {
    ret[i] = val;
  }

  return ret;
}


int *ivec_new_set (long n, int val)
{
  int i;
  int *ret = (int *) calloc (sizeof (*ret), n);
  if (!ret) 
  {
    fprintf (stderr, "ivec_new_set %ld : out of memory\n", n);
    abort();
  }

  for (i = 0 ; i < n ; i++)
  {
    ret[i] = val;
  }

  return ret;
}


int * ivec_new_range (long a, long b)
{
  int i;
  int *ret = (int *) calloc (sizeof (*ret), b - a);
  if (!ret) 
  {
    fprintf (stderr, "ivec_new_range : out of memory\n");
    abort();
  }

  for (i = a ; i < b ; i++)
  {
    ret[i - a] = i;
  }

  return ret;
}


int * ivec_new_cpy (const int * v, long n)
{
  int *ret = (int *) malloc (sizeof (int) * n);
  if (!ret) 
  {
    fprintf (stderr, "ivec_new %ld : out of memory\n", n);
    abort();
  }
  
  memcpy (ret, v, n * sizeof (*ret));
  return ret;
}


float * fvec_new_cpy (const float * v, long n) 
{
  float *ret = fvec_new(n);
  memcpy (ret, v, n * sizeof (*ret));
  return ret;  
}



int * ivec_new_random_idx (int n, int k)
{
  int *idx = ivec_new (n);
  int i , j , p;

  for (i = 0; i < n; i++)
  {
    idx[i] = i;
  }

  for (i = 0; i < k ; i++) {
#if defined(__OS2__)  || defined(__WINDOWS__) || defined(WIN32) || defined(WIN64) || defined(_MSC_VER)
	  j = i + (int)floor(rand () * (n - i));
#else
       j = i + random () % (n - i);
#endif
       /* swap i and j */
         p = idx[i];
    idx[i] = idx[j];
    idx[j] = p;
  }

  return idx;
}


int *ivec_new_random_perm (int n)
{
  return ivec_new_random_idx (n, n - 1);
}


int *ivec_new_histogram (int k, int *v, long n)
{
  long i;
  int *h = ivec_new_0 (k);

  for (i = 0; i < n; i++) 
  {
    assert (v[i] >= 0 && v[i] < k);
    h[v[i]]++;
  }

  return h;
}

void fvec_splat_add(const float *a,int n,const int *assign,float *accu) 
{
  int i;
  for(i=0;i<n;i++) 
  {
    accu[assign[i]] += a[i];
  }
}


void fvec_isplat_add(const float *a,int n,
                     const int *assign,float *accu) 
{
  int i;
  for(i=0;i<n;i++) 
  {
    accu[i] += a[assign[i]];
  }
  
}

void fvec_map(const float *src,const int *map,int n,float *dest) 
{
  int i;
  for(i=0;i<n;i++) 
  {
    dest[i]=src[map[i]];
  }
}

void fvec_imap(const float *src,const int *imap,int n,float *dest) 
{
  int i;
  for(i=0;i<n;i++) 
  {
    dest[imap[i]]=src[i];
  }
}


int ivec_hash(const int * v, long n) 
{
  unsigned int *k   = (unsigned int*)v;
  unsigned int b    = 378551;
  unsigned int a    = 63689;
  unsigned int hash = 0;
  int i;
  for(i = 0; i < n; i++) 
  {
    hash = hash * a + k[i];
    a    = a * b;
  }
  return hash;
}

void ivec_replace(int * v, long n,int val,int replace_val) 
{
  while(n--) if(v[n]==val) v[n]=replace_val;  
}

long ivec_count_occurrences(const int * v, long n, int val) 
{
  long count=0;
  while(n--) if(v[n]==val) count++;
  return count;
}

long fvec_count_occurrences(const float * v, long n, float val) 
{
  long count=0;
  while(n--) if(v[n]==val) count++;
  return count;
}

long fvec_count_lt(const float * v, long n, float val) 
{
  long count=0;
  while(n--) if(v[n]<val) count++;
  return count;
}

long ivec_count_lt(const int * v, long n, int val) 
{
  long count=0;
  while(n--) if(v[n]<val) count++;
  return count;
}

long fvec_count_gt(const float * v, long n, float val) 
{
  long count=0;
  while(n--) if(v[n]>val) count++;
  return count;
}

long ivec_count_gt(const int * v, long n, int val) 
{
  long count=0;
  while(n--) if(v[n]>val) count++;
  return count;
}


void ivec_accumulate_slices(const int *v,int *sl,int n) 
{
  int i;
  int accu=0,j=0;
  for(i=0;i<n;i++) 
  {
    while(j<sl[i]) 
      accu+=v[j++];
    sl[i]=accu;
  }
}



/*---------------------------------------------------------------------------*/
/* Input/Output functions                                                    */
/*---------------------------------------------------------------------------*/

long fvecs_fsize (const char * fname, int *d_out, int *n_out)
{
  int d, ret; 
  long nbytes;

  FILE * f = fopen (fname, "r");
  
  /* read the dimension from the first vector */
  ret = fread (&d, sizeof (d), 1, f);
  if (ret == 0)
  {
    return ret;
  }
  
  fseek (f, 0, SEEK_END);
  nbytes = ftell (f);
  fclose (f);
  
  assert (nbytes % (d + 1) == 0);   /* checksum */

  *d_out = d;
  *n_out = nbytes / (4 * d + 4);
  return nbytes;
}


long ivecs_fsize (const char * fname, int *d_out, int *n_out)
{
  return fvecs_fsize (fname, d_out, n_out);
}




int fvec_fwrite (FILE *fo, const float *v, int d) 
{
  int ret;
  ret = fwrite (&d, sizeof (int), 1, fo);
  if (ret != 1) 
  {
    perror ("fvec_fwrite: write error 1");
    return -1;
  }
  ret = fwrite (v, sizeof (float), d, fo);
  if (ret != d) 
  {
    perror ("fvec_fwrite: write error 2");
    return -1;
  }  
  return 0;
}




int fvec_fwrite_raw(FILE *fo, const float *v, long d) 
{
  long ret = fwrite (v, sizeof (float), d, fo);
  if (ret != d) 
  {
    perror ("fvec_fwrite_raw: write error 2");
    return -1;
  }  
  return 0;
}

int ivec_fwrite_raw(FILE *fo, const int *v, long d) 
{
  long ret = fwrite (v, sizeof (int), d, fo);
  if (ret != d) 
  {
    perror ("ivec_fwrite_raw: write error 2");
    return -1;
  }  
  return 0;
}


int fvecs_fwrite (FILE *fo, int d, int n, const float *vf)
{
  int i;
  /* write down the vectors as fvecs */
  for (i = 0; i < n; i++) 
  {
    if(fvec_fwrite(fo, vf+i*d, d)<0) 
	{
      return i;
	}
  }
  return n;
}



int fvecs_write (const char *fname, int d, int n, const float *vf)
{
  int ret;
  FILE *fo = fopen (fname, "w");
  if (!fo) 
  {
    perror ("fvecs_write: cannot open file");
    return -1;
  }

  ret = fvecs_fwrite (fo, d, n, vf);

  fclose (fo);
  return ret;
}


int fvecs_write_txt (const char * fname, int d, int n, const float *vf)
{ 
  int i, j, ret = 0;
  FILE * fo = fopen (fname, "w");
  if (!fo) 
  {
    perror ("fvecs_write_txt: cannot open file");
    return -1;
  }

  for (i = 0 ; i < n ; i++) 
  {
    for (j = 0 ; j < d ; j++)
      fprintf (fo, "%f ", vf[i * d + j]);
    ret += fprintf (fo, "\n");
  }

  return ret;
}


int fvecs_new_read (const char *fname, int *d_out, float **vf_out)
{
  int n;
  FILE *f = fopen (fname, "r");
  if (!f) 
  {
    fprintf (stderr, "fvecs_new_read: could not open %s\n", fname);
    perror ("");
    *d_out=-1;
    vf_out=NULL;
    return -1;
  }

  n=fvecs_new_fread_max(f,d_out,vf_out,-1);

  fclose (f);
  
  return n;
}

int fvecs_new_fread_max (FILE *f, int *d_out, float **vf_out, long nmax)
{
  int n, new_d , d = -1;

  float *vf=NULL;
  long nalloc=0;
  
  for(n=0; nmax<0 || n<nmax ; n++) 
  {

    if (fread (&new_d, sizeof (int), 1, f) != 1) 
	{
      if (feof (f))
	  {
        break;
	  }
      else 
	  {
        perror ("fvecs_new_read error 1");
        goto error;
      }
    }

	if (n == 0)
	{
		d = new_d;
	}
	else if (new_d != d) 
	{
		fprintf (stderr, "fvecs_new_read: non-uniform vectors sizes: new_d=%d d=%d\n",new_d,d);
		goto error;
	}

    if((n+1)*d>nalloc) 
	{
      do 
	  {
        nalloc = nalloc==0 ? 1024 : nalloc*2;
      } 
	  while((n+1)*d>nalloc);
      vf = realloc(vf,nalloc*sizeof(float));
      assert(vf || !"out of memory!");
    }

    if (fread (vf+n*d, sizeof (float), d, f) != d) 
	{
      fprintf (stderr, "fvecs_new_read error 3");
      goto error;
    }

  }
  vf=realloc(vf,n*d*sizeof(float));

  *vf_out = vf;
  *d_out = d;
  return n;
 error: 
  free(vf); 
  *vf_out = NULL;
  *d_out = -1;
  return -1;
}

int fvecs_new_read_sparse (const char *fname, int d, float **vf_out) 
{
  float *vf=NULL , *dense;
  long n=0,na=0;
  float *vals=fvec_new(d);
  int *idx=ivec_new(d);
  
  FILE *f = fopen (fname, "r");
#define E(msg) {                                                \
  fprintf (stderr, "fvecs_new_read_sparse %s: " msg , fname);   \
  perror ("");                                                  \
  free(vf); free(vals); free(idx);                              \
  return -1;                                                    \
}
  if (!f) E("");
  
  while(!feof(f)) 
  {
    int nz,ret,nz2;
    ret=fread(&nz,sizeof(int),1,f);
    if(ret!=1) 
	{
      if(feof(f)) break;
      E("err 1");
    }
    if(fread(idx,sizeof(int),nz,f)!=nz) E("err 2");
    if(fread(&nz2,sizeof(int),1,f)!=1) E("err 3");
    if(nz!=nz2) E("err 4");
    if(fread(vals,sizeof(float),nz,f)!=nz) E("err 5");
    
    if(n>=na) 
	{
      na=(na+1)*3/2;
      vf=realloc(vf,na*sizeof(float)*d);
    }
    
    dense=spfvec_to_fvec (idx,vals,nz,d);
    memcpy(vf+n*d,dense,sizeof(float)*d);
    free(dense);
    
    n++;       
  }
#undef E
  free(vals);
  free(idx);
  fclose(f);
  *vf_out=vf;
  return n;
}




int fvecs_read (const char *fname, int d, int n, float *a)
{
  int i , new_d;
  FILE *f = fopen (fname, "r");
  if (!f) 
  {
    fprintf (stderr, "fvecs_read: could not open %s\n", fname);
    perror ("");
    return -1;
  }

  for (i = 0; i < n; i++) 
  {

    if (fread (&new_d, sizeof (int), 1, f) != 1) 
	{
      if (feof (f))
        break;
      else 
	  {
        perror ("fvecs_read error 1");
        /* TODO free mem*/
        return -1;
      }
    }

    if (new_d != d) 
	{
      fprintf (stderr, "fvecs_read error 2: unexpected vector dimension\n");
      return -1;
    }

    if (fread (a + d * (long) i, sizeof (float), d, f) != d) 
	{
      fprintf (stderr, "fvecs_read error 3\n");
      return -1;
    }
    n++;
  }
  fclose (f);

  return i;
}

int fvecs_read_txt (const char *fname, int d, int n, float *v)
{
  int i, ret;
  FILE * f = fopen (fname, "r");
  if (!f) 
  {
    fprintf (stderr, "fvecs_read_txt: could not open %s\n", fname);
    perror ("");
    return -1;
  }

  for (i = 0; i < n * d ; i++) 
  {
    ret = fscanf (f, "%f", v + i);
    if (ret != 1) 
	{
      if (feof (f))
	break;
      else 
	  {
        perror ("fvecs_read_txt error 1");
        /* TODO free mem*/
        return -1;
      }
    }
  }
  fclose (f);

  return i / d;
}



int fvec_read (const char *fname, int d, float *a, int o_f)
{
  int d2 , ret;
  FILE *f = fopen (fname, "r");
  if (!f) {
    fprintf (stderr, "fvec_read: could not open %s\n", fname);
    perror ("");
    return -1;
  }

  if (fseek (f, (sizeof (int) + sizeof (float) * d) * o_f, SEEK_SET) != 0) 
  {
    fprintf (stderr, "fvec_read %s fseek", fname);
    perror ("");
    return -1;
  }

  d2  = 0;
  ret = fread (&d2, sizeof (int), 1, f);
  if (d != d2) {
    fprintf (stderr, "fvec_read %s bad dim: %d!=%d\n", fname, d, d2);
    return -1;
  }

  ret = fread (a, sizeof (float), d, f);
  fclose (f);
  return ret;
}



int fvec_fread (FILE * f, float * v)
{
  int d;
  int ret = fread (&d, sizeof (int), 1, f);

  if (ret != 1) {
    perror ("# fvec_fread error 1");
    return -1;
  }

  ret = fread (v, sizeof (*v), d, f);
  if (ret != d) {
    perror ("# fvec_fread error 2");
    return -1;
  }

  return d;
}


float *fvec_fread_raw(FILE * f, long d) {
  float *v=fvec_new(d);

  int ret = fread (v, sizeof (*v), d, f);
  if (ret != d) {
    free(v);
    perror ("# fvec_fread error 2");
    return NULL;
  }
  return v;
}

int ivec_fwrite (FILE *f, const int *v, int d)
{
  int ret = fwrite (&d, sizeof (d), 1, f);
  if (ret != 1) {
    perror ("ivec_fwrite: write error 1");
    return -1;
  }

  ret = fwrite (v, sizeof (*v), d, f);
  if (ret != d) {
    perror ("ivec_fwrite: write error 2");
    return -2;
  }
  return 0;
}


int ivecs_fwrite(FILE *f, int d, int n, const int *v)
{
  int i;
  for (i = 0 ; i < n ; i++) {    
    ivec_fwrite (f, v, d);
    v+=d;
  }
  return n;
}


int ivecs_write (const char *fname, int d, int n, const int *v)
{
  int ret = 0;
  FILE *f = fopen (fname, "w");
  if (!f) {
    perror ("ivecs_write");
    return -1;
  }

  ret = ivecs_fwrite (f, d, n, v);

  fclose (f);
  return ret;
}


int ivecs_new_fread_max (FILE *f, int *d_out, int **vi_out, long nmax)
{
  
  assert(sizeof(int)==sizeof(float)); /* that's all what matters */

  return fvecs_new_fread_max(f,d_out,(float**)vi_out,nmax);
}


int ivecs_new_read (const char *fname, int *d_out, int **vi_out)
{
  int n;
  FILE *f = fopen (fname, "r");
  if (!f) {
    fprintf (stderr, "ivecs_new_read: could not open %s\n", fname);
    perror ("");
    *d_out=-1; *vi_out=NULL;
    return -1;
  }
    
  n=ivecs_new_fread_max(f,d_out,vi_out,-1);
 
  fclose(f);
  
  return n;
}


int *ivec_new_read(const char *fname, int *d_out) 
{
  int d, n;
  int *vi;
  n = ivecs_new_read(fname,&d,&vi);
  if (n<0) 
    return NULL;
  assert (n==1);
  if (d_out) 
    *d_out=d;
  return vi;
}


int ivec_fread (FILE * f, int * v)
{
  int d;
  int ret = fread (&d, sizeof (int), 1, f);

  if (ret != 1) {
    perror ("# ivec_fread error 1");
    return -1;
  }

  ret = fread (v, sizeof (*v), d, f);
  if (ret != d) {
    perror ("# ivec_fread error 2");
    return -1;
  }
  return d;
}



void fvec_print (const float * v, int n)
{
  int i;
  printf ("[");
  for (i = 0 ; i < n ; i++)
    printf ("%g ", v[i]);
  printf ("]\n");
}


void ivec_print (const int * v, int n)
{
  int i;
  printf ("[");
  for (i = 0 ; i < n ; i++)
    printf ("%d ", v[i]);
  printf ("]\n");
}


void fvec_fprintf (FILE * f, const float *v, int n, const char *fmt)
{
  int i;
  if (fmt == NULL)
    fmt = "%f ";

  for (i = 0; i < n; i++)
    fprintf (f, fmt, v[i]);
}


void ivec_fprintf (FILE * f, const int *v, int n, const char *fmt)
{
  int i;
  if (fmt == NULL)
    fmt = "%d ";

  for (i = 0; i < n; i++)
    fprintf (f, fmt, v[i]);
}




/*---------------------------------------------------------------------------*/
/* Vector manipulation                                                       */
/*---------------------------------------------------------------------------*/

void fvec_0(float * v, long n)
{
  memset (v, 0, n * sizeof (*v));
}

void fvec_nan (float * v, long n) 
{
  memset (v, -1, n * sizeof (*v));
} 


void ivec_0(int * v, long n)
{
  memset (v, 0, n * sizeof (*v));
}

int fvec_all_0 (float * v, long n) 
{
  while(n--) if(v[n]!=0) return 0;
  return 1;
}


int fvec_all_finite (float * v, long n) 
{
  while(n--) if(!LM_FINITE(v[n])) return 0;
  return 1;
}


int ivec_all_0 (int * v, long n) 
{
  while(n--) if(v[n]!=0) return 0;
  return 1;
}

void fvec_set (float * v, long n, float val)
{
  long i;
  for (i = 0 ; i < n ; i++)
  {
    v[i] = val;
  }
}


void ivec_set (int * v, long n, int val)
{
  long i;
  for (i = 0 ; i < n ; i++)
  {
    v[i] = val;
  }
}


void ivec_cpy (int * vdest, const int * vsource, long n)
{
  memcpy (vdest, vsource, n * sizeof (*vdest));
}


void fvec_cpy (float * vdest, const float * vsource, long n)
{
  memcpy (vdest, vsource, n * sizeof (*vdest));
}


void fvec_incr (float * v, long n, double scal)
{
  long i = 0;
  float fscal = (float) scal;
  for (i = 0 ; i < n ; i++)
  {
    v[i] += fscal;
  }
}


void fvec_decr (float * v, long n, double scal)
{
  fvec_incr(v, n, -scal);  
}


void fvec_mul_by (float * v, long n, double scal)
{
  long i = 0;  
  float fscal = (float) scal;

  for (i = 0 ; i < n ; i++)
  {
    v[i] *= fscal;
  }
}

void fvec_div_by (float * v, long n, double scal)
{
  fvec_mul_by(v, n, 1. / scal);
}

void fvec_rdiv_by (float * v, long n, double scal) 
{
  long i = 0;
  float fscal = (float) scal;

  for (i = 0 ; i < n ; i++)
  {
    v[i] = fscal / v[i];
  }
}


void fvec_add (float * v1, const float * v2, long n)
{
  long i = 0;
  for (i = 0 ; i < n ; i++)
  {
    v1[i] += v2[i];
  }
}

void fvec_add_mul (float * v1, const float * v2, long n,double scal) 
{
  long i = 0;
  float fscal = (float) scal;

  for (i = 0 ; i < n ; i++)
  {
    v1[i] += v2[i]*fscal;
  }
}

void fvec_sub (float * v1, const float * v2, long n)
{
  long i = 0;
  for (i = 0 ; i < n ; i++)
  {
    v1[i] -= v2[i];
  }
}

void fvec_rev_sub (float * v1, const float * v2, long n) {
  long i = 0;
  for (i = 0 ; i < n ; i++)
  {
    v1[i] = v2[i] - v1[i];
  }
}

void fvec_mul (float * v1, const float * v2, long n)
{
  long i = 0;
  for (i = 0 ; i < n ; i++)
  {
    v1[i] *= v2[i];
  }
}


void fvec_div (float * v1, const float * v2, long n)
{
  long i = 0;
  for (i = 0 ; i < n ; i++)
  {
    v1[i] /= v2[i];
  }
}


double fvec_normalize (float * v, long n, double norm)
{
 double nr;
  if(norm==0) 
  {
    return 0;
  }

  nr = fvec_norm (v, n, norm);

  /*  if(nr!=0)*/
  fvec_mul_by (v, n, 1 / nr);
  return nr;
}


int fvecs_normalize (float * v, long n, long d, double norm)
{
  int i, nNaN = 0;
  if (norm == 0) 
  {
    return 0;
  }

  for (i = 0 ; i < n ; i++) 
  {
    double nr = fvec_norm (v + i * d, d, norm);
    if (nr == 0)
	{
      nNaN++;
	}

    fvec_mul_by (v + i * d, d, 1 / nr);
  }

  return nNaN;
}


int fvec_purge_nans(float * v, long n, float replace_value) 
{
  long i, count=0;
  
  for(i=0;i<n;i++) if(LM_ISNAN(v[i])) 
  {
    count++;
    v[i]=replace_value;
  }
  
  return count;
}




long fvec_shrink_nonfinite(float * v, long n) 
{
  long i,j=0;
  
  for(i=0;i<n;i++) if(LM_FINITE(v[i])) 
    v[j++]=v[i];
  return j;  
}




long fvec_index_nonfinite(float * v, long n) {
  long i;
  for(i=0;i<n;i++) if(!LM_FINITE(v[i])) return i;
  return -1;  
}



void fvec_round (float * v, long n)
{
  long i = 0;
  for (i = 0 ; i < n ; i++)
    v[i] = (int)(v[i]+0.5);
}

void fvec_sqrt (float * v, long n)
{
  long i;
  for (i = 0 ; i < n ; i++)
  {
    v[i] = (float)sqrt (v[i]);
  }
}


void fvec_sqr (float * v, long n)
{
  long i;
  for (i = 0 ; i < n ; i++)
  {
    v[i] =  v[i] * v[i];
  }
}


void fvec_exp (float * v, long n)
{
  long i;
  for (i = 0 ; i < n ; i++)
  {
    v[i] = (float)exp (v[i]);
  }
}


void fvec_log (float * v, long n)
{
  long i;
  for (i = 0 ; i < n ; i++)
  {
    v[i] = (float)log (v[i]);
  }
}

void fvec_neg (float * v, long n) 
{
  long i;
  for (i = 0 ; i < n ; i++)
  {
    v[i] = -v[i];
  }
}

void fvec_ssqrt (float * v, long n) 
{
  long i;
  for (i = 0 ; i < n ; i++)
  {
    v[i] = v[i]>0 ? (float)sqrt(v[i]) : (float)(-sqrt(-v[i]));
  }

}

void ivec_add (int * v1, const int * v2, long n)
{
  long i = 0;
  for (i = 0 ; i < n ; i++)
  {
    v1[i] += v2[i];
  }
}

void ivec_sub (int * v1, const int * v2, long n)
{
  long i = 0;
  for (i = 0 ; i < n ; i++)
  {
    v1[i] -= v2[i];
  }
}

void ivec_mul_by (int * v,long n, int scal) 
{
  long i = 0;
  for (i = 0 ; i < n ; i++)
  {
    v[i] *= scal;
  }
}

void ivec_mod_by (int * v,long n, int scal) 
{
  long i = 0;
  for (i = 0 ; i < n ; i++)
  {
    v[i] = v[i] % scal;
  }
}


void ivec_add_scalar (int * v, long n, int scal) 
{
  long i = 0;
  for (i = 0 ; i < n ; i++)
  {
    v[i] += scal;
  }
}


/*---------------------------------------------------------------------------*/
/* Vector measures and statistics                                            */
/*---------------------------------------------------------------------------*/

double fvec_sum (const float * v, long n)
{
  long i;
  double s = 0;
  for (i = 0 ; i < n ; i++)
  {
    s += v[i];
  }

  return s;
}


double fvec_sum_sqr (const float * v, long n)
{
  long i;
  double s = 0;
  for (i = 0 ; i < n ; i++)
  {
    s += v[i] * (double) v[i];
  }

  return s;
}


double fvec_product (const float * v, long n) 
{
  double p=1.0;
  while(n--) p*=v[n];
  return p;
}


long long ivec_product (const int * v, long n) 
{
  long long p=1;
  while(n--) p*=v[n];
  return p;  
}



long long ivec_sum (const int * v, long n)
{
  long i;
  long long s = 0;
  for (i = 0 ; i < n ; i++)
  {
    s += v[i];
  }

  return s;
}

long long ivec_sum_sqr (const int * v, long n)
{
  long i;
  long long s = 0;
  for (i = 0 ; i < n ; i++)
  {
    s += v[i]*(long long)v[i];
  }

  return s;
}


double fvec_norm (const float * v, long n, double norm)
{
	long i;
	double s = 0;

	if(norm==0) return n;


	if(norm==1) 
	{
		for (i = 0 ; i < n ; i++)
		{
			s += fabs(v[i]);
		}
		return s;
	}

	if(norm==2) 
	{
		for (i = 0 ; i < n ; i++)
		{
			s += v[i]*v[i];
		}
		return sqrt(s);
	}

	for (i = 0 ; i < n ; i++)
	{
		s += pow (v[i], norm);
	}

	return pow (s, 1 / norm);
}


double fvec_norm2sqr (const float * v, long n) 
{
  double s=0;
  long i;
  for (i = 0 ; i < n ; i++)
  {
    s += v[i]*v[i];
  }
  return s;
}

long fvec_nz (const float * v, long n)
{
  long i, nz = 0;
  for (i = 0 ; i < n ; i++)
    if (v[i] != 0)
      nz++;

  return nz;
}


long ivec_nz (const int * v, long n)
{
  long i, nz = 0;
  for (i = 0 ; i < n ; i++)
    if (v[i] != 0)
      nz++;

  return nz;
}


int fvec_find (const float *v, int n, int ** nzpos_out)
{
  int nz = fvec_nz (v, n);
  int * nzpos = ivec_new (nz);
  int i, ii = 0;

  for (i = 0 ; i < n ; i++) 
    if (v[i] != 0)
	{
      nzpos[ii] = i;
      ii++;
    }

  *nzpos_out = nzpos;
  return nz;
}


int ivec_find (const int *v, int n, int ** nzpos_out)
{
  int nz = ivec_nz (v, n);
  int * nzpos = ivec_new (nz);
  int i, ii = 0;

  for (i = 0 ; i < n ; i++)
  {
    if (v[i] != 0) 
	{
      nzpos[ii] = i;
      ii++;
    }
  }

  *nzpos_out = nzpos;
  return nz;
}


void ivec_shuffle (int * v, long n)
{
  int i , j , p;

  for (i = 0; i < n - 1; i++) 
  {
#if defined(__OS2__)  || defined(__WINDOWS__) || defined(WIN32) || defined(WIN64) || defined(_MSC_VER)
	  j = i + rand()*(n - i);
#else
      j = i + random () % (n - i);
#endif

    /* swap i and j */
     p   = v[i];
    v[i] = v[j];
    v[j] = p;
  }
}




double fvec_entropy (const float *pmf, int n)
{
  int i;
  double minusent = 0;

  for (i = 0 ; i < n ; i++)
  {
    if (pmf[i] > 0)
	{
      minusent += pmf[i] * log (pmf[i]);
	}
  }

  return - minusent / log(2);
}

/*! @brief entropy of a binary variable */
double binary_entropy (double p)
{
  if (p == 0 || p == 1)
  {
    return 0;
  }
  return -(p * log (p) + (1-p) * log (1-p)) / log (2);
}

double ivec_unbalanced_factor(const int *hist, long n) 
{
  int vw;
  double tot = 0, uf = 0;

  for (vw = 0 ; vw < n ; vw++)
  {
    tot += hist[vw];
    uf  += hist[vw] * (double) hist[vw];
  }

  uf = uf * n / (tot * tot);

  return uf;

}

/*---------------------------------------------------------------------------*/
/* Distances                                                                 */
/*---------------------------------------------------------------------------*/

long ivec_distance_hamming (const int * v1, const int * v2, long n)
{
  long i, dis = 0; 

  for (i = 0 ; i < n ; i++)
  {
    dis += (v1[i] == v2[i] ? 0 : 1);
  }

  return dis;
}


double fvec_distance_L2 (const float * v1, const float * v2, long n)
{
  return sqrt (fvec_distance_L2sqr (v1, v2, n));
}


double fvec_distance_L1 (const float * v1, const float * v2, long n) 
{
  long i;
  double dis = 0;

  for (i = 0 ; i < n ; i++) 
  {
    dis += fabs(v1[i] - v2[i]);
  }

  return dis;  
}

double fvec_distance_L2sqr (const float * v1, const float * v2, long n)
{
  long i;
  double dis = 0, a;

  for (i = 0 ; i < n ; i++) 
  {
    a    = (double) v1[i] - v2[i];
    dis += a * a;
  }

  return dis;  
}


double fvec_inner_product (const float * v1, const float * v2, long n)
{
  long i;
  double res = 0;
  for (i = 0 ; i < n ; i++)
  {
    res += v1[i] * v2[i];
  }
  return res;
}


/*---------------------------------------------------------------------------*/
/* Sparse vector handling                                                    */
/*---------------------------------------------------------------------------*/

int fvec_to_spfvec (float * v, int n, int ** idx_out, float ** v_out)
{
  int i, ii = 0;
  int nz = fvec_nz (v, n);
  int * idx = ivec_new (nz);
  float * val = fvec_new (nz);

  for (i = 0 ; i < n ; i++) 
  {
    if (v[i] != 0) 
	{
      idx[ii] = i;
      val[ii] = v[i];
      ii++;
    }
  }

  *idx_out = idx;
  *v_out = val;
  return nz;
}


int ivec_to_spivec (int * v, int n, int ** idx_out, int ** v_out)
{
  int i, ii = 0;
  int nz = ivec_nz (v, n);
  int * idx = ivec_new (nz);
  int * val = ivec_new (nz);

  for (i = 0 ; i < n ; i++) 
    if (v[i] != 0) 
	{
      idx[ii] = i;
      val[ii] = v[i];
      ii++;
    }

  *idx_out = idx;
  *v_out = val;
  return nz;
}


float * spfvec_to_fvec (int * idx, float * v, int nz, int n)
{
  int i;
  float * ret = fvec_new_0 (n);
  for (i = 0 ; i < nz ; i++)
  {
    ret[idx[i]] = v[i];
  }

  return ret;
}


int * spivec_to_ivec (int * idx, int * v, int nz, int n)
{
  int i;
  int * ret = ivec_new_0 (n);
  for (i = 0 ; i < nz ; i++)
  {
    ret[idx[i]] = v[i];
  }

  return ret;
}


float spfvec_inner_product (int *idx1, float *val1, int nz1, int *idx2, float *val2, int nz2)
{
  double s = 0;
  long i1 = 0, i2 = 0;

  while (1) 
  {

    if (i1 == nz1) 
      break;

    if (i2 == nz2) 
      break;

	if (idx1[i1] == idx2[i2]) 
	{
		s += val1[i1] * val2[i2];
		i1++;
		i2++;
	}
	else 
	{
		if (idx1[i1] < idx2[i2])
		{
			i1++;
		}

		else
		{
			i2++;
		}
	}
  }
  return (float)s;
}


long ivec_index(const int * v, long n,int val) 
{
  long i;
  for(i=0;i<n;i++) 
  {
	  if(v[i]==val) 
	  {
		  return i;
	  }
  }
  return -1;
}


