
/*

Histogram of color visual words with a Multi-Level Spatial pyramid.

Usage
------

H = mlhbow_spyr(D , patches , infopatches , [options] );


Inputs
-------

D                              Dictionaries of K Visual Words (d x K x nscale x dimcolor) in single format.

patches                        Patched extracted (d x npatches) from image I (ny x nx) 

infopatches                    Informations associated with each patch (4 x npatches).
                               infopatches(1,i) = yi
                               infopatches(2,i) = xi
							   infopatches(3,i) = scalei
							   infopatches(4,:) = dimcolori
							   infopatches(5,:) = nyscale
							   infopatches(6,:) = nxscale


options 
	   scale                   Scaling vector (1 x nscale). Extract descriptors at different scaling of the image (default scale = [1]).

       spyr                    Spatial Pyramid (nspyr x 4) (default [1 , 1 , 1 , 1] with nspyr = 1)
                               where spyr(i,1) is the ratio of ny in y axis of the blocks at level i (by(i) = spyr(i,1)*ny)
                               where spyr(i,2) is the ratio of nx in x axis of the blocks at level i (bx(i) = spyr(i,3)*nx)
                               where spyr(i,3) is the ratio of ny in y axis of the shifting at level i (deltay(i) = spyr(i,2)*ny)
                               where spyr(i,4) is the ratio of nx in x axis of the shifting at level i (deltax(i) = spyr(i,4)*nx)

	   assignment              Hard or Soft assignment (0/1). (default assignment = 0)


	   beta                    Soft quantification parameter such G(i,j) = exp(-beta*||D(:,i)-patches(:,j)||_2^2)/sum(j)(exp(-beta*||D(:,i)-patches(:,j)||_2^2))

	   

 	   norm                    Normalization vector (1 x 3) : [for all subwindows, for each subwindows of a pyramid level, for each subwindows]
                               norm = 0 <=> no normalization, norm = 1 <=> v=v/(sum(v)+epsi), norm = 2 <=> v=v/sqrt(sum(v²)+epsi²),
	                           norm = 3 <=> v=sqrt(v/(sum(v)+epsi)) , norm = 4 <=> L2-clamped (default norm = [0 , 0 , 4])

       clamp                   Clamping value (default clamp = 0.2)


Outputs
-------

H                              mlhbow_spyr features (K*nH*nscale*dimcolor x 1) in double format  and nH is the total number of subwindows defined
                               by the spatial pyramid spyr.

To compile
----------


mex  -g -output mlhbow_spyr.dll mlhbow_spyr.c

mex  -f mexopts_intel10.bat -output mlhbow_spyr.dll mlhbow_spyr.c


Example 1
---------


d                     = 128;
K                     = 1024;
nscale                = 2;
ncolor                = 1;
npatches              = 5000;
ny                    = 200;
nx                    = 260;

options.scale         = [0.75 , 1];
options.spyr          = [1 , 1 , 1 , 1 ; 1/2 , 1/2 , 1/4 , 1/4];
options.beta          = 1;
options.assignment    = 1;
options.norm          = [0 , 0 , 4];
options.clamp         = 0.2;



D                     = rand(d , K , nscale , ncolor , 'single');
patches               = rand(d , npatches , 'single');
infopatches           = [4+(ny-4)*rand(1,npatches , 'single') ; 4+(nx-4)*rand(1,npatches , 'single') ; [0.75*ones(1 , npatches/2 , 'single') , ones(1 , npatches/2 , 'single')] ; ones(1 , npatches , 'single') ; ny*ones(1 , npatches , 'single') ; nx*ones(1 , npatches , 'single')];


H                     = mlhbow_spyr(D , patches , infopatches , options );

figure(1)
plot(1:length(H) , H)



Author : Sébastien PARIS : sebastien.paris@lsis.org
-------  Date : 09/06/2010



Reference : 
---------       

Changelog : - add extra option for normalization
----------


*/

#include <math.h>
#include <mex.h>

#define tiny 1e-8
#define verytiny 1e-15

#ifndef max
#define max(a,b) (a >= b ? a : b)
#define min(a,b) (a <= b ? a : b)
#endif


