/*

Train binary SVM for Homeogeneous additive kernel with Pegasos algorithm

MIN_w lambda/2 |w|^2 + 1/N SUM_i LOSS(w, X(:,i), y(i))
 where LOSS(w,x,y) = MAX(0, 1 - y w'x) is the hinge loss 


Usage
------

w = homker_pegasos_train(X , y , [options] );


Inputs
-------

X                              Input data (d x N) in double format. 
y                              Binary label (1 x N) where y_i ={-1,1}, i=1,...,N
options 
        lambda                 Regularizer (default lambda = 1/N); 
        biasmult               Bias term (default biasmult = 1.0);
        nbite                  Number of iteration (default nbite = 20*N);
        n                      Approximation order (n>0, default n = 0)
        L                      Sampling step (default L = 0.5);
        kerneltype             0 for intersection kernel, 1 for Jensen-shannon kernel, 2 for Chi2 kernel (default kerneltype = 0)
		numsubdiv              Number of subdivisions (default numsubdiv = 8);
        minexponent            Minimum exponent value (default minexponent = -20)
        maxexponent            Maximum exponent value (default minexponent = 8)
		seed                   seed value for uniform generator (default seed = 31340134)




Outputs
-------

w                              Model w(((2*n+1)*d + (biasmult != 0)) x 1);

To compile
----------


mex  -g -output homker_pegasos_train.dll homker_pegasos_train.c 
mex  -f mexopts_intel10.bat -output homker_pegasos_train.dll homker_pegasos_train.c
mex  -DBLAS -f mexopts_intel10.bat -output homker_pegasos_train.dll homker_pegasos_train.c "C:\Program Files\MATLAB\R2009b\extern\lib\win32\microsoft\libmwblas.lib"

mex  -DOMP -f mexopts_intel10.bat -output homker_pegasos_train.dll homker_pegasos_train.c "C:\Program Files\MATLAB\R2009b\extern\lib\win32\microsoft\libmwblas.lib"
mex  -DOMP -f mexopts_intel10.bat -output homker_pegasos_train.dll homker_pegasos_train.c "C:\Program Files\Intel\Compiler\11.1\065\mkl\ia32\lib\mkl_core.lib" "C:\Program Files\Intel\Compiler\11.1\065\mkl\ia32\lib\mkl_intel_c.lib" "C:\Program Files\Intel\Compiler\11.1\065\mkl\ia32\lib\mkl_intel_thread.lib" "C:\Program Files\Intel\Compiler\11.1\065\lib\ia32\libiomp5md.lib"

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

options.n            = 1;
options.L            = 0.5;
options.kerneltype   = 2;
options.seed         = 31340134;


w                    = homker_pegasos_train(X , y  , options);
fX                   = homker_predict(X , w , options);

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
	int            n;
	double         L;
	int            kerneltype;
	int            numsubdiv;
	int            minexponent;
	int            maxexponent;
};

/*-------------------------------------------------------------------------------------------------------------- */

/* Function prototypes */

extern double BLASCALL(ddot)(int *, double *, int *, double *, int *);
void randini(UL);
void honkertable(int , struct opts , double *);
void honkerproj(double* , double* , int  , struct opts , double *);
void homker_pegasos_train(double * , double * , int  , int   , struct opts , double * );

/*-------------------------------------------------------------------------------------------------------------- */


