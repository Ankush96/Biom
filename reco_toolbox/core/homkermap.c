
/*

Gamma-Homogeneous Feature Kernel Map

Usage
------

PSI = homkermap(X , [options] );


Inputs
-------

X                              Input data (d x N) in double format. 
options 
        n                      Appoximation order (n>0, default n = 1)
        L                      Sampling step (default L = 0.5);
        kerneltype             0 for intersection kernel, 1 for Jensen-shannon kernel, 2 for Chi2 kernel (default kerneltype = 0)
		gamma                  Gamma value (default gamma = 1)
		numsubdiv              Number of subdivisions (default numsubdiv = 8);
        minexponent            Minimum exponent value (default minexponent = -20)
        maxexponent            Maximum exponent value (default minexponent = 8)

Outputs
-------

PSI                            Feature map (d*(2n+1) x N) in double format

To compile
----------

mex  -g -output homkermap.dll homkermap.c
mex  -f mexopts_intel10.bat -output homkermap.dll homkermap.c

Example 1
---------


X                    = rand(10,100);

options.n            = 2;
options.L            = 0.5;
options.kerneltype   = 2;

PSI                  = homkermap(X , options );
PSI1                 = vl_homkermap(X , options.n , options.L , 'kchi2');



Example 2
---------


X                    = rand(10,100);

options.n            = 2;
options.L            = 0.5;
options.kerneltype   = 2;
options.gamma        = 1/2;

PSI                  = homkermap(X , options );




Author : Sébastien PARIS : sebastien.paris@lsis.org
-------  Date : 09/23/2010

References [1] A. Vedaldi and A. Zisserman, "Efficient Additive Kernels via Explicit Feature Maps", 
                in Proceedings of the IEEE Conf. on Computer Vision and Pattern Recognition (CVPR), 2010
           [2] http://www.vlfeat.org/

*/

#include <math.h>
#include <mex.h>

#define PI 3.14159265358979323846
#ifndef max
    #define max(a,b) (a >= b ? a : b)
    #define min(a,b) (a <= b ? a : b)
#endif

struct opts
{
	int            n;
	double         L;
	int            kerneltype;
	double         gamma;
	int            numsubdiv;
	int            minexponent;
	int            maxexponent;
};


/*-------------------------------------------------------------------------------------------------------------- */
/* Function prototypes */

void homkermap(double * , int  , int   , struct opts , double * );

/*-------------------------------------------------------------------------------------------------------------- */
#ifdef MATLAB_MEX_FILE
void mexFunction( int nlhs, mxArray *plhs[] , int nrhs, const mxArray *prhs[] )
{  
	double *X;
	int    d , N;
	double *PSI;
	struct opts options = {1 , 0.5, 0 , 1 , 8 , -20 , 8};
	mxArray *mxtemp;
	double *tmp , temp;
	int tempint;

	/* Input 1  */

	if( !mxIsDouble(prhs[0]) )
	{
		mexErrMsgTxt("X must be (d x N) in double format\n");
	}

	X                                     = (double *)mxGetData(prhs[0]);
	d                                     = mxGetM(prhs[0]);
	N                                     = mxGetN(prhs[0]);

	/* Input 2  */

	if ((nrhs > 1) && !mxIsEmpty(prhs[1]) )
	{
		mxtemp                            = mxGetField(prhs[1] , 0 , "n");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			tempint                       = (int) tmp[0];
			if( (tempint < 1) )
			{
				mexPrintf("n > 0, force to 1\n");	
				options.n                 = 1;
			}
			else
			{
				options.n                 = tempint;
			}
		}

		mxtemp                            = mxGetField(prhs[1] , 0 , "L");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			temp                          = tmp[0];

			if( (temp < 0.0) )
			{
				mexPrintf("L >= 0, force to 0.5\n");	
				options.L                 = 0.5;
			}
			else
			{
				options.L                 = temp;
			}
		}

		mxtemp                            = mxGetField(prhs[1] , 0 , "kerneltype");
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

		mxtemp                            = mxGetField(prhs[1] , 0 , "gamma");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			temp                          = tmp[0];
			options.gamma                 = temp;
		}

		mxtemp                            = mxGetField(prhs[1] , 0 , "numsubdiv");
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

		mxtemp                            = mxGetField(prhs[1] , 0 , "minexponent");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			options.minexponent           = (int) tmp[0];
		}

		mxtemp                            = mxGetField(prhs[1] , 0 , "maxexponent");
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

	plhs[0]                               =  mxCreateDoubleMatrix(d*(options.n*2+1) , N , mxREAL);
	PSI                                   =  mxGetPr(plhs[0]);

	/*------------------------ Main Call ----------------------------*/
	
	homkermap(X , d  ,  N , options , PSI );
	
	/*--------------------------- Free memory -----------------------*/
}

#else


#endif

/*----------------------------------------------------------------------------------------------------------------------------------------- */
void homkermap(double *X , int d , int N  , struct opts options , double *PSI )
{
	int n = options.n, kerneltype = options.kerneltype, numsubdiv = options.numsubdiv, minexponent = options.minexponent, maxexponent = options.maxexponent;
	double L = options.L , subdiv = 1.0 / numsubdiv;
	int n1 = (2*n + 1) , D = d*n1, numsubdivn1 = numsubdiv*n1, jD , jd , in1 , diffexponent = (maxexponent - minexponent + 1) , exponent;
	unsigned int i,j,l , co=0 , v1 , v2;
	double x, logx, Lx, sqrtLx, Llogx, lambda ;
	double kappa, kappa0 , sqrtkappa0, sqrt2kappa ;
	double mantissa  , f1 , f2;
	double *table;

	/* table initialization */

	table            = (double *)malloc(n1*diffexponent*numsubdiv*sizeof(double));

	if (kerneltype == 0)
	{
		kappa0          = 2.0/PI;
		sqrtkappa0      = sqrt(kappa0) ;
	}
	else if (kerneltype == 1)
	{
		kappa0          = 2.0/log(4.0);
		sqrtkappa0      = sqrt(kappa0) ;
	}
	else if (kerneltype == 2)
	{
		sqrtkappa0      = 1.0 ;
	}

	for (exponent  = minexponent ; exponent <= maxexponent ; ++exponent) 
	{
		mantissa        = 1.0;
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
		/* Evaluate for each X(i,j) */

	for (j = 0 ; j < N ; j++)
	{
		jD   = j*D;
		jd   = j*d;
		in1  = jD;

		for (i = 0 ; i < d ; i++)
		{
			mantissa  = frexp(X[i + jd] , &exponent) ;
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
					f1           = table[l + v1];
					f2           = table[l + v2];
					PSI[l + in1] = ((f2 - f1) * (numsubdiv * mantissa) + f1) ;
				}
				in1 += n1;
			}
		}
	}
	free(table);
}
/*----------------------------------------------------------------------------------------------------------------------------------------- */
