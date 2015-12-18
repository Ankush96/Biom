// pwl_sgd.cc alex berg march 2009

// mex -f mexopts_intel10.bat -output encoder.dll encoder.c

// mex -g -output encoder.dll encoder.c

/*


clear

mu1                    = [0  0];
sigma1                 = [1 0; 0 1];
sigma2                 = [2 0; 0 2];
npts                   = 5000;
options.ndivs          = 20;
options.use_sqrt       = 1;

Xtrain                 = single(mvnrnd(mu1,sigma1,npts)');
Xtest                  = single(mvnrnd(mu1,sigma1,npts)');

[Xtraine , seg_data]   = encoder(Xtrain , options);
Xteste                 = encoder(Xtest , options , seg_data);







*/

#include <math.h>
#include <stdio.h>
#include "mex.h"


#ifndef max
    #define max(a,b) (a >= b ? a : b)
    #define min(a,b) (a <= b ? a : b)
#endif

#define bigpos 10e30
#define bigneg -10e30




struct opts
{
	int ndivs ;

	int use_sqrt;
};


int  segmentation(float * , int  , int  , int  , float *);

void mexFunction (int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{

	float *X;

	float *seg_data ,*tmpf;
	double *Xe;
	double *tmp ;

	int *ir , *jc;

	struct opts options = {20 , 1};

	int d , N , tot_dims , nzmax ;
	int count = 0, i , dsofar , j , dN , ind , index , oldnzmax , tempint , use_sqrt , lind , uind , nsteps;

	mxArray *mxtemp;

	float min_val , max_val , step   , msv , lval,uval,offset, omoffset;
	double percent_sparse = 0.1;

	if( (mxGetNumberOfDimensions(prhs[0]) > 2) || !mxIsSingle(prhs[0]) )
	{
		mexErrMsgTxt("X must be (d x N) in float (single) format");
	}


	X                                     = (float *)mxGetData(prhs[0]);

	d                                     = mxGetM(prhs[0]);
	N                                     = mxGetN(prhs[0]);

	dN                                    = d*N;


	if ((nrhs > 1) && !mxIsEmpty(prhs[1]) )
	{

		mxtemp                            = mxGetField(prhs[1] , 0 , "ndivs");

		if(mxtemp != NULL)
		{

			tmp                           = mxGetPr(mxtemp);
			tempint                       = (int) tmp[0];

			if( (tempint < 1))
			{

				mexErrMsgTxt("ndivs > 0, force to 20");	

				options.ndivs             = 20;

			}
			else
			{
				options.ndivs            = tempint;
			}
		}

		mxtemp                            = mxGetField(prhs[1] , 0 , "usesqrt");

		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			tempint                       = (int) tmp[0];

			if( (tempint < 0))
			{

				mexErrMsgTxt("usesqrt > 0, force to 0");	

				options.use_sqrt         = 0;

			}
			else
			{
				options.use_sqrt         = tempint;
			}
		}
	}


	if ((nrhs > 2) && !mxIsEmpty(prhs[2]) )
	{

			plhs[1]                      = mxCreateNumericMatrix(4 , d , mxSINGLE_CLASS,mxREAL);
			seg_data                     = (float *)mxGetPr(plhs[1]);

			tmpf                         = (float *)mxGetPr(prhs[2]);
			for(i = 0 ; i < 4*d ; i++)
			{
				seg_data[i]              = tmpf[i];
			}

			tot_dims                     = 0;

			for(i = 0 ; i < d ; i++)
			{
				tot_dims                += seg_data[3 + 4*i];

			}
	}


	else
	{

		plhs[1]                          = mxCreateNumericMatrix(4 , d , mxSINGLE_CLASS,mxREAL);
		seg_data                         = (float *)mxGetPr(plhs[1]);

		tot_dims                         = segmentation(X , d , N , options.ndivs , seg_data);
	}

	use_sqrt                             = options.use_sqrt;

	nzmax                                =  2*dN;


	plhs[0]                              = mxCreateSparse(tot_dims , N , nzmax , mxREAL);
	Xe                                   = mxGetPr(plhs[0]); 
	ir                                   = mxGetIr(plhs[0]);  // (nzmax x 1) index vector
	jc                                   = mxGetJc(plhs[0]);




	ind        = 0;
	for (i = 0 ; i< N; i++)
	{
		index  = 0;
		dsofar = 0;

		for (j=0 ; j < d; j++)
		{
			min_val    = seg_data[0 + index];
			max_val    = seg_data[1 + index];
			step       = seg_data[2 + index];
			nsteps     = (int) seg_data[3 + index];

			msv        = (X[j + ind ] - min_val);

			if (msv<=0)
			{  
				// take case of lower end of range
				lind     = 0;
				uind     = 1;
				lval     = 1.0;
				uval     = 0.0;
			}
			else
			{
				lind = (int) (floor(msv/step));

				if (lind>=(nsteps-1))
				{
					lind = nsteps-2;
					uind = nsteps-1;
					lval = 0.0;
					uval = 1.0;
				}
				else
				{
					offset = (msv - (lind*step))/step;

					if (offset < 0)
					{
						offset = 0.0;
					}
					else if (offset>1)
					{
						offset = 1.0;
					}

					omoffset = 1.0 - offset;
					if (omoffset < 0)
					{
						omoffset = 0.0;
					}
					else if (omoffset > 1)
					{
						omoffset = 1.0;
					}
					uind = lind + 1;
					lval = omoffset;
					uval = offset;
				}
			}
			if (count>=nzmax)
			{
				oldnzmax        = nzmax;		
				percent_sparse += 0.1;
				nzmax           = (int)ceil(dN*percent_sparse);

				if (oldnzmax == nzmax)
				{
					nzmax++;
				}

				mxSetNzmax(plhs[0] , nzmax); 
				mxSetPr(plhs[0] , mxRealloc(Xe , nzmax*sizeof(double)));
				mxSetIr(plhs[0] , mxRealloc(ir , nzmax*sizeof(int)));

				Xe                = mxGetPr(plhs[0]);
				ir                = mxGetIr(plhs[0]);

			}

			ir[count]    = dsofar + lind;

			if (use_sqrt > 0)
			{
				Xe[count] = lval*sqrt(step);
			}
			else
			{
				Xe[count] = lval;
			}
			count++;

			if (count>=nzmax)
			{
				oldnzmax        = nzmax;		
				percent_sparse += 0.1;
				nzmax           = (int)ceil(dN*percent_sparse);

				if (oldnzmax == nzmax)
				{
					nzmax++;
				}

				mxSetNzmax(plhs[0] , nzmax); 
				mxSetPr(plhs[0] , mxRealloc(Xe , nzmax*sizeof(double)));
				mxSetIr(plhs[0] , mxRealloc(ir , nzmax*sizeof(int)));

				Xe                = mxGetPr(plhs[0]);
				ir                = mxGetIr(plhs[0]);

			}

			ir[count]     = dsofar + uind;

			if (use_sqrt > 0)
			{
				Xe[count] = uval*sqrt(step);
			}
			else
			{
				Xe[count] = uval;
			}
			count++;
			dsofar    += nsteps;
			index     += 4;
		} 

		jc[i + 1] = count;	
		ind      += d;

	}
}


/*----------------------------------------------------------------------------------------------------------------------------------------- */

int segmentation(float *X , int d , int N , int ndivs , float *seg_data)

{
	int i , j , index , tot_dims = 0;
	float mini , maxi , step , tmpx , invndivs = 1.0/(float) ndivs;

	for (i = 0 ; i < d ; i++)
	{
		mini                = bigpos;
		maxi                = bigneg;
		index               = 4*i;

		for (j = 0 ; j < N ; j++)
		{
			tmpx            = X[i + j*d];
			mini            = min(mini , tmpx);
			maxi            = max(maxi , tmpx);
		}

		step                = (maxi - mini)*invndivs;

		tot_dims           += ndivs;

		seg_data[0 + index] = mini;
		seg_data[1 + index] = maxi;
		seg_data[2 + index] = step;
		seg_data[3 + index] = ndivs;
	}

	return tot_dims;
}

/*----------------------------------------------------------------------------------------------------------------------------------------- */