struct opts
{
	double  *scale;
	int     nscale;
	double  *spyr;
    double  beta;
	int     nspyr;
	int     assignment;
	double *norm;
	double  clamp;
};


/*-------------------------------------------------------------------------------------------------------------- */

/* Function prototypes */

int	number_histo_spyr(double * , int );
void mlhbow_spyr(float * , float * , float *  , struct opts , double *, int , int  , int , int , int , int);

/*-------------------------------------------------------------------------------------------------------------- */
#ifdef MATLAB_MEX_FILE
void mexFunction( int nlhs, mxArray *plhs[] , int nrhs, const mxArray *prhs[] )
{  
	float *D , *patches , *infopatches;
	const int *dimD;
	double *H;
	int i , d , K , nH = 1 , npatches = 1 , nscale = 1 , ncolor = 1;
	double	scale_default[1]    = {1};
	double  norm_default[3]     = {0 , 0 , 4};
	struct opts options;
	mxArray *mxtemp;
	double *tmp , temp;
	int tempint;

	options.nspyr         = 1;
	options.beta          = 0.1;
	options.assignment    = 0;
	options.clamp         = 0.2;


	/* Input 1  */


	if( (mxGetNumberOfDimensions(prhs[0]) > 4) || !mxIsSingle(prhs[0]) )
	{
		mexErrMsgTxt("D must be (d x K x nscale x ncolor) in SINGLE format");
	}


	D          = (float *)mxGetData(prhs[0]);
	dimD       = mxGetDimensions(prhs[0]);

	d          = dimD[0];
	K          = dimD[1];

	if(mxGetNumberOfDimensions(prhs[0]) > 2)
	{
		nscale   = dimD[2];
	}

	if(mxGetNumberOfDimensions(prhs[0]) > 3)
	{
		ncolor   = dimD[3];
	}

	/* Input 2  */

	patches      = (float *)mxGetData(prhs[1]);

	if( (mxGetM(prhs[1]) != d) )
	{
		mexErrMsgTxt("patches must be (d x npaches) in SINGLE format");
	}
	npatches     = mxGetN(prhs[1]);


	/* Input 3  */

	infopatches  = (float *)mxGetData(prhs[2]);

	if( (mxGetM(prhs[2]) != 6)  || (mxGetN(prhs[2]) != npatches))
	{
		mexErrMsgTxt("infopatches must be (6 x npaches) in SINGLE format");
	}

	/* Input 4  */


	if ((nrhs > 3) && !mxIsEmpty(prhs[3]) )
	{
		mxtemp                             = mxGetField( prhs[3], 0, "scale" );
		if(mxtemp != NULL)
		{

			if((mxGetM(mxtemp) != 1)  && (mxGetN(mxtemp) != 2) )
			{
				mexErrMsgTxt("options.scale must be (1 x nscale) in double format");
			}

			options.scale                  = mxGetPr(mxtemp);
			options.nscale                 = (int) mxGetN(mxtemp);
			if(options.nscale != nscale)
			{
				mexErrMsgTxt("options.scale must be (1 x nscale) in double format");

			}
		}
		else
		{
			options.scale                 = (double *)mxMalloc(1*sizeof(double));
			for(i = 0 ; i < options.nscale ; i++)
			{
				options.scale[i]          = scale_default[i];
			}	
		}

		mxtemp                            = mxGetField(prhs[3] , 0 , "spyr");
		if(mxtemp != NULL)
		{
			if( mxGetN(mxtemp) != 4 )
			{
				mexErrMsgTxt("pyr must be (nscale x 4) in double format\n");
			}
			options.spyr                  = mxGetPr(mxtemp);
			options.nspyr                 = mxGetM(mxtemp);
		}
		else
		{
			options.nspyr                 = 1;
			options.spyr                  = (double *)mxMalloc(4*sizeof(double));

			options.spyr[0]               = 1.0;
			options.spyr[1]               = 1.0;
			options.spyr[2]               = 1.0;
			options.spyr[3]               = 1.0;
		}

		mxtemp                            = mxGetField(prhs[3] , 0 , "beta");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			temp                          = tmp[0];
			if( (temp < 0.0) )
			{
				mexPrintf("beta must be >= 0, force to 0.1\n");	
				options.beta             = 0.1;
			}
			else
			{
				options.beta             = temp;
			}
		}

		mxtemp                            = mxGetField(prhs[3] , 0 , "assignment");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			tempint                       = (int) tmp[0];
			if( (tempint < 0) || (tempint > 2) )
			{
				mexPrintf("assignment = {0,1,2}, force to 0\n");	
				options.assignment        = 0;
			}
			else
			{
				options.assignment        = tempint;
			}
		}

		mxtemp                            = mxGetField(prhs[3] , 0 , "norm");
		if(mxtemp != NULL)
		{
			if( mxGetN(mxtemp) != 3 )
			{
				mexErrMsgTxt("norm must be (1 x 3) in double format\n");
			}
			options.norm                      = mxGetPr(mxtemp);
			for (i = 0 ; i < 3 ; i++)
			{
				if((options.norm[i] < 0) || (options.norm[i] > 4))
				{
					mexErrMsgTxt("norm must be (1 x 3) in double format\n");
				}
			}
		}
		else
		{
			options.norm                  = (double *)mxMalloc(3*sizeof(double));
			for(i = 0 ; i < 3 ; i++)
			{
				options.norm[i]           = norm_default[i];
			}	
		}

		mxtemp                            = mxGetField(prhs[3] , 0 , "clamp");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			temp                          = tmp[0];
			if( (temp < 0.0) )
			{
				mexPrintf("clamp must be >= 0, force to 0.2\n");	
				options.clamp             = 0.2;
			}
			else
			{
				options.clamp             = temp;
			}
		}
	}
	else
	{
		options.nspyr                 = 1;
		options.spyr                  = (double *)mxMalloc(4*sizeof(double));

		options.spyr[0]               = 1.0;
		options.spyr[1]               = 1.0;
		options.spyr[2]               = 1.0;
		options.spyr[3]               = 1.0;

		options.norm                  = (double *)mxMalloc(3*sizeof(double));
		for(i = 0 ; i < 3 ; i++)
		{
			options.norm[i]           = norm_default[i];
		}	
	}
 
	nH                 = number_histo_spyr(options.spyr , options.nspyr);

	/*------------------------------------------------- Outputs -------------------------------------------------*/

	plhs[0]            =  mxCreateDoubleMatrix(K*nH*ncolor*nscale , 1 , mxREAL);
	H                  =  mxGetPr(plhs[0]);

	/*------------------------------------------------- Main Call ------------------------------------------------*/

	mlhbow_spyr(D , patches , infopatches  , options , H , d , K  , npatches  , nscale , ncolor , nH );

	/*-------------------------------------------------- Free memory ---------------------------------------------*/

	if ( (nrhs > 3) && !mxIsEmpty(prhs[3]) )
	{
		if ( (mxGetField( prhs[3] , 0 , "scale" )) == NULL )
		{
			mxFree(options.scale);
		}
		if ( (mxGetField( prhs[3] , 0 , "spyr" )) == NULL )
		{
			mxFree(options.spyr);
		}
		if ( (mxGetField( prhs[3] , 0 , "norm" )) == NULL )
		{
			mxFree(options.norm);
		}
	}
	else
	{
		mxFree(options.scale);
		mxFree(options.spyr);
        mxFree(options.norm);
	}
}