#ifdef MATLAB_MEX_FILE
void mexFunction( int nlhs, mxArray *plhs[] , int nrhs, const mxArray *prhs[] )
{  

	double *X , *y;
	int    d , N;
	double *w;
	struct opts options = {0.01 , 1.0 , 100 , (UL)NULL , 0 , 0.5 , 2 , 8 , -20 ,8};
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
	if ((nrhs > 2) && !mxIsEmpty(prhs[2]) )
	{
		mxtemp                            = mxGetField(prhs[2] , 0 , "lambda");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			temp                          = tmp[0];
			if( (temp < 0.0) )
			{
				mexPrintf("lambda >= 0, force to 1/N\n");	
				options.lambda            = 1.0/N;
			}
			else
			{
				options.lambda            = temp;
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

		mxtemp                            = mxGetField(prhs[2] , 0 , "n");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			tempint                       = (int) tmp[0];
			if( (tempint < 0) )
			{
				mexPrintf("n >= 0, force to 0\n");	
				options.n                 = 0;
			}
			else
			{
				options.n                 = tempint;
			}
		}

		mxtemp                            = mxGetField(prhs[2] , 0 , "L");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			temp                          = tmp[0];
			if( (temp < 0.0) )
			{
				mexPrintf("L >= 0, force to 0.5\n");	
				options.L             = 0.5;
			}
			else
			{
				options.L             = temp;
			}
		}

		mxtemp                            = mxGetField(prhs[2] , 0 , "kerneltype");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			tempint                       = (int) tmp[0];
			if( (tempint < 0) ||  (tempint > 2))
			{
				mexPrintf("kerneltype = {0,1,2}, force to 0\n");	
				options.kerneltype        = 1;
			}
			else
			{
				options.kerneltype        = tempint;
			}
		}

		mxtemp                            = mxGetField(prhs[2] , 0 , "numsubdiv");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			tempint                       = (int) tmp[0];
			if( (tempint < 1) )
			{
				mexPrintf("numsubdiv > 0 , force to 8\n");	
				options.numsubdiv         = 8;
			}
			else
			{
				options.numsubdiv         = tempint;
			}
		}

		mxtemp                            = mxGetField(prhs[2] , 0 , "minexponent");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			options.minexponent            = (int) tmp[0];
		}

		mxtemp                            = mxGetField(prhs[2] , 0 , "maxexponent");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			tempint                       = (int) tmp[0];
			if( (tempint < options.minexponent) )
			{
				mexPrintf("maxexponent > minexponent , force to 8\n");	
				options.maxexponent       = options.minexponent + 2;
			}
			else
			{
				options.maxexponent       = tempint;
			}
		}
	}

	/*----------------------- Outputs -------------------------------*/

	plhs[0]            =  mxCreateDoubleMatrix(d*(options.n*2+1) + (options.biasmult > 0) , 1, mxREAL);
	w                  =  mxGetPr(plhs[0]);

	/*------------------------ Main Call ----------------------------*/

	randini(options.seed);
	homker_pegasos_train(X , y , d ,  N , options , w );

}

#else


#endif

/*----------------------------------------------------------------------------------------------------------------------------------------- */
void homker_pegasos_train(double *X , double *y , int d , int N  , struct opts options , double *w )
{
	int nbite = options.nbite;
	double lambda = options.lambda, sqrtlambda = sqrt(lambda) , biasmult = options.biasmult ;
	int n = options.n , n1 = (2*n + 1) , diffexponent = (options.maxexponent - options.minexponent + 1) , numsubdiv = options.numsubdiv;
	int dd = d*n1 , currentlabel;
	int iteration, i , k , kd ;	
	double eta, scale = 1.0 , acc , a , b , temp;
	double *table , *PSI , *ptX;

	if(n > 0)
	{
		table            = (double *)malloc(n1*diffexponent*numsubdiv*sizeof(double));
		PSI              = (double *)malloc(d*n1*sizeof(double));
		honkertable(d , options , table);
		ptX              = PSI;
	}

	for (iteration = 0 ; iteration < nbite ; iteration ++) 
	{
		k              = (unsigned int) floor(N*rand());
		kd             = k*d;
		currentlabel   = y[k];
		if(n > 0)
		{
			honkerproj(X + kd , table , d , options , PSI);
		}
		else
		{
          ptX          = (X+kd);
		}

		/* project on the weight vector */


#ifdef BLAS
		acc            = BLASCALL(ddot)(&dd , ptX , &inc , w , &inc);

#else
		acc            = 0.0;

#ifdef OMP 
#pragma omp parallel for default(none) private(i) shared (w,ptX,dd) reduction (+:acc)
#endif
		for (i = 0 ; i < dd ; i++)
		{
			acc       += w[i]*ptX[i];
		}
#endif

		if (biasmult) 
		{
			acc       += biasmult * w[dd] ;
		}

		acc *= scale ;

		/* learning rate */
		eta = 1.0 / ((iteration+1) * lambda) ;

		if (currentlabel * acc < 1.0) 
		{
			/* margin violated */
			a   = scale * (1.0 - eta * lambda);
			b   = currentlabel * eta ;

			acc = 0.0;
			#ifdef OMP 
			#pragma omp parallel for default(none) private(i,temp) shared (w,ptX,dd,a,b) reduction (+:acc)
			#endif
			for (i = 0 ; i < dd ; i++) 
			{
				temp      = a*w[i] + b*ptX[i] ;
				w[i]      = temp;
				acc      += temp*temp ;
			}

			if (biasmult) 
			{
				temp      = a*w[dd] + b*biasmult ;
				w[dd]     = temp;
				acc      += temp*temp ;
			}
			scale       = min((1.0 / (sqrtlambda * sqrt(acc + EPSILON_D))) , 1.0);
		} 
		else 
		{
			/* margin not violated */
			scale      *= (1.0 - eta * lambda) ;
		}
	}

#ifdef OMP 
#pragma omp parallel for default(none) private(i) shared (w,dd,biasmult,scale)
#endif
	/* denormalize representation */
	for (i = 0 ; i < dd + (biasmult ? 1 : 0) ; i++) 
	{
		w[i] *= scale ;
	}
	if(n > 0)
	{
		free(table);
		free(PSI);
	}
}

