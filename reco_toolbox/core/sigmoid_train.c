/*


sigmoid_train : Fit paramaters of a sigmoïd.


Usage
-------

[A , B]     = sigmoid_train(y , f);

Inputs
-------

y             binary Labels  (1 x N)

f             Output of the classifier   (1 x N)


Ouputs
-------

A             Balanced Error Rate

B             Confusion Matrix (m x m), where m is the number of label, i.e. m = length(unique(ytest));


To compile
----------

mex -output sigmoid_train.dll sigmoid_train.c 


mex -f mexopts_intel10.bat -output sigmoid_train.dll sigmoid_train.c


Example 1
---------

N                    = 100;
y                    = double(rand(1 , N) > 0.5);
f                    = randn(1 , N);


[A , B]              = sigmoid_train(y , f);
pf                   = sigmoid_predict(f , A , B);
x                    = min(f):0.01:max(f);
px                   = sigmoid_predict(x , A , B);

plot(f , pf , '+' , x , px , 'r')



Author : Sébastien PARIS : sebastien.paris@lsis.org
-------

*/

#include <math.h>
#include "mex.h"


/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/

void sigmoid_train(double *, double *, int , double *  , double * , double * , double *);


void qs( double * , int , int  ); 


/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/




void mexFunction( int nlhs, mxArray *plhs[] , int nrhs, const mxArray *prhs[] )

{


	double *y  , *f;

	double *A , *B;


	double *t;

	int  i , j,  m=0 , im , N;

	int indytest, indytest_est, tempytest, tempytest_est;


	double *ysorted, *labels;

	double currentlabel , sum , temp;


	/*--------------------------------------------------------------------------------*/
	/*--------------------------------------------------------------------------------*/
	/* -------------------------- Parse INPUT  -------------------------------------- */
	/*--------------------------------------------------------------------------------*/	
	/*--------------------------------------------------------------------------------*/


	/* ----- Input 1 ----- */


	y               = mxGetPr(prhs[0]);


	if(mxGetM(prhs[0]) != 1)
	{

		mexErrMsgTxt("y must be (1 x N)");	

	}


	N              = mxGetN(prhs[0]);


	/* ----- Input 2 ----- */


	f              = mxGetPr(prhs[1]);


	if(mxGetN(prhs[1]) != N)
	{

		mexErrMsgTxt("f must be (1 x N)");	

	}



	/* Determine number of class and vector of labels*/


	ysorted    = mxMalloc(N*sizeof(double));


	for ( i = 0 ; i < N; i++ ) 
	{

		ysorted[i] = y[i];

	}


	qs( ysorted , 0 , N - 1 );


	labels       = mxMalloc(sizeof(double)); 

	labels[m]    = ysorted[0];

	currentlabel = labels[0];

	for (i = 0 ; i < N ; i++) 
	{ 
		if (currentlabel != ysorted[i]) 
		{ 
			labels       = (double *)mxRealloc(labels , (m+2)*sizeof(double)); 

			labels[++m]  = ysorted[i]; 

			currentlabel = ysorted[i];

		} 
	} 

	m++; 


	if (m != 2)

	{


		mexErrMsgTxt("Only for binary labels ");


	}


	/*--------------------------------------------------------------------------------*/
	/*---------------------------------------,----------------------------------------*/
	/* -------------------------- Parse OUTPUT  ------------------------------------- */
	/*--------------------------------------------------------------------------------*/
	/*--------------------------------------------------------------------------------*/


	t              = mxMalloc(N*sizeof(double));



	/* ----- output 1 ----- */


	plhs[0]        = mxCreateDoubleMatrix(1 , 1 , mxREAL);

	A              = mxGetPr(plhs[0]);


	/* ----- output 2 ----- */


	plhs[1]        = mxCreateDoubleMatrix(1 , 1 , mxREAL);

	B              = mxGetPr(plhs[1]);




	/*---------------------------------------------------------------------------------*/
	/*---------------------------------------------------------------------------------*/
	/* ----------------------- MAIN CALL  -------------------------------------------- */
	/*---------------------------------------------------------------------------------*/
	/*---------------------------------------------------------------------------------*/	
	/*---------------------------------------------------------------------------------*/



	sigmoid_train(f , y , N , A , B , t , labels);


	mxFree(t);

	mxFree(ysorted);

	mxFree(labels);



	/*-----------------------------------------------*/
	/*-----------------------------------------------*/
	/* ------------ END of Mex File ---------------- */
	/*-----------------------------------------------*/
	/*-----------------------------------------------*/

}