#else


#endif

/*----------------------------------------------------------------------------------------------------------------------------------------- */
void mlhbow_spyr(float *D , float *patches , float *infopatches , struct opts options , double *H , int d , int K , int npatches  , int nscale , int ncolor ,int nH   )
{
	int i , c , k , s , p , n , l , m;
	int ly, lx , nspyr = options.nspyr , assignment = options.assignment;
	double scaley, scalex;
	double maxfactor = 0.0 , ratio;
	double *scale = options.scale , *spyr = options.spyr;
	float beta = (float)options.beta;
	double clamp = options.clamp;

	float *G ;
	int *x , *y;
	float currentscale , currentcolor , temp , sum , sum_total , maxi;
	double SUM , TEMP , cteK = 1.0/K , mu;

	int dK = d*K , sdK , nsdK = nscale*dK , csdK , np = npatches/(ncolor*nscale), kd , Knp = K*np , idx , index , index1;
	int deltay, deltax, sy , sx, origy, origx , offsety = 0 , offsetx = 0;
	int co , coK = 0, idxmaxi;
	int nycurrent, nxcurrent;

	int norm_all = (int) options.norm[0] , norm_p = (int) options.norm[1] , norm_w = (int) options.norm[2] , V = K*nH*ncolor*nscale;
	int co_p ,  indj;

	G               = (float *)malloc(Knp*sizeof(float));
	x               = (int *)malloc(np*sizeof(int));
	y               = (int *)malloc(np*sizeof(int));

	for (p = 0 ; p < nspyr ; p++)
	{
		maxfactor   = max(maxfactor , spyr[p + 0]*spyr[p + nspyr]);
	}

	for (c = 0  ; c < ncolor ; c++)
	{
		csdK         = c*nsdK;
		currentcolor = (float)(c+1);

		for (s = 0 ; s < nscale ; s++)
		{
			sdK          = s*dK + csdK;		
			currentscale = (float) scale[s];
			co           = 0;
			for (n = 0 ; n < npatches ; n++)
			{
				idx  = 6*n;
				if( (infopatches[2 + idx] == currentscale) && (infopatches[3 + idx] == currentcolor))
				{
					index     = d*n;
					index1    = K*co;
					y[co]     = (int) infopatches[idx];
					x[co]     = (int) infopatches[1 + idx];
					nycurrent = (int) infopatches[4 + idx];
					nxcurrent = (int) infopatches[5 + idx];

					if((assignment == 0) || (assignment == 2))
					{
						for (k = 0 ; k < K ; k++)
						{
							kd            = k*d + sdK;
							sum           = 0.0;
							for(i = 0 ; i < d ; i++)
							{
								temp      = (D[i+kd] - patches[i+index]);
								sum      += (temp*temp);
							}
							G[k+index1]   = (float)sqrt(sum);
						}
					}
					else if(assignment == 1)
					{
						sum_total = 0.0;
						for (k = 0 ; k < K ; k++)
						{
							kd            = k*d + sdK;
							sum           = 0.0;
							for(i = 0 ; i < d ; i++)
							{
								temp      = (D[i+kd] - patches[i+index]);
								sum      += (temp*temp);
							}
							G[k+index1]   = (float)exp(-sum*beta);
							sum_total    += G[k+index1];
						}
						sum_total         = (float)(1.0/sum_total);
						for (k = index1 ; k < (K+index1) ; k++)
						{
							G[k]         *= sum_total;
						}
					}
					co++;
				}
			}

			for (p = 0 ; p < nspyr ; p++)
			{
				scaley      = (spyr[p + nspyr*2]);
				ly          = (int) ( (1 - spyr[p + 0])/scaley + 1);
				deltay      = (int) (nycurrent*scaley);
				sy          = (int) (nycurrent*spyr[p + 0]);
				offsety     = max(0 , (int) ( floor(nycurrent - ( (ly-1)*deltay + sy + 1)) ));

				scalex      = (spyr[p + nspyr*3]);
				lx          = (int) ( (1 - spyr[p + nspyr*1])/scalex + 1);
				deltax      = (int) (nxcurrent*scalex);
				sx          = (int) (nxcurrent*spyr[p + nspyr*1]);
				offsetx     = max(0 , (int) ( floor(nxcurrent - ( (lx-1)*deltax + sx + 1)) ));

				ratio       = maxfactor/(spyr[p + 0]*spyr[p + nspyr]);
				co_p        = 0;

				for(l = 0 ; l < lx ; l++) 
				{
					origx  = offsetx + l*deltax ;
					for(m = 0 ; m < ly ; m++)   
					{
						origy     = offsety + m*deltay ;
						if(assignment == 0) /* Hard assignment */
						{
							for (n = 0 ; n < np ; n++)
							{
								if( (y[n]>=origy) && (y[n]<=origy + sy) && (x[n]>=origx) && (x[n]<=origx + sx) )
								{						
									index     = n*K;
									maxi      = 0.0;
									for (k = 0 ; k < K ; k++)
									{
										if(G[k + index] > maxi)
										{
											maxi    = G[k + index];
											idxmaxi = k;
										}
									}
									H[idxmaxi + coK]++;
								}
							}
						}
						else if(assignment == 1) /* Soft assignment */
						{
							for (n = 0 ; n < np ; n++)
							{
								if( (y[n]>=origy) && (y[n]<=origy + sy) && (x[n]>=origx) && (x[n]<=origx + sx) )
								{						
									index     = n*K;
									for (k = 0 ; k < K ; k++)
									{
										H[k + coK] += (double)(G[k + index]);
									}
								}
							}
						}
						else if(assignment == 2) /* Triangle assignment */
						{
							for (n = 0 ; n < np ; n++)
							{
								if( (y[n]>=origy) && (y[n]<=origy + sy) && (x[n]>=origx) && (x[n]<=origx + sx) )
								{						
									index     = n*K;
									mu        = 0.0;
									for (k = 0 ; k < K ; k++)
									{
										mu   += (double)(G[k + index]);
									}
									mu       *= cteK;
									for (k = 0 ; k < K ; k++)
									{
										H[k + coK] += max(0 , mu - (double)(G[k + index]));
									}

								}
							}
						}

						for(k = coK ; k < (K + coK) ; k++)
						{
							H[k]  *= ratio;
						}
						/* Normalization per subwindows */

						if(norm_w == 1)
						{
							SUM       = 0.0;
							for(k = coK ; k < (K + coK) ; k++)
							{
								SUM   += H[k];
							}
							SUM = 1.0/(SUM + tiny);
							for(k = coK ; k < (K + coK) ; k++)
							{
								H[k]   *= SUM;
							}
						}
						if(norm_w == 2)
						{
							SUM       = 0.0;
							for(k = coK ; k < (K + coK) ; k++)
							{
								TEMP   = H[k];
								SUM   += TEMP*TEMP;
							}
							SUM = 1.0/sqrt(SUM + verytiny);
							for(k = coK ; k < (K + coK) ; k++)
							{
								H[k]   *= SUM;
							}
						}
						if(norm_w == 3)
						{
							SUM       = 0.0;
							for(k = coK ; k < (K + coK) ; k++)
							{
								SUM   += H[k];
							}
							SUM = 1.0/(SUM + tiny);
							for(k = coK ; k < (K + coK) ; k++)
							{
								H[k]   = sqrt(H[k]*SUM);
							}
						}
						if(norm_w == 4)
						{
							SUM       = 0.0;
							for(k = coK ; k < (K + coK) ; k++)
							{
								TEMP   = H[k];
								SUM   += TEMP*TEMP;
							}
							SUM = 1.0/sqrt(SUM + verytiny);
							for(k = coK ; k < (K + coK) ; k++)
							{
								H[k]   *= SUM;

								if(H[k] > clamp)
								{
									H[k] = clamp;
								}
							}
							SUM       = 0.0;
							for(k = coK ; k < (K + coK) ; k++)
							{
								TEMP   = H[k];
								SUM   += TEMP*TEMP;
							}
							SUM = 1.0/sqrt(SUM + verytiny);
							for(k = coK ; k < (K + coK) ; k++)
							{
								H[k]   *= SUM;
							}
						}
						coK  += K;							
						co_p++;
					}
				}

				/* Normalization per pyramid level */

				if(norm_p == 1)
				{
					SUM       = 0.0;
					indj      = coK - K*co_p;
					for(i = indj ; i < (K*co_p + indj) ; i++)
					{
						SUM   += H[i];
					}
					SUM       = 1.0/(SUM + tiny);
					for(i = indj ; i < (K*co_p + indj) ; i++)
					{
						H[i] *= SUM;
					}
				}
				if(norm_p == 2)
				{
					SUM       = 0.0;
					indj      = coK - K*co_p;
					for(i = indj ; i < (K*co_p + indj) ; i++)
					{
						TEMP   = H[i];
						SUM   += TEMP*TEMP;
					}
					SUM = 1.0/sqrt(SUM + verytiny);
					for(i = indj ; i < (K*co_p + indj) ; i++)
					{
						H[i] *= SUM;
					}
				}
				if(norm_p == 3)
				{
					SUM       = 0.0;
					indj      = coK - K*co_p;
					for(i = indj ; i < (K*co_p + indj) ; i++)
					{
						SUM   += H[i];
					}
					SUM = 1.0/(SUM + tiny);
					for(i = indj ; i < (K*co_p + indj) ; i++)
					{
						H[i]   = sqrt(H[i]*SUM);
					}
				}
				if(norm_p == 4)
				{
					SUM       = 0.0;
					indj      = coK - K*co_p;
					for(i = indj ; i < (K*co_p + indj) ; i++)
					{
						TEMP   = H[i];
						SUM   += TEMP*TEMP;
					}

					SUM = 1.0/sqrt(SUM + verytiny);
					for(i = indj ; i < (K*co_p + indj) ; i++)
					{
						H[i]   *= SUM;
						if(H[i] > clamp)
						{
							H[i] = clamp;
						}
					}
					SUM       = 0.0;
					for(i = indj ; i < (K*co_p + indj) ; i++)
					{
						TEMP   = H[i];
						SUM   += TEMP*TEMP;
					}

					SUM = 1.0/sqrt(SUM + verytiny);
					for(i = indj ; i < (K*co_p + indj) ; i++)
					{
						H[i] *= SUM;
					}
				}
			}
		}
	}

	/* Normalization for full descriptor (V x 1) */
	if(norm_all == 1)
	{
		SUM       = 0.0;
		for(i = 0 ; i < V ; i++)
		{
			SUM   += H[i];
		}
		SUM = 1.0/(SUM + tiny);
		for(i = 0 ; i < V ; i++)
		{
			H[i]   *= SUM;
		}
	}
	if(norm_all == 2)
	{
		SUM       = 0.0;
		for(i = 0 ; i < V ; i++)
		{
			TEMP   = H[i];
			SUM   += TEMP*TEMP;
		}
		SUM = 1.0/sqrt(SUM + verytiny);
		for(i = 0 ; i < V ; i++)
		{
			H[i]   *= SUM;
		}
	}
	if(norm_all == 3)
	{
		SUM       = 0.0;
		for(i = 0 ; i < V ; i++)
		{
			SUM   += H[i];
		}
		SUM = 1.0/(SUM + tiny);
		for(i = 0 ; i < V ; i++)
		{
			H[i]   = sqrt(H[i]*SUM);
		}
	}
	if(norm_all == 4)
	{
		SUM       = 0.0;
		for(i = 0 ; i < V ; i++)
		{
			TEMP   = H[i];
			SUM   += TEMP*TEMP;
		}
		SUM = 1.0/sqrt(SUM + verytiny);
		for(i = 0 ; i < V ; i++)
		{
			H[i]   *= SUM;
			if(H[i] > clamp)
			{
				H[i] = clamp;
			}
		}
		SUM       = 0.0;
		for(i = 0 ; i < V ; i++)
		{
			TEMP   = H[i];
			SUM   += TEMP*TEMP;
		}
		SUM = 1.0/sqrt(SUM + verytiny);
		for(i = 0 ; i < V ; i++)
		{
			H[i]   *= SUM;
		}
	}

	free(G);
	free(x);
	free(y);
}

/*----------------------------------------------------------------------------------------------------------------------------------------- */
int	number_histo_spyr(double *spyr , int nspyr)
{
	int l , nH = 0 , ly , lx ;

	for (l = 0 ; l < nspyr ; l++)
	{
		ly          = (int) ( (1 - spyr[l + 0]) /(spyr[l + nspyr*2]) + 1);  
		lx          = (int) ( (1 - spyr[l + nspyr*1])/(spyr[l + nspyr*3]) + 1); 
		nH         += ly*lx;
	}
	return nH;
}
/*----------------------------------------------------------------------------------------------------------------------------------------- */