/*----------------------------------------------------------------------------------------------------------------------------------------- */
void honkertable(int d , struct opts options, double *table)
{
	int n = options.n, kerneltype = options.kerneltype, numsubdiv = options.numsubdiv, minexponent = options.minexponent, maxexponent = options.maxexponent;
	double L = options.L , subdiv = 1.0 / numsubdiv;
	int exponent;
	unsigned int i,j , co=0;
	double x, logx, Lx, sqrtLx, Llogx, lambda ;
	double kappa, kappa0 , sqrtkappa0, sqrt2kappa ,  mantissa ;
	if (kerneltype == 0)
	{
		kappa0       = 2.0/PI;
		sqrtkappa0   = sqrt(kappa0) ;
	}
	else if (kerneltype == 1)
	{
		kappa0       = 2.0/log(4.0);
		sqrtkappa0   = sqrt(kappa0) ;
	}
	else if (kerneltype == 2)
	{
		sqrtkappa0   = 1.0 ;
	}

	for (exponent  = minexponent ; exponent <= maxexponent ; ++exponent) 
	{
		mantissa = 1.0;
		for (i = 0 ; i < numsubdiv ; ++i , mantissa += subdiv) 
		{
			x           = ldexp(mantissa, exponent);
			Lx          = L * x ;
			logx        = log(x);
			sqrtLx      = sqrt(Lx);
			Llogx       = L*logx;

			table[co++] = (sqrtkappa0 * sqrtLx) ;

			for (j = 1 ; j <= n ; ++j) 
			{
				lambda = j * L;

				if (kerneltype == 0)
				{
					kappa   = kappa0 / (1.0 + 4.0*lambda*lambda) ;
				}
				else if (kerneltype == 1)
				{
					kappa   = kappa0 * 2.0 / (exp(PI * lambda) + exp(-PI * lambda)) / (1.0 + 4.0*lambda*lambda) ;
				}
				else if (kerneltype == 2)
				{
					kappa   = 2.0 / (exp(PI * lambda) + exp(-PI * lambda)) ;
				}
				sqrt2kappa  = sqrt(2.0 * kappa)* sqrtLx ;
				table[co++] = (sqrt2kappa * cos(j * Llogx)) ;
				table[co++] = (sqrt2kappa * sin(j * Llogx)) ;
			}
		}
	}
}
/*----------------------------------------------------------------------------------------------------------------------------------------- */
void honkerproj(double *X , double *table , int d  , struct opts options , double *PSI)
{
	int n = options.n, numsubdiv = options.numsubdiv, minexponent = options.minexponent, maxexponent = options.maxexponent;
	double subdiv = 1.0 / numsubdiv;
	int  n1 = (2*n + 1) , in1 = 0 , numsubdivn1 = numsubdiv*n1  , exponent;
	unsigned int i,l  , v1 , v2;
	double mantissa  , f1 , f2;

	for (i = 0 ; i < d ; i++)
	{
		mantissa  = frexp(X[i] , &exponent) ;
		mantissa *= 2 ;
		exponent -- ;

		if (mantissa == 0 || exponent <= minexponent || exponent >= maxexponent) 
		{
			for (l = in1 ; l <= (n+in1) ; ++l) 
			{
				PSI[l] =  0.0 ;
			}
			in1 += n1;
		}
		else
		{
			v1            = (exponent - minexponent) * numsubdivn1;
			mantissa     -= 1.0 ;
			while (mantissa >= subdiv) 
			{
				mantissa -= subdiv ;
				v1       += n1 ;
			}
			v2            = v1 + n1 ;
			for (l = 0 ; l < n1 ; ++l) 
			{
				f1             = table[l + v1];
				f2             = table[l + v2];
				PSI[l + in1]   = ((f2 - f1) * (numsubdiv * mantissa) + f1) ;
			}
			in1 += n1;
		}
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
