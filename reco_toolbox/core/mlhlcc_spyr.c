
/*

Color Multi-Level Histograms of Approximated Locality-Constraint  Linear Coding with Spatial Pyramid.

Usage
------

H = mlhlcc_spyr(D , patches , infopatches , [options] );


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

	   assignment              Pooling assignment (0,1,2): 0 <=> Max Pooling, 1<=> Mean Pooling, 2<=> RMS of Energy Pooling. (default assignment = 0)

	   knn                     Number of Neigbors (default nn = 5)

	   lambda                  Regularization factor to improve condition (default lambda = 1e-4)

 	   norm                    Normalization vector (1 x 3) : [for all subwindows, for each subwindows of a pyramid level, for each subwindows]
                               norm = 0 <=> no normalization, norm = 1 <=> v=v/(sum(v)+epsi), norm = 2 <=> v=v/sqrt(sum(v²)+epsi²),
	                           norm = 3 <=> v=sqrt(v/(sum(v)+epsi)) , norm = 4 <=> L2-clamped (default norm = [0 , 0 , 4])

       clamp                   Clamping value (default clamp = 0.2)


Outputs
-------

H                              mlhlcc_spyr features (K*nH*nscale*dimcolor x 1) in double format  and nH is the total number of subwindows defined
                               by the spatial pyramid spyr.

To compile
----------


mex  -g -output mlhlcc_spyr.dll mlhlcc_spyr.c "C:\Program Files\MATLAB\R2009b\extern\lib\win32\microsoft\libmwlapack.lib"

mex  -f mexopts_intel10.bat -output mlhlcc_spyr.dll mlhlcc_spyr.c "C:\Program Files\MATLAB\R2009b\extern\lib\win32\microsoft\libmwlapack.lib"

mex  -f mexopts_intel10.bat -output mlhlcc_spyr.dll mlhlcc_spyr.c "C:\Program Files\Intel\Compiler\11.1\065\mkl\ia32\lib\mkl_core.lib" "C:\Program Files\Intel\Compiler\11.1\065\mkl\ia32\lib\mkl_intel_c.lib" "C:\Program Files\Intel\Compiler\11.1\065\mkl\ia32\lib\mkl_intel_thread.lib" "C:\Program Files\Intel\Compiler\11.1\065\lib\ia32\libiomp5md.lib"


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
options.nn            = 5;
options.lambda        = 1e-4;
options.assignment    = 0;
options.norm          = [0 , 0 , 4];
options.clamp         = 0.2;



D                     = rand(d , K , nscale , ncolor , 'single');
patches               = rand(d , npatches , 'single');
infopatches           = [4+(ny-4)*rand(1,npatches , 'single') ; 4+(nx-4)*rand(1,npatches , 'single') ; [0.75*ones(1 , npatches/2 , 'single') , ones(1 , npatches/2 , 'single')] ; ones(1 , npatches , 'single') ; ny*ones(1 , npatches , 'single') ; nx*ones(1 , npatches , 'single')];


H                     = mlhlcc_spyr(D , patches , infopatches , options );

figure(1)
plot(1:length(H) , H)



Author : Sébastien PARIS : sebastien.paris@lsis.org
-------  Date : 09/06/2010



Reference : 
---------       
                

*/

#include <math.h>
#include <mex.h>

#define tiny 1e-8
#define verytiny 1e-15

#ifndef max
#define max(a,b) (a >= b ? a : b)
#define min(a,b) (a <= b ? a : b)
#endif

#if defined(__OS2__)  || defined(__WINDOWS__) || defined(WIN32) || defined(_MSC_VER)
#define BLASCALL(f) f
#else
#define BLASCALL(f) f ##_
#endif

struct opts
{
	double *scale;
	int     nscale;
	double *spyr;
	int     nspyr;
    int     nn;
    double  lambda;
	int     assignment;
	double *norm;
	double  clamp;
};


/*-------------------------------------------------------------------------------------------------------------- */

/* Function prototypes */

