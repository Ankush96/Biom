/*

Train binary Linear SVM with Pegasos algorithm

MIN_w lambda/2 |w|^2 + 1/N SUM_i LOSS(w, X(:,i), y(i))
 where LOSS(w,x,y) = MAX(0, 1 - y w'x) is the hinge loss 


Usage
------

w = pegasos_train(X , y , [options] );


Inputs
-------

X                              Input data (d x N) in double format. 
y                              Binary label (1 x N) where y_i ={-1,1}, i=1,...,N
options 
        lambda                 Regularizer (default lambda = 1/N); 
        biasmult               Bias term (default biasmult = 1.0);
        nbite                  Number of iteration (default nbite = 20*N);
		seed                   Seed number 


Outputs
-------

w                              Model w((d + (biasmult != 0)) x 1);

To compile
----------


mex  -g -output pegasos_train.dll pegasos_train.c "C:\Program Files\MATLAB\R2009b\extern\lib\win32\microsoft\libmwblas.lib"

mex  -f mexopts_intel10.bat -output pegasos_train.dll pegasos_train.c

If compiled with BLAS option, linked with BLAS lib

mex  -DBLAS -f mexopts_intel10.bat -output pegasos_train.dll pegasos_train.c "C:\Program Files\MATLAB\R2009b\extern\lib\win32\microsoft\libmwblas.lib"

mex  -DBLAS -f mexopts_intel10.bat -output pegasos_train.dll pegasos_train.c "C:\Program Files\Intel\Compiler\11.1\065\mkl\ia32\lib\mkl_core.lib" "C:\Program Files\Intel\Compiler\11.1\065\mkl\ia32\lib\mkl_intel_c.lib" "C:\Program Files\Intel\Compiler\11.1\065\mkl\ia32\lib\mkl_intel_thread.lib" "C:\Program Files\Intel\Compiler\11.1\065\lib\ia32\libiomp5md.lib"


If compiled with OMP option, OMP support


mex  -DOMP -f mexopts_intel10.bat -output pegasos_train.dll pegasos_train.c "C:\Program Files\Intel\Compiler\11.1\065\mkl\ia32\lib\mkl_core.lib" "C:\Program Files\Intel\Compiler\11.1\065\mkl\ia32\lib\mkl_intel_c.lib" "C:\Program Files\Intel\Compiler\11.1\065\mkl\ia32\lib\mkl_intel_thread.lib" "C:\Program Files\Intel\Compiler\11.1\065\lib\ia32\libiomp5md.lib"

If compiled with BLAS & OMP options

mex  -DBLAS -DOMP -f mexopts_intel10.bat -output pegasos_train.dll pegasos_train.c "C:\Program Files\Intel\Compiler\11.1\065\mkl\ia32\lib\mkl_core.lib" "C:\Program Files\Intel\Compiler\11.1\065\mkl\ia32\lib\mkl_intel_c.lib" "C:\Program Files\Intel\Compiler\11.1\065\mkl\ia32\lib\mkl_intel_thread.lib" "C:\Program Files\Intel\Compiler\11.1\065\lib\ia32\libiomp5md.lib"


Example 1
---------
d                    = 10000;
N                    = 5000;
C                    = 1;

X                    = rand(d , N);
y                    = double(rand(1,N)>0.5);
y(y==0)              = -1;

options.lambda       = 1/(C*N);
options.biasmult     = 1;
options.nbite        = 30*N;
options.seed         = 1234543;


w                    = pegasos_train(X , y  , options);
b                    = w(d+1);
w                    = w(1:d);


fX                   = w'*X+b*ones(1,N);

plot(1:N,fX);




Author : Sébastien PARIS : sebastien.paris@lsis.org
-------  Date : 09/23/2010

References [1] S. Shalev-Shwartz, Y. Singer, and N. Srebro. "Pegasos: Primal estimated sub-GrAdient SOlver for SVM."
               In Proc. ICML, 2007.
           [2] http://www.vlfeat.org/

*/

#include <time.h>
#include <math.h>
#include <mex.h>

#ifdef OMP
 #include <omp.h>
#endif


#define PI 3.14159265358979323846
#define EPSILON_D 2.220446049250313e-16
#ifndef max
    #define max(a,b) (a >= b ? a : b)
    #define min(a,b) (a <= b ? a : b)
#endif


#if defined(__OS2__)  || defined(__WINDOWS__) || defined(WIN32) || defined(_MSC_VER)
#define BLASCALL(f) f
#else
#define BLASCALL(f) f ##_
#endif


#define SHR3   ( jsr ^= (jsr<<17), jsr ^= (jsr>>13), jsr ^= (jsr<<5) )
#define randint SHR3
#define rand() (0.5 + (signed)randint*2.328306e-10)

#ifdef __x86_64__
    typedef int UL;
#else
    typedef unsigned long UL;
#endif

static UL jsrseed = 31340134 , jsr;


struct opts
{
    double         lambda;
	double         biasmult;
	int            nbite;
	UL             seed;
};


/*-------------------------------------------------------------------------------------------------------------- */

/* Function prototypes */


extern double BLASCALL(ddot)(int *, double *, int *, double *, int *);
void randini(UL);
void pegasos_train(double * , double * , int  , int   , struct opts , double * );



/*-------------------------------------------------------------------------------------------------------------- */


