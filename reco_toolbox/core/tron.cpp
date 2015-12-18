/*




C:\Program Files\MATLAB\R2009b\extern\lib\win32\microsoft






*/



#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "tron.h"

#ifdef BLAS
 #include <omp.h>
#endif


#if defined(__OS2__)  || defined(__WINDOWS__) || defined(WIN32) || defined(_MSC_VER)
#define BLASCALL(f) f
#else
#define BLASCALL(f) f ##_
#endif


#ifndef min
template <class T> static inline T min(T x,T y) { return (x<y)?x:y; }
#endif

#ifndef max
template <class T> static inline T max(T x,T y) { return (x>y)?x:y; }
#endif

#ifdef __cplusplus
extern "C" {
#endif



#ifdef BLAS
extern int BLASCALL(daxpy)(int *, double *, double *, int *, double *, int *);
extern int BLASCALL(dscal)(int *, double *, double *, int *);
extern double BLASCALL(ddot)(int *, double *, int *, double *, int *);
extern double BLASCALL(dnrm2)(int *, double *, int *);
#else

extern double dnrm2_(int *, double *, int *);
extern double ddot_(int *, double *, int *, double *, int *);
extern int daxpy_(int *, double *, double *, int *, double *, int *);
extern int dscal_(int *, double *, double *, int *);


#endif
#ifdef __cplusplus
}
#endif



static void default_print(const char *buf)
{
	fputs(buf,stdout);
	fflush(stdout);
}

void TRON::info(const char *fmt,...)
{
	char buf[BUFSIZ];
	va_list ap;
	va_start(ap,fmt);
	vsprintf(buf,fmt,ap);
	va_end(ap);
	(*tron_print_string)(buf);
}

TRON::TRON(const function *fun_obj, double eps, int max_iter)
{
	this->fun_obj=const_cast<function *>(fun_obj);
	this->eps=eps;
	this->max_iter=max_iter;
	tron_print_string = default_print;
}

TRON::~TRON()
{
}

void TRON::tron(double *w)
{
	/* Parameters for updating the iterates. */
	double eta0 = 1e-4, eta1 = 0.25, eta2 = 0.75;

	/* Parameters for updating the trust region size delta. */
	double sigma1 = 0.25, sigma2 = 0.5, sigma3 = 4;

	int n = fun_obj->get_nr_variable();
	int i, cg_iter;
	double delta, snorm, one=1.0;
	double alpha, f, fnew, prered, actred, gs;
	int search = 1, iter = 1, inc = 1;
	double *s = new double[n];
	double *r = new double[n];
	double *w_new = new double[n];
	double *g = new double[n];

	for (i=0; i<n; i++)
		w[i] = 0;

     f = fun_obj->fun(w);
	fun_obj->grad(w, g);

#ifdef BLAS
	delta = BLASCALL(dnrm2)(&n, g, &inc);
#else
	delta = dnrm2_(&n, g, &inc);

#endif
	double gnorm1 = delta;
	double gnorm = gnorm1;

	if (gnorm <= eps*gnorm1)
		search = 0;

	iter = 1;

	while (iter <= max_iter && search)
	{
		cg_iter = trcg(delta, g, s, r);

		memcpy(w_new, w, sizeof(double)*n);
/*		daxpy_(&n, &one, s, &inc, w_new, &inc); */

#ifdef BLAS
		BLASCALL(daxpy)(&n, &one, s, &inc, w_new, &inc);

		gs     = BLASCALL(ddot)(&n, g, &inc, s, &inc);
		prered = -0.5*(gs-BLASCALL(ddot)(&n, s, &inc, r, &inc));

#else
		daxpy_(&n, &one, s, &inc, w_new, &inc);

		gs     = ddot_(&n, g, &inc, s, &inc);
		prered = -0.5*(gs-ddot_(&n, s, &inc, r, &inc));

#endif
            fnew = fun_obj->fun(w_new);

		/* Compute the actual reduction. */
	        actred = f - fnew;

		/* On the first iteration, adjust the initial step bound. */

#ifdef BLAS
		snorm = BLASCALL(dnrm2)(&n, s, &inc);
#else
		snorm = dnrm2_(&n, s, &inc);

#endif
		if (iter == 1)
			delta = min(delta, snorm);

		/* Compute prediction alpha*snorm of the step. */
		if (fnew - f - gs <= 0)
			alpha = sigma3;
		else
			alpha = max(sigma1, -0.5*(gs/(fnew - f - gs)));

		/* Update the trust region bound according to the ratio of actual to predicted reduction. */
		if (actred < eta0*prered)
			delta = min(max(alpha, sigma1)*snorm, sigma2*delta);
		else if (actred < eta1*prered)
			delta = max(sigma1*delta, min(alpha*snorm, sigma2*delta));
		else if (actred < eta2*prered)
			delta = max(sigma1*delta, min(alpha*snorm, sigma3*delta));
		else
			delta = max(delta, min(alpha*snorm, sigma3*delta));

		info("iter %2d act %5.3e pre %5.3e delta %5.3e f %5.3e |g| %5.3e CG %3d\n", iter, actred, prered, delta, f, gnorm, cg_iter);

		if (actred > eta0*prered)
		{
			iter++;
			memcpy(w, w_new, sizeof(double)*n);
			f = fnew;
		    fun_obj->grad(w , g);
#ifdef BLAS
			gnorm = BLASCALL(dnrm2)(&n, g, &inc);
#else
			gnorm = dnrm2_(&n, g, &inc);

#endif
			if (gnorm <= eps*gnorm1)
				break;
		}
		if (f < -1.0e+32)
		{
			info("warning: f < -1.0e+32\n");
			break;
		}
		if (fabs(actred) <= 0 && prered <= 0)
		{
			info("warning: actred and prered <= 0\n");
			break;
		}
		if (fabs(actred) <= 1.0e-12*fabs(f) && fabs(prered) <= 1.0e-12*fabs(f))
		{
			info("warning: actred and prered too small\n");
			break;
		}
	}

	delete[] g;
	delete[] r;
	delete[] w_new;
	delete[] s;
}