extern void BLASCALL(dposv)( char* , int* , int* , double* , int* , double* , int* , int* );
int	number_histo_spyr(double * , int );
void quickindex (double  *, int * , int , int );
void mlhlcc_spyr(float * , float * , float *  , struct opts , double *, int , int , int , int , int , int );

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
	options.nn            = 5;
	options.lambda        = 1e-4;
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

		mxtemp                            = mxGetField(prhs[3] , 0 , "nn");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			tempint                       = (int) tmp[0];
			if( (tempint < 1))
			{
				mexPrintf("nn must be >= 1, force to 5\n");	
				options.nn                = 5;
			}
			else
			{
				options.nn                = tempint;
			}
		}

		mxtemp                            = mxGetField(prhs[3] , 0 , "lambda");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			temp                          = tmp[0];
			if( (temp < 0.0) )
			{
				mexPrintf("lambda must be >= 0, force to 10e-4\n");	
				options.lambda             = 10e-4;
			}
			else
			{
				options.lambda             = temp;
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

	mlhlcc_spyr(D , patches , infopatches  , options , H , d , K  , npatches  , nscale , ncolor , nH);

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
void mlhlcc_spyr(float *D , float *patches , float *infopatches , struct opts options , double *H , int d , int K , int npatches  , int nscale , int ncolor , int nH   )
{
	int i , j , c , k , s , p , n , l , m , t;
	int ly, lx , nspyr = options.nspyr , assignment = options.assignment , nn = options.nn;
	double scaley, scalex;
	double maxfactor = 0.0 , ratio;
	double *scale = options.scale , *spyr = options.spyr;
	double lambda = options.lambda;
	double clamp = options.clamp;
	double *G , *distxD , *Z , *w , *C;
	int *x , *y , *idxdistxD;
	float currentscale , currentcolor;
	double SUM , TEMP , temp , sum;
	int dK = d*K , sdK , nsdK = nscale*dK , csdK , np = npatches/(ncolor*nscale), kd , Knp = K*np , idx , index , index1 , indexK, K1 = K - 1;
	int deltay, deltax, sy , sx, origy, origx , offsety = 0 , offsetx = 0;
	int co , coK = 0, nn1 = nn + 1, nn2 = nn*nn , id , jd , inn  , one=1,info;
	int nycurrent, nxcurrent;

	int norm_all = (int) options.norm[0] , norm_p = (int) options.norm[1] , norm_w = (int) options.norm[2] , V = K*nH*ncolor*nscale;
	int co_p ,  indj;


	G               = (double *)malloc(Knp*sizeof(double));
	Z               = (double *)calloc(d*nn,sizeof(double));
	distxD          = (double *)malloc(K*sizeof(double));
	C               = (double *)calloc(nn2,sizeof(double));
	w               = (double *)malloc(nn*sizeof(double));
	x               = (int *)malloc(np*sizeof(int));
	y               = (int *)malloc(np*sizeof(int));
	idxdistxD       = (int *)malloc(K*sizeof(int));


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
			for (i = 0 ; i < Knp ;i++)
			{
				G[i]     = 0.0;
			}
			for (n = 0 ; n < npatches ; n++)
			{
				idx      = 6*n;
				if( (infopatches[2 + idx] == currentscale) && (infopatches[3 + idx] == currentcolor))
				{
					index     = d*n;
					indexK    = co*K;
					y[co]     = (int) infopatches[idx];
					x[co]     = (int) infopatches[1 + idx];
					nycurrent = (int) infopatches[4 + idx];
					nxcurrent = (int) infopatches[5 + idx];

					for (k = 0 ; k < K ; k++)
					{
						kd            = k*d + sdK;
						sum           = 0.0;
						for(i = 0 ; i < d ; i++)
						{
							temp      = (D[i+kd] - patches[i+index]);
							sum      += temp*temp;

						}
						distxD[k]     = sum;
						idxdistxD[k]  = k;
					}

					quickindex(distxD, idxdistxD, 0 , K1);

					for (k = 0 ; k < nn ; k++)
					{
						kd     = k*d;
						index1 = idxdistxD[k]*d;
						for(i = 0 ; i < d ; i++)
						{
							Z[i + kd] = (D[i + index1] - patches[i + index]);
						}											
					}

					/* C =triu(Zt*Z )*/

					for (i = 0 ; i < nn; i++)
					{

						id         = i*d;
						inn        = i*nn;

						for (j = i  ; j < nn ; j++)
						{
							jd    = j*d;
							sum   = 0.0;
							for (t = 0 ; t < d ; t++)
							{
								sum += (Z[t + id]*Z[t + jd]);
							}

							C[j + inn] = sum;
						}

					}

					/* full C from triu(C) */

					for (i = 0 ; i < nn-1; i++)
					{
						inn  = i*nn;
						for (j = i + 1 ; j < nn ; j++)
						{
							C[i + j*nn] = C[j + inn];				
						}
					}

					/* Trace(C) */

					sum = 0.0;
					for (i = 0 ; i < nn2 ; i += nn1)
					{
						sum   += C[i];
					}

					sum   *= lambda;
					for (i = 0 ; i < nn2 ; i += nn1)
					{
						C[i]   += sum;
					}

					/* Solve 1 = C*w => w = C\ones(nn,1) */
					for (i = 0 ; i < nn ; i++)
					{
						w[i]   = 1.0;
					}

					BLASCALL(dposv)( "Upper" , &nn , &one , C , &nn , w , &nn , &info );

					sum       = 0.0;
					for (i = 0 ; i < nn ; i++)
					{
						sum   += w[i];
					}

					sum       = 1.0/sum;
					for (i = 0 ; i < nn ; i++)
					{
/*						w[i]                    *= sum; */
						G[idxdistxD[i] + indexK] = fabs(w[i]*sum);
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

						if(assignment == 0) /* Max Pooling */
						{
							for (n = 0 ; n < np ; n++)
							{
								if( (y[n]>=origy) && (y[n]<=origy + sy) && (x[n]>=origx) && (x[n]<=origx + sx) )
								{						
									index     = n*K;

									for (k = 0 ; k < K ; k++)
									{
										temp   = G[k + index];
										if(temp > H[k + coK])
										{
											H[k + coK]    = temp;
										}
									}
								}
							}
						}
						else if (assignment == 1) /* Mean Poolng */
						{
							co = 0;
							for (n = 0 ; n < np ; n++)
							{
								if( (y[n]>=origy) && (y[n]<=origy + sy) && (x[n]>=origx) && (x[n]<=origx + sx) )
								{						
									index     = n*K;

									for (k = 0 ; k < K ; k++)
									{
										H[k + coK] += G[k + index];
										co++;
									}
								}
								temp = 1.0/(double)co;
								for (k = 0 ; k < K ; k++)
								{
									H[k + coK] *=temp;
								}
							}
						}
						else if (assignment == 2)
						{
							co = 0;
							for (n = 0 ; n < np ; n++)
							{								
								if( (y[n]>=origy) && (y[n]<=origy + sy) && (x[n]>=origx) && (x[n]<=origx + sx) )
								{						
									index     = n*K;

									for (k = 0 ; k < K ; k++)
									{
										temp        = G[k + index];
										H[k + coK] += (temp*temp);
										co++;
									}
								}

								temp = 1.0/(double)co;
								for (k = 0 ; k < K ; k++)
								{
									H[k + coK] = sqrt(H[k + coK]*temp);
								}
							}
						}
						for(k = coK ; k < (K + coK) ; k++)
						{
							H[k]  *= ratio;
						}
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
	free(Z);
	free(C);
	free(distxD);
	free(w);
	free(idxdistxD);
	free(x);
	free(y);
}

/*----------------------------------------------------------------------------------------------------------------------------------------- */
void quickindex (double  *a, int *index , int lo, int hi)
{
/*  lo is the lower index, hi is the upper index
   of the region of array a that is to be sorted */

    int i=lo, j=hi , ind;
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
			ind      = index[i];
			index[i] = index[j];
			index[j] = ind;
            i++; 
			j--;
        }
    }
	while (i<=j);

    /*  recursion */
    if (lo<j) quickindex(a , index , lo , j);
    if (i<hi) quickindex(a , index , i , hi);
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