#ifdef MATLAB_MEX_FILE
void mexFunction( int nlhs, mxArray *plhs[] , int nrhs, const mxArray *prhs[] )
{  
	double *X , *y;
	int    d , N;
	double *w;
	struct opts options = {0.01 , 1.0 , 100 , (UL)NULL};

	mxArray *mxtemp;
	double *tmp , temp;
	int tempint;
	UL templint;


	/* Input 1  */


	if( !mxIsDouble(prhs[0]) )
	{
		mexErrMsgTxt("X must be (d x N) in double format\n");
	}


	X           = (double *)mxGetData(prhs[0]);

	d           = mxGetM(prhs[0]);
	N           = mxGetN(prhs[0]);



	/* Input 2  */


	if( !mxIsDouble(prhs[1]) )
	{
		mexErrMsgTxt("y must be (1 x N) in double format\n");
	}

	y           = (double *)mxGetData(prhs[1]);

	if(mxGetN(prhs[1]) != N)
	{
		mexErrMsgTxt("y must be (1 x N) in double format\n");
	}


	/* Input 3  */


	if ((nrhs > 1) && !mxIsEmpty(prhs[2]) )
	{
		mxtemp                            = mxGetField(prhs[2] , 0 , "lambda");
		if(mxtemp != NULL)
		{

			tmp                           = mxGetPr(mxtemp);
			temp                          = tmp[0];

			if( (temp < 0.0) )
			{
				mexPrintf("lambda >= 0, force to 1/N\n");	
				options.lambda             = 1.0/N;
			}
			else
			{
				options.lambda             = temp;
			}
		}


		mxtemp                            = mxGetField(prhs[2] , 0 , "biasmult");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			temp                          = tmp[0];
			options.biasmult              = temp;
		}

		mxtemp                            = mxGetField(prhs[2] , 0 , "nbite");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			tempint                       = (int) tmp[0];
			if( (tempint < 1) )
			{

				mexPrintf("nbite > 0 , force to 20*N\n");	
				options.nbite             = 20*N;
			}
			else
			{
				options.nbite             = tempint;
			}
		}


		mxtemp                            = mxGetField(prhs[2] , 0 , "seed");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			templint                      = (UL) tmp[0];
			if( (templint < 1) )
			{
				mexPrintf("seed >= 1 , force to NULL (random seed)\n");	
				options.seed             = (UL)NULL;
			}
			else
			{
				options.seed             = templint;

			}
		}
	}


	/*----------------------- Outputs -------------------------------*/

	plhs[0]            =  mxCreateDoubleMatrix(d + (options.biasmult > 0) , 1, mxREAL);
	w                  =  mxGetPr(plhs[0]);

	/*------------------------ Main Call ----------------------------*/

	randini(options.seed);
	pegasos_train(X , y , d  ,  N , options , w );
}

#else


#endif

/*----------------------------------------------------------------------------------------------------------------------------------------- */
void pegasos_train(double *X , double *y , int d , int N  , struct opts options , double *w )
{
	int nbite = options.nbite;
	double lambda = options.lambda, sqrtlambda = sqrt(lambda) , biasmult = options.biasmult ;

	int iteration, i , k , kd , currentlabel;
	double eta, scale = 1.0 , acc , a , b , temp;

#ifdef BLAS
	int inc = 1;
#endif

	for (iteration = 0 ; iteration < nbite ; iteration ++) 
	{
		k              = (int) floor(N*rand());
		kd             = k*d;
		currentlabel   = y[k];

		/* project on the weight vector */

#ifdef BLAS
		acc            = BLASCALL(ddot)(&d , X+kd , &inc , w , &inc);
#else
		acc            = 0.0;
#ifdef OMP 
#pragma omp parallel for default(none) private(i) shared (w,X,kd,d) reduction(+:acc)
#endif
		for (i = 0 ; i < d ; i++)
		{
			acc       += w[i]*X[i+kd];
		}
#endif
		if (biasmult) 
		{
			acc       += biasmult*w[d] ;
		}

		acc           *= scale ;

		/* learning rate */
		eta           = 1.0 / ((iteration+1) * lambda) ;

		if (currentlabel * acc < 1.0) 
		{
			/* margin violated */
			a          = scale * (1.0 - eta * lambda);
			b          = currentlabel * eta ;

			acc        = 0.0 ;

#ifdef OMP 
#pragma omp parallel for default(none) private(i,temp) shared (a,b,X,w,kd,d) reduction(+:acc)
#endif
			for (i = 0 ; i < d ; i++) 
			{
				temp      = a*w[i]+b*X[i + kd] ;
				w[i]      = temp;
				acc      += (temp*temp) ;
			}
			if (biasmult) 
			{
				temp      = a*w[d] + b*biasmult ;
				w[d]      = temp;
				acc      += (temp*temp) ;
			}
			scale         = min((1.0 / (sqrtlambda * sqrt(acc + EPSILON_D))) , 1.0);
		} 
		else 
		{
			/* margin not violated */
			scale        *= (1.0 - eta * lambda) ;
		}
	}

// /*
#ifdef OMP 
#pragma omp parallel for default(none) private(i) shared (w,d,biasmult,scale)
#endif
// */
	/* denormalize representation */
	for (i = 0 ; i < d + (biasmult ? 1 : 0) ; i++) 
	{
		w[i]            *= scale ;
	}
}

/*----------------------------------------------------------------------------------------------------------------------------------------- */
void randini(UL seed)
{
	/* SHR3 Seed initialization */

	if(seed == (UL)NULL)
	{
		jsrseed  = (UL) time( NULL );
		jsr     ^= jsrseed;
	}
	else
	{
		jsr     = (UL)NULL;
		jsrseed = seed;
		jsr    ^= jsrseed;
	}
}
/*----------------------------------------------------------------------------------------------------------------------------------------- */