int TRON::trcg(double delta, double *g, double *s, double *r)
{
	int i, inc = 1;
	int n = fun_obj->get_nr_variable();
	double one = 1;
	double *d = new double[n];
	double *Hd = new double[n];
	double rTr, rnewTrnew, alpha, beta, cgtol;

	for (i=0; i<n; i++)
	{
		s[i] = 0;
		r[i] = -g[i];
		d[i] = r[i];
	}
#ifdef BLAS
	cgtol = 0.1*BLASCALL(dnrm2)(&n, g, &inc);
#else
	cgtol = 0.1*dnrm2_(&n, g, &inc);

#endif
	int cg_iter = 0;
#ifdef BLAS
	rTr = BLASCALL(ddot)(&n, r, &inc, r, &inc);
#else
	rTr = ddot_(&n, r, &inc, r, &inc);
#endif

	while (1)
	{
#ifdef BLAS
		if (BLASCALL(dnrm2)(&n, r, &inc) <= cgtol)
#else
		if (dnrm2_(&n, r, &inc) <= cgtol)

#endif
			break;
		cg_iter++;
		fun_obj->Hv(d, Hd);
#ifdef BLAS
		alpha = rTr/BLASCALL(ddot)(&n, d, &inc, Hd, &inc);
		BLASCALL(daxpy)(&n, &alpha, d, &inc, s, &inc);
		if (BLASCALL(dnrm2)(&n, s, &inc) > delta)
#else
		alpha = rTr/ddot_(&n, d, &inc, Hd, &inc);
		daxpy_(&n, &alpha, d, &inc, s, &inc);
		if (dnrm2_(&n, s, &inc) > delta)

#endif
		{
			info("cg reaches trust region boundary\n");
			alpha = -alpha;
#ifdef BLAS
			BLASCALL(daxpy)(&n, &alpha, d, &inc, s, &inc);
			double std = BLASCALL(ddot)(&n, s, &inc, d, &inc);
			double sts = BLASCALL(ddot)(&n, s, &inc, s, &inc);
			double dtd = BLASCALL(ddot)(&n, d, &inc, d, &inc);
#else
			daxpy_(&n, &alpha, d, &inc, s, &inc);
			double std = ddot_(&n, s, &inc, d, &inc);
			double sts = ddot_(&n, s, &inc, s, &inc);
			double dtd = ddot_(&n, d, &inc, d, &inc);

#endif
			double dsq = delta*delta;
			double rad = sqrt(std*std + dtd*(dsq-sts));
			if (std >= 0)
				alpha = (dsq - sts)/(std + rad);
			else
				alpha = (rad - std)/dtd;
#ifdef BLAS
			BLASCALL(daxpy)(&n, &alpha, d, &inc, s, &inc);
			alpha = -alpha;
			BLASCALL(daxpy)(&n, &alpha, Hd, &inc, r, &inc);
#else
			daxpy_(&n, &alpha, d, &inc, s, &inc);
			alpha = -alpha;
			daxpy_(&n, &alpha, Hd, &inc, r, &inc);

#endif
			break;
		}
		alpha = -alpha;
#ifdef BLAS
		BLASCALL(daxpy)(&n, &alpha, Hd, &inc, r, &inc);
		rnewTrnew = BLASCALL(ddot)(&n, r, &inc, r, &inc);
		beta = rnewTrnew/rTr;
		BLASCALL(dscal)(&n, &beta, d, &inc);
		BLASCALL(daxpy)(&n, &one, r, &inc, d, &inc);
#else
		daxpy_(&n, &alpha, Hd, &inc, r, &inc);
		rnewTrnew = ddot_(&n, r, &inc, r, &inc);
		beta = rnewTrnew/rTr;
		dscal_(&n, &beta, d, &inc);
		daxpy_(&n, &one, r, &inc, d, &inc);
#endif
		rTr = rnewTrnew;
	}

	delete[] d;
	delete[] Hd;

	return(cg_iter);
}

double TRON::norm_inf(int n, double *x)
{
	double dmax = fabs(x[0]);
	for (int i=1; i<n; i++)
		if (fabs(x[i]) >= dmax)
			dmax = fabs(x[i]);
	return(dmax);
}

void TRON::set_print_string(void (*print_string) (const char *buf))
{
	tron_print_string = print_string;
}
