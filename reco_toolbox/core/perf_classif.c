/*


 perf_classif : Returns the Classification Rate  and the confusion matrix.
 

 Usage
 -------

 [R , mat_conf ] = perf_classif(ytest , ytest_est , [m]);

 Inputs
 -------

 ytest         True labels   (1 x Ntest)

 ytest_est     Estimated labels   (1 x Ntest)


 Ouputs
 -------

 R             Classification rate

 mat_conf      Confusion Matrix (m x m), where m is the number of label, i.e. m = length(unique(ytest));

	
 To compile
 ----------

mex -output perf_classif.dll perf_classif.c 


mex -f mexopts_intel10.bat -output perf_classif.dll perf_classif.c


Example 1
---------

Ntest                = 100;
ytest                = double(rand(1 , Ntest) > 0.5);
ytest_est            = double(rand(1 , Ntest) > 0.5);


[R , mat_conf]       = perf_classif(ytest , ytest_est);
 


  
Author : Sébastien PARIS : sebastien.paris@lsis.org
-------

*/

#include <limits.h>
#include <math.h>
#include "mex.h"


#define MAX_INF INT_MAX

/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/


void qs( double * , int , int  ); 


/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/




void mexFunction( int nlhs, mxArray *plhs[] , int nrhs, const mxArray *prhs[] )

{
	
	
	double *ytest  , *ytest_est;
	
	double *R , *mat_conf ;
		
	int  i , j,  m=0 , im , Ntest;

	int indytest, indytest_est, tempytest, tempytest_est;


	double *ytestsorted, *labels;

	double currentlabel , sum , temp;

	
	/*--------------------------------------------------------------------------------*/
	/*--------------------------------------------------------------------------------*/
	/* -------------------------- Parse INPUT  -------------------------------------- */
	/*--------------------------------------------------------------------------------*/	
	/*--------------------------------------------------------------------------------*/
	
		
	/* ----- Input 1 ----- */
	
	
	ytest            = mxGetPr(prhs[0]);


	if(mxGetM(prhs[0]) != 1)
	{

		 mexErrMsgTxt("ytest must be (1 x Ntest)");	

	}

	
	Ntest            = mxGetN(prhs[0]);
	

	/* ----- Input 2 ----- */

	
	ytest_est       = mxGetPr(prhs[1]);


	if(mxGetN(prhs[1]) != Ntest)
	{

		 mexErrMsgTxt("ytest_est must be (1 x Ntest)");	

	}


	
    /* Determine number of class and vector of labels*/

	if (nrhs < 3)
		
	{
		
		
		ytestsorted    = mxMalloc(Ntest*sizeof(double));
		
		
		for ( i = 0 ; i < Ntest; i++ ) 
		{
			
			ytestsorted[i] = ytest[i];
			
		}
		
		
		qs( ytestsorted , 0 , Ntest - 1 );
		
		
		labels       = mxMalloc(sizeof(double)); 
		
		labels[m]    = ytestsorted[0];
		
		currentlabel = labels[0];
		
		for (i = 0 ; i < Ntest ; i++) 
		{ 
			if (currentlabel != ytestsorted[i]) 
			{ 
				labels       = (double *)mxRealloc(labels , (m+2)*sizeof(double)); 
				
				labels[++m]  = ytestsorted[i]; 
				
				currentlabel = ytestsorted[i];
				
			} 
		} 
		
		m++; 
		
	}

	else

	{


		m        = (int) mxGetScalar(prhs[2]);


	}



	
	/*--------------------------------------------------------------------------------*/
	/*---------------------------------------,----------------------------------------*/
	/* -------------------------- Parse OUTPUT  ------------------------------------- */
	/*--------------------------------------------------------------------------------*/
	/*--------------------------------------------------------------------------------*/
	
	/* ----- output 1 ----- */
	
	
	plhs[0]        = mxCreateDoubleMatrix(1 , 1 , mxREAL);

	R              = mxGetPr(plhs[0]);



	/* ----- output 3 ----- */
	
	
	plhs[1]        = mxCreateDoubleMatrix(m , m , mxREAL);

	mat_conf       = mxGetPr(plhs[1]);

	
	
	/*---------------------------------------------------------------------------------*/
	/*---------------------------------------------------------------------------------*/
	/* ----------------------- MAIN CALL  -------------------------------------------- */
	/*---------------------------------------------------------------------------------*/
	/*---------------------------------------------------------------------------------*/	
	/*---------------------------------------------------------------------------------*/
	

	
	for(i = 0 ; i < Ntest ; i++)
		
	{
		indytest     = (int)ytest[i];
		
		indytest_est = (int)ytest_est[i];
		
		for (j = 0 ; j < m ; j++)
		{
			
			if (indytest == labels[j])
				
			{
				tempytest = j;
				
			}
			if (indytest_est == labels[j])
				
			{
				tempytest_est = j;
				
			}
			
			
		}
		
		mat_conf[tempytest_est + tempytest*m]++; 	
	}
	

	R[0] = 0.0;


	for (i = 0 ; i < m ; i++)
	{

		im    = i*m;

		temp  = mat_conf[i + im];

		R[0] += temp;

/*
		sum   = 0.0;

		for(j = 0 ; j < m ; j++)

		{

			sum += mat_conf[j + im];

		}


		if(sum != 0.0)
		{
			
			sum = 1.0/sum;
			
		}
		
		else
		{
			
			
			sum  = 1.0;
		}

		for(j = 0 ; j < m ; j++)

		{

			mat_conf[j + im] *=sum ;

		}
*/

	}

    R[0]   /= Ntest;


	if (nrhs < 3)
		
	{
		
		mxFree(labels);
		
		mxFree(ytestsorted);
		
	}
	
	
	/*-----------------------------------------------*/
	/*-----------------------------------------------*/
	/* ------------ END of Mex File ---------------- */
	/*-----------------------------------------------*/
	/*-----------------------------------------------*/
		
}

/*----------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------*/


void qs(double  *a , int lo, int hi)
{
/*
lo is the lower index, hi is the upper index
  of the region of array a that is to be sorted
*/
    int i=lo, j=hi;
    double x=a[(lo+hi)/2] , h;

    /*  partition */
    do
    {    
        while (a[i]<x) i++; 
        while (a[j]>x) j--;
        if (i<=j)
        {
            h        = a[i]; 
			a[i]     = a[j]; 
			a[j]     = h;
            i++; 
			j--;
        }
    }
	while (i<=j);

    /*  recursion */
    if (lo<j) qs(a , lo , j);
    if (i<hi) qs(a , i , hi);
}