/*----------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------*/


void sigmoid_train(double *f, double *y , int N, double *A , double *B , double *t , double *labels)
{
	double prior1   = 0.0, prior0 = 0.0;


	int max_iter    = 100; 	// Maximal number of iterations
	double min_step = 1e-10;	// Minimal step taken in line search
	double sigma    = 1e-3;	// For numerically strict PD of Hessian
	double eps      = 1e-5;
	double hiTarget,loTarget,fApB,p,q,h11,h22,h21,g1,g2,det,dA,dB,gd,stepsize;
	double newA,newB,newf,d1,d2;
	int iter; 
	double fval = 0.0;


	int i;

	for (i = 0 ; i<N;i++)
	{
		if (y[i] == labels[1]) 
		{
			prior1   += 1;
		}
		else 
		{
			prior0  += 1;

		}
	}

	hiTarget = (prior1 + 1.0)/(prior1 + 2.0);

	loTarget = 1.0/(prior0 + 2.0);

	// Initial Point and Initial Fun Value

	A[0]   = 0.0; 

	B[0]   = log((prior0 + 1.0)/(prior1 + 1.0));


	for (i = 0 ; i<N ;i++)
	{
		if (y[i] == labels[1]) 
		{

			t[i] = hiTarget;
		}
		else 
		{

			t[i] = loTarget;
		}

		fApB = f[i]*A[0] + B[0];

		if (fApB>=0)
		{

			fval += t[i]*fApB + log(1.0 + exp(-fApB));
		}
		else
		{

			fval += (t[i] - 1.0)*fApB + log(1.0 + exp(fApB));
		}
	}

	for (iter = 0 ; iter < max_iter ; iter++)
	{
		// Update Gradient and Hessian (use H' = H + sigma I)

		h11 = sigma; // numerically ensures strict PD

		h22 = sigma;

		h21 = 0.0;

		g1  = 0.0;

		g2  = 0.0;

		for (i= 0 ; i<N ; i++)
		{

			fApB = f[i]*A[0] + B[0];

			if (fApB >= 0)
			{
				q = 1.0/(1.0 + exp(-fApB));

				p = exp(-fApB)*q;

			}
			else
			{

				p = 1.0/(1.0 + exp(fApB));

				q = exp(fApB)*p;
			}

			d2   = p*q;

			h11 += f[i]*f[i]*d2;

			h22 += d2;

			h21 += f[i]*d2;

			d1   = t[i] - p;

			g1  += f[i]*d1;

			g2  += d1;
		}

		// Stopping Criteria
		if ( fabs(g1)<eps && fabs(g2)<eps)
			break;

		// Finding Newton direction: -inv(H') * g

		det      = h11*h22 - h21*h21;

		dA       = -(h22*g1 - h21 * g2) / det;

		dB       = -(-h21*g1+ h11 * g2) / det;

		gd       = g1*dA + g2*dB;


		stepsize = 1.0; 		// Line Search

		while (stepsize >= min_step)
		{

			newA = A[0] + stepsize*dA;

			newB = B[0] + stepsize*dB;

			// New function value

			newf = 0.0;

			for (i = 0 ; i < N ; i++)
			{

				fApB = f[i]*newA+newB;

				if (fApB >= 0)
				{

					newf += t[i]*fApB + log(1.0 + exp(-fApB));
				}
				else
				{

					newf += (t[i] - 1.0)*fApB +log(1.0 + exp(fApB));
				}
			}
			// Check sufficient decrease

			if (newf < fval + 0.0001*stepsize*gd)
			{
				A[0]       = newA;

				B[0]       = newB;

				fval       = newf;

				break;
			}
			else
			{

				stepsize = stepsize * 0.5;

			}
		}

	}

}


void qs( double *array, int left, int right ) 
{

	double pivot;	// pivot element.

	int holex;	// hole index.

	int i;

	holex          = left + ( right - left ) / 2;

	pivot          = array[ holex ];		     // get pivot from middle of array.

	array[holex]   = array[ left ];              // move "hole" to beginning of

	holex          = left;			             // range we are sorting.

	for ( i = left + 1 ; i <= right ; i++ ) 
	{
		if ( array[ i ] <= pivot ) 
		{
			array[ holex ] = array[ i ];

			array[ i ]     = array[ ++holex ];
		}
	}

	if ( holex - left > 1 ) 
	{

		qs( array, left, holex - 1 );

	}
	if ( right - holex > 1 ) 
	{

		qs( array, holex + 1, right );

	}

	array[ holex ] = pivot;

}

