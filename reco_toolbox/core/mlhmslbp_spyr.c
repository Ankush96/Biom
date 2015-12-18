
/*

Multi-Level Histograms of Color Multi-Scale Local Binary Pattern with Spatial Pyramid.

Usage
------

H     =  mlhmslbp_spyr(I , [options] );


Inputs
-------

I                              Input image (ny x nx x [3]) in UINT8 format. 
options 
       spyr                    Spatial Pyramid (nspyr x 4) (default [1 , 1 , 1 , 1] with nspyr = 1)
                               where spyr(i,1) is the ratio of ny in y axis of the blocks at level i (by(i) = spyr(i,1)*ny)
                               where spyr(i,2) is the ratio of nx in x axis of the blocks at level i (bx(i) = spyr(i,3)*nx)
                               where spyr(i,3) is the ratio of ny in y axis of the shifting at level i (deltay(i) = spyr(i,2)*ny)
                               where spyr(i,4) is the ratio of nx in x axis of the shifting at level i (deltax(i) = spyr(i,4)*nx)

       scale                   Multi-Scale vector (1 x nscale) (default scale = 1) where scale(i) = s is the size's factor to apply to each 9 blocks
                               in the LBP computation, i = 1,...,nscale

       color                   0 : force gray-scale (dimcolor = 1, default), 1 : RGB-LBP (dimcolor = 3), 2 : nRGB-LBP (dimcolor = 3), 3 : Opponent-LBP (dimcolor = 3), 
                               4 : nOpponent-LBP (dimcolor = 2), 5 : Hue-LBP (dimcolor = 1)


	   maptable                Mapping table for LBP codes. maptable = 0 <=> normal LBP = {0,...,255} (default), 
	                           maptable = 1 <=> uniform LBP = {0,...,58}, maptable = 2 <=> rotation-invariant LBP = {0,...,35},
							   maptable = 3 <=> uniform rotation-invariant LBP = {0,...,9}

	   useFF                   Use LBP-Fourier Feature rotation invariant (force maptable = 1)

       improvedLBP             0 for default 8 bits encoding, 1 for 9 bits encoding (8 + central area)

       kernelx                 Kernel in x-direction for computing the gradient (default kernelx = [1])
       kernely                 Kernel in y-direction for computing the gradient (default kernely = [1])
                               If numel(kernelx) ~1 || numel(kernely) ~1, compute the magnitude, i.e. sqrt(dx*dx + dy*dy)
                               For example if kernelx = [1 2 1 ; 0 0 0 ; -1 -2 -1] and kernely = kernelx', this is equivalent to compute filter
                               the image with a sobel filter.


       rmextremebins           Force to zero bin = {0 , {255,58,9}} if  rmextremebins = 1 (default rmextremebins = 1)

 	   norm                    Normalization vector (1 x 3) : [for all subwindows, for each subwindows of a pyramid level, for each subwindows]
                               norm = 0 <=> no normalization, norm = 1 <=> v=v/(sum(v)+epsi), norm = 2 <=> v=v/sqrt(sum(v²)+epsi²),
	                           norm = 3 <=> v=sqrt(v/(sum(v)+epsi)) , norm = 4 <=> L2-clamped (default norm = [0 , 0 , 4])

       clamp                   Clamping value (default clamp = 0.2)


Output
------

H                              mlhmslbp_spyr features (nbin*nH*nscale*dimcolor x 1) in double format where nbin = ([256,59,10]*(improvedLBP+1)) (depending on improvedLBP) and nH is the total number of subwindows defined
                               by the spatial pyramid spyr, i.e. nH = sum(floor(((1 - spyr(:,1))./(spyr(:,3)) + 1)).*floor((1 - spyr(:,2))./(spyr(:,4)) + 1)).

To compile
----------

i ) Link with FFTW

   a) for win32

     mex  mlhmslbp_spyr.c libfftw_x32.lib

     mex  -g -output mlhmslbp_spyr.dll mlhmslbp_spyr.c libfftw_x32.lib

     mex  -f mexopts_intel10.bat -output mlhmslbp_spyr.dll mlhmslbp_spyr.c libfftw_x32.lib


   b) for win64

    mex  mlhmslbp_spyr.c libfftw_x64.lib

	mex  -f mexopts_intel11_64.bat -output mlhmslbp_spyr.mexw64 mlhmslbp_spyr.c libfftw_x64.lib

ii) Link with MKL

   a) for win32

     mex  -f mexopts_intel10.bat -output mlhmslbp_spyr.mexw32 mlhmslbp_spyr.c "C:\Program Files\Intel\Compiler\11.1\065\mkl\ia32\lib\mkl_core.lib" "C:\Program Files\Intel\Compiler\11.1\065\mkl\ia32\lib\mkl_intel_c.lib" "C:\Program Files\Intel\Compiler\11.1\065\mkl\ia32\lib\mkl_intel_thread.lib"

   b) for win64

    mex  -f mexopts_intel11_64.bat -output mlhmslbp_spyr.mexw64 mlhmslbp_spyr.c "C:\Program Files (x86)\Intel\Compiler\11.1\065\mkl\em64t\lib\mkl_core.lib" "C:\Program Files (x86)\Intel\Compiler\11.1\065\mkl\em64t\lib\mkl_intel_lp64.lib" "C:\Program Files (x86)\Intel\Compiler\11.1\065\mkl\em64t\lib\mkl_intel_thread.lib"



Example 1
---------

I                     = imread('02769_Right_StudentOffice.jpeg');
I                     = imread('image_0174.jpg');


options.spyr          = [1 , 1 , 1 , 1 ; 1/2 , 1/2 , 1/4 , 1/4];
options.scale         = [1 , 2 , 3 , 4 ];
options.maptable      = 0;
options.useFF         = 0;
options.improvedLBP   = 0;
options.rmextremebins = 1;
options.color         = 0;
options.norm          = [0 , 0 , 4];
options.clamp         = 0.2;


nH                    = length(options.scale)*sum(floor(((1 - options.spyr(:,1))./(options.spyr(:,3)) + 1)).*floor((1 - options.spyr(:,2))./(options.spyr(:,4)) + 1));

H                     = mlhmslbp_spyr(I , options );

figure(1)
plot(H)

figure(2)
imagesc(reshape(H , 256 , nH) )



Example 2
---------

I                     = imread('image_0001.jpg');

options.spyr          = [1 , 1 , 1 , 1 ; 1/2 , 1/2 , 1/4 , 1/4];
options.scale         = [1 , 2 , 3 , 4];
options.rmextremebins = 1;
options.norm          = [0 , 0 , 4];
options.clamp         = 0.2;



nH                    = length(options.scale)*sum(floor(((1 - options.spyr(:,1))./(options.spyr(:,3)) + 1)).*floor((1 - options.spyr(:,2))./(options.spyr(:,4)) + 1));


tic,H                 = mlhmslbp_spyr(I , options );,toc
figure(1)
plot(reshape(H , 256 , nH) )
figure(2)
imagesc(reshape(H , 256 , nH) )




Example 3
---------

I                     = imread('image_0001.jpg');

options.spyr          = [1 , 1 , 1 , 1 ; 1/2 , 1/2 , 1/4 , 1/4];
options.scale         = [1 , 2];
options.improvedLBP   = 1;
options.rmextremebins = 1;
options.norm          = [0 , 0 , 4];
options.clamp         = 0.2;



nH                    = length(options.scale)*sum(floor(((1 - options.spyr(:,1))./(options.spyr(:,3)) + 1)).*floor((1 - options.spyr(:,2))./(options.spyr(:,4)) + 1));


tic,H                 = mlhmslbp_spyr(I , options );,toc
figure(1)
plot(reshape(H , 512 , nH) )
figure(2)
imagesc(reshape(H , 512 , nH) )


Example 4
---------

I                     = imread('image_0001.jpg');
spyr                  = [1 , 1 , 1 , 1];
scale                 = 1:4:26;
options.rmextremebins = 1;
options.norm          = [0 , 0 , 1];

tic,H                 = reshape(mlhmslbp_spyr(I , options ) , 256 , length(scale));,toc

figure(1)

plot(H)
legend((num2str(scale')))




Example 5
---------

I                     = imread('02769_Right_StudentOffice.jpeg');

options.spyr          = [1 , 1 , 1 , 1 ; 1/2 , 1/2 , 1/4 , 1/4];
options.scale         = [1 , 2];
options.kernelx       = [1 2 1 ; 0 0 0 ; -1 -2 -1];  %sobel mask
options.kernely       = [1 0 -1 ; 2 0 -2 ; 1 0 -1];  %sobel mask
options.improvedLBP   = 1;
options.color         = 5;
options.rmextremebins = 1;
options.norm          = [0 , 0 , 4];
options.clamp         = 0.2;



nH                    = length(options.scale)*sum(floor(((1 - options.spyr(:,1))./(options.spyr(:,3)) + 1)).*floor((1 - options.spyr(:,2))./(options.spyr(:,4)) + 1));


tic,H                 = mlhmslbp_spyr(I , options );,toc
figure(1)
plot(reshape(H , 512 , nH) )
figure(2)
imagesc(reshape(H , 512 , nH) )


Example 5
---------

I1                    = imread('rice.png');
I2                    = imrotate(I1,90);



options.spyr          = [1 , 1 , 1 , 1];
options.scale         = [1];
options.maptable      = 1;
options.useFF         = 0;
options.improvedLBP   = 0;
options.rmextremebins = 0;
options.color         = 0;
options.norm          = [0 , 0 , 1];


nH                    = length(options.scale)*sum(floor(((1 - options.spyr(:,1))./(options.spyr(:,3)) + 1)).*floor((1 - options.spyr(:,2))./(options.spyr(:,4)) + 1));

H1                    = mlhmslbp_spyr(I1 , options );
H2                    = mlhmslbp_spyr(I2 , options );

figure(1)
plot(1:length(H1) , H1 , 1:length(H2) , H2 , 'r')





Author : Sébastien PARIS : sebastien.paris@lsis.org
-------  Date : 09/06/2010

References [1] Timo Ahonen, Jiri Matas, Chu He, and Matti Pietikäinen ,
---------      "Rotation Invariant Image Description with Local Binary Pattern Histogram Fourier Features"
           [2] Zhang, L. and Chu, R.F. and Xiang, S.M. and Liao, S.C. and Li, S.Z, 
		       "Face Detection Based on Multi-Block LBP Representation",ICB07
		   [3] Jianxin Wu and James M. Rehg, "CENTRIST: A Visual Descriptor for Scene Categorization",
		       IEEE Transactions on Pattern Analysis and Machine Intelligence, accepted     
           

Changelog : - add extra option for normalization
----------

*/


#include <time.h>
#include <math.h>
#include <mex.h>
#include "fftw3.h" 



#define tiny 1e-8
#define verytiny 1e-15
#define sqrt3 1.73205080756888
#define invsqrt2 0.707106781186547
#define invsqrt3 0.577350269189626
#define invsqrt6 0.408248290463863

#ifndef max
    #define max(a,b) (a >= b ? a : b)
    #define min(a,b) (a <= b ? a : b)
#endif


struct opts
{
	double *scale;
	int nscale;
	double  *spyr;
	int     nspyr;
	double *kernelx;
	int    kyx;
	int    kxx;
	double *kernely;
	int    kyy;
	int    kxy;
	int    color;
	int    maptable;
    int    useFF;
	int    improvedLBP;
	int    rmextremebins;
	double *norm;
	double clamp;
};


/*-------------------------------------------------------------------------------------------------------------- */


/* Function prototypes */

int	number_histo_lbp(double * , int);
void rgb2gray(unsigned char * , int , int , double *);
void rgb2nrgb(unsigned char * , int , int , double *);
void rgb2opponent(unsigned char * , int , int , double *);
void rgb2nopponent(unsigned char * , int , int , double *);
void rgb2hue(unsigned char * , int , int , double *);


void MakeIntegralImage(double *, double *, int , int , double *);
double Area(double * , int , int , int , int , int );
void conv2(double *, double *, int  , int , int  , int , int , int , double * , double * );
void compute_hmblbp(double *, int , int , int , int , int  , int  , double * , int , double , double * , int , int* , struct opts  );
void compute_hmblbpff(double *, int , int , int , int , int  , int  , double * , int , double , double * , int , int* , fftw_complex *, fftw_plan , double * , struct opts  );
void mlhmslbp_spyr(double * , double * , int  , int   , int , int , struct opts  );

/*-------------------------------------------------------------------------------------------------------------- */

#ifdef MATLAB_MEX_FILE
void mexFunction( int nlhs, mxArray *plhs[] , int nrhs, const mxArray *prhs[] )
{  
	unsigned char *im;
	const int *dimim;
	double *I;
	double *H;
	double	kernelx_default[1]  = {1};
	double	kernely_default[1]  = {1};
	double  norm_default[3]     = {0 , 0 , 4};
	int i , ny , nx , nynx , dimcolor = 1 , nH = 1 , npt_ker = 1 , Nbins;
	struct opts options;
	mxArray *mxtemp;
	double *tmp , temp;
	int tempint;

	options.nscale        = 1;
	options.nspyr         = 1;
	options.kyx           = 1;
	options.kxx           = 1;
	options.kxy           = 1;
	options.kyy           = 1;
	options.color         = 0;
	options.maptable      = 0;
    options.useFF         = 0;
	options.improvedLBP   = 0;
	options.rmextremebins = 1;
	options.clamp         = 0.2;


	/* Input 1  */


	if( (mxGetNumberOfDimensions(prhs[0]) > 3) || !mxIsUint8(prhs[0]) )
	{
		mexErrMsgTxt("I must be (ny x nx x [3]) in UINT8 format\n");
	}

	im          = (unsigned char *)mxGetData(prhs[0]);
	dimim       = mxGetDimensions(prhs[0]);
	ny          = dimim[0];
	nx          = dimim[1];
	nynx        = ny*nx;

	/* Input 2  */


	if ((nrhs > 1) && !mxIsEmpty(prhs[1]) )
	{
		mxtemp                            = mxGetField(prhs[1] , 0 , "scale");
		if(mxtemp != NULL)
		{
			if( mxGetM(mxtemp) != 1 )
			{
				mexErrMsgTxt("spyr must be (1 x nscale) in double format\n");
			}
			options.scale                 = mxGetPr(mxtemp);
			options.nscale                = mxGetN(mxtemp);
		}
		else
		{
			options.nscale                = 1;
			options.scale                 = (double *)mxMalloc(sizeof(double));
			options.scale[0]              = 1.0;
		}

		mxtemp                            = mxGetField(prhs[1] , 0 , "spyr");
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

		mxtemp                            = mxGetField( prhs[1], 0, "kernelx" );
		if(mxtemp != NULL)
		{
			options.kernelx               = mxGetPr(mxtemp);
			options.kyx                   = (int) mxGetM(mxtemp);
			options.kxx                   = (int) mxGetN(mxtemp);
		}
		else
		{
			options.kernelx               = (double *)mxMalloc(npt_ker*sizeof(double));
			for(i = 0 ; i < npt_ker ; i++)
			{
				options.kernelx[i]        = kernelx_default[i];
			}	
		}

		mxtemp                            = mxGetField( prhs[1], 0, "kernely" );
		if(mxtemp != NULL)
		{
			options.kernely               = mxGetPr(mxtemp);
			options.kyy                   = (int) mxGetM(mxtemp);
			options.kxy                   = (int) mxGetN(mxtemp);
		}
		else
		{
			options.kernely               = (double *)mxMalloc(npt_ker*sizeof(double));
			for(i = 0 ; i < npt_ker ; i++)
			{
				options.kernely[i]        = kernely_default[i];
			}	
		}

		mxtemp                            = mxGetField(prhs[1] , 0 , "color");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			tempint                       = (int) tmp[0];
			if( (tempint < 0) || (tempint > 6))
			{
				mexPrintf("color = {0,1,2,3,4,5}, force to 0\n");	
				options.color             = 0;
			}
			else
			{
				options.color            = tempint;
			}
		}

		mxtemp                            = mxGetField(prhs[1] , 0 , "maptable");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			tempint                       = (int) tmp[0];
			if( (tempint < 0) || (tempint > 3))
			{
				mexPrintf("maptable = {0,1,2,3}, force to 0\n");	
				options.maptable          = 0;
			}
			else
			{
				options.maptable          = tempint;
			}
		}

		mxtemp                            = mxGetField(prhs[1] , 0 , "useFF");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			tempint                       = (int) tmp[0];
			if( (tempint < 0) || (tempint > 1))
			{
				mexPrintf("useFF = {0,1}, force to 0\n");	
				options.useFF             = 0;
			}
			else
			{
				options.useFF            = tempint;
			}
		}

		mxtemp                            = mxGetField(prhs[1] , 0 , "improvedLBP");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			tempint                       = (int) tmp[0];
			if( (tempint < 0) || (tempint > 1))
			{
				mexPrintf("improvedLBP = {0,1}, force to 0\n");	
				options.improvedLBP      = 0;
			}
			else
			{
				options.improvedLBP      = tempint;
			}
		}

		mxtemp                            = mxGetField(prhs[1] , 0 , "rmextremebins");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			tempint                       = (int) tmp[0];
			if( (tempint < 0) || (tempint > 1))
			{
				mexPrintf("rmextremebins = {0,1}, force to 0\n");	
				options.rmextremebins     = 0;
			}
			else
			{
				options.rmextremebins     = tempint;
			}
		}

		mxtemp                            = mxGetField(prhs[1] , 0 , "norm");
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

		mxtemp                            = mxGetField(prhs[1] , 0 , "clamp");
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

		options.nscale                = 1;
		options.scale                 = (double *)mxMalloc(1*sizeof(double));
		options.scale[0]              = 1.0;

		options.kernelx               = (double *)mxMalloc(npt_ker*sizeof(double));
		options.kernely               = (double *)mxMalloc(npt_ker*sizeof(double));

		for(i = 0 ; i < npt_ker ; i++)
		{
			options.kernelx[i]        = kernelx_default[i];
			options.kernely[i]        = kernely_default[i];
		}	
		options.norm                  = (double *)mxMalloc(3*sizeof(double));
		for(i = 0 ; i < 3 ; i++)
		{
			options.norm[i]           = norm_default[i];
		}	
	}

	if(options.useFF)
	{
		options.maptable      = 1;
	}

	if((mxGetNumberOfDimensions(prhs[0]) == 2))
	{
		options.color  = 0;
		I              = (double *)mxMalloc(nynx*sizeof(double));
		dimcolor       = 1;
		for (i = 0 ; i < nynx ; i++)
		{
			I[i]       = (double)im[i];
		}
	}
	else
	{
		if((options.color == 0) )
		{
			I        = (double *)mxMalloc(nynx*sizeof(double));
			rgb2gray(im , ny , nx , I);
			dimcolor = 1;
		}

		else if (options.color == 1)
		{
			I        = (double *)mxMalloc(3*nynx*sizeof(double));
			for (i = 0 ; i < 3*nynx ; i++)
			{
				I[i] = (double)im[i];
			}
			dimcolor = 3;
		}
		else if (options.color == 2)
		{
			I        = (double *)mxMalloc(3*nynx*sizeof(double));
			rgb2nrgb(im , ny , nx , I);
			dimcolor = 3;
		}

		else if (options.color == 3)
		{
			I        = (double *)mxMalloc(3*nynx*sizeof(double));
			rgb2opponent(im , ny , nx , I);
			dimcolor = 3;
		}
		else if(options.color == 4)
		{
			I        = (double *)mxMalloc(2*nynx*sizeof(double));
			rgb2nopponent(im , ny , nx , I);
			dimcolor = 2;
		}
		else if(options.color == 5)
		{
			I        = (double *)mxMalloc(nynx*sizeof(double));
			rgb2hue(im , ny , nx , I);
			dimcolor = 1;
		}
	}

	nH                 = number_histo_lbp(options.spyr , options.nspyr);

	if(options.maptable == 0)
	{
		Nbins          = 256;
	}
	else if(options.maptable == 1)
	{
		if(options.useFF)
		{
			Nbins      = 38;
		}
		else
		{
			Nbins      = 59;
		}
	}
	else if(options.maptable == 2)
	{
		Nbins          = 36;
	}

	else if(options.maptable == 3)
	{
		Nbins          = 10;
	}

	/*----------------------- Outputs -------------------------------*/

	plhs[0]            =  mxCreateDoubleMatrix((1+options.improvedLBP)*Nbins*nH*options.nscale*dimcolor , 1 , mxREAL);
	H                  =  mxGetPr(plhs[0]);

	/*------------------------ Main Call ----------------------------*/

	mlhmslbp_spyr(I , H  ,  ny , nx  , nH , dimcolor , options );

	/*--------------------------- Free memory -----------------------*/

	if ( (nrhs > 1) && !mxIsEmpty(prhs[1]) )
	{
		if ( (mxGetField( prhs[1] , 0 , "spyr" )) == NULL )
		{
			mxFree(options.spyr);
		}
		if ( (mxGetField( prhs[1] , 0 , "scale" )) == NULL )
		{
			mxFree(options.scale);
		}
		if ( (mxGetField( prhs[1] , 0 , "kernelx" )) == NULL )
		{
			mxFree(options.kernelx);
		}
		if ( (mxGetField( prhs[1] , 0 , "kernely" )) == NULL )
		{
			mxFree(options.kernely);
		}
		if ( (mxGetField( prhs[1] , 0 , "norm" )) == NULL )
		{
			mxFree(options.norm);
		}
	}
	else
	{
		mxFree(options.spyr);
		mxFree(options.scale);
		mxFree(options.kernelx);
		mxFree(options.kernely);
		mxFree(options.norm);
	}
	mxFree(I);
}

#else


#endif

/*----------------------------------------------------------------------------------------------------------------------------------------- */
void mlhmslbp_spyr(double *I , double *H , int ny , int nx , int nH , int dimcolor , struct opts options  )
{
	double *kernelx = options.kernelx , *kernely = options.kernely;
	double *spyr = options.spyr , *scale = options.scale;
	double *grady, *gradx , *tempgrady , *tempgradx , *mag;
	double *II , *IItemp;
	double *Htemp;
	int *table;
	int table_normal[256] = {0 , 1 , 2 , 3 , 4 , 5 , 6 , 7 , 8 , 9 , 10 , 11 , 12 , 13 , 14 , 15 , 16 , 17 , 18 , 19 , 20 , 21 , 22 , 23 , 24 , 25 , 26 , 27 , 28 , 29 , 30 , 31 , 32 , 33 , 34 , 35 , 36 , 37 , 38 , 39 , 40 , 41 , 42 , 43 , 44 , 45 , 46 , 47 , 48 , 49 , 50 , 51 , 52 , 53 , 54 , 55 , 56 , 57 , 58 , 59 , 60 , 61 , 62 , 63 , 64 , 65 , 66 , 67 , 68 , 69 , 70 , 71 , 72 , 73 , 74 , 75 , 76 , 77 , 78 , 79 , 80 , 81 , 82 , 83 , 84 , 85 , 86 , 87 , 88 , 89 , 90 , 91 , 92 , 93 , 94 , 95 , 96 , 97 , 98 , 99 , 100 , 101 , 102 , 103 , 104 , 105 , 106 , 107 , 108 , 109 , 110 , 111 , 112 , 113 , 114 , 115 , 116 , 117 , 118 , 119 , 120 , 121 , 122 , 123 , 124 , 125 , 126 , 127 , 128 , 129 , 130 , 131 , 132 , 133 , 134 , 135 , 136 , 137 , 138 , 139 , 140 , 141 , 142 , 143 , 144 , 145 , 146 , 147 , 148 , 149 , 150 , 151 , 152 , 153 , 154 , 155 , 156 , 157 , 158 , 159 , 160 , 161 , 162 , 163 , 164 , 165 , 166 , 167 , 168 , 169 , 170 , 171 , 172 , 173 , 174 , 175 , 176 , 177 , 178 , 179 , 180 , 181 , 182 , 183 , 184 , 185 , 186 , 187 , 188 , 189 , 190 , 191 , 192 , 193 , 194 , 195 , 196 , 197 , 198 , 199 , 200 , 201 , 202 , 203 , 204 , 205 , 206 , 207 , 208 , 209 , 210 , 211 , 212 , 213 , 214 , 215 , 216 , 217 , 218 , 219 , 220 , 221 , 222 , 223 , 224 , 225 , 226 , 227 , 228 , 229 , 230 , 231 , 232 , 233 , 234 , 235 , 236 , 237 , 238 , 239 , 240 , 241 , 242 , 243 , 244 , 245 , 246 , 247 , 248 , 249 , 250 , 251 , 252 , 253 , 254 , 255};
	int table_u2[256]     = {0 , 1 , 2 , 3 , 4 , 58 , 5 , 6 , 7 , 58 , 58 , 58 , 8 , 58 , 9 , 10 , 11 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 12 , 58 , 58 , 58 , 13 , 58 , 14 , 15 , 16 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 17 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 18 , 58 , 58 , 58 , 19 , 58 , 20 , 21 , 22 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 23 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 24 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 25 , 58 , 58 , 58 , 26 , 58 , 27 , 28 , 29 , 30 , 58 , 31 , 58 , 58 , 58 , 32 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 33 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 34 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 35 , 36 , 37 , 58 , 38 , 58 , 58 , 58 , 39 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 40 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 41 , 42 , 43 , 58 , 44 , 58 , 58 , 58 , 45 , 58 , 58 , 58 , 58 , 58 , 58 , 58 , 46 , 47 , 48 , 58 , 49 , 58 , 58 , 58 , 50 , 51 , 52 , 58 , 53 , 54 , 55 , 56 , 57};
	int table_ri[256]     = {0 , 1 , 1 , 2 , 1 , 3 , 2 , 4 , 1 , 5 , 3 , 6 , 2 , 7 , 4 , 8 , 1 , 9 , 5 , 10 , 3 , 11 , 6 , 12 , 2 , 13 , 7 , 14 , 4 , 15,8,16,1,5,9,13,5,17,10,18,3,17,11,19,6,20,12,21,2,10,13,22,7,23,14,24,4,18,15,25,8,26,16,27,1,3,5,7,9,11,13,15,5,17,17,20,10,23,18,26,3,11,17,23,11,28,19,29,6,19,20,30,12,29,21,31,2,6,10,14,13,19,22,25,7,20,23,30,14,30,24,32,4,12,18,24,15,29,25,33,8,21,26,32,16,31,27,34,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,5,13,17,18,17,19,20,21,10,22,23,24,18,25,26,27,3,7,11,15,17,20,23,26,11,23,28,29,19,30,29,31,6,14,19,25,20,30,30,32,12,24,29,33,21,32,31,34,2,4,6,8,10,12,14,16,13,18,19,21,22,24,25,27,7,15,20,26,23,29,30,31,14,25,30,32,24,33,32,34,4,8,12,16,18,21,24,27,15,26,29,31,25,32,33,34,8,16,21,27,26,31,32,34,16,27,31,34,27,34,34,35};	
	int table_riu2[256]   = {0 , 1 , 1 , 2 , 1 , 9 , 2 , 3 , 1 , 9 , 9 , 9 , 2 , 9 , 3 , 4 , 1 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 2 , 9 , 9 , 9 , 3 , 9 , 4 , 5 , 1 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 2 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 3 , 9 , 9 , 9 , 4 , 9 , 5 , 6 , 1 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 2 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 3 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 4 , 9 , 9 , 9 , 5 , 9 , 6 , 7 , 1 , 2 , 9 , 3 , 9 , 9 , 9 , 4 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 5 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 6 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 7 , 2 , 3 , 9 , 4 , 9 , 9 , 9 , 5 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 6 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 7 , 3 , 4 , 9 , 5 , 9 , 9 , 9 , 6 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 7 , 4 , 5 , 9 , 6 , 9 , 9 , 9 , 7 , 5 , 6 , 9 , 7 , 6 , 7 , 7 , 8};

	fftw_complex *work; 
	fftw_plan plan; 

	int i , j , p , l , m , v ;
	int ly, lx, nynx = ny*nx , vnynx , stride , coscale = 0 , nscale = options.nscale , nspyr = options.nspyr , improvedLBP = options.improvedLBP;
	int kyy = options.kyy , kxy = options.kxy , kyx = options.kyx , kxx = options.kxx  , maptable = options.maptable;
	int useFF = options.useFF;
	int norm_all = (int) options.norm[0] , norm_p = (int) options.norm[1] , d;

	double clamp = options.clamp , sum;
	double scaley, scalex;
	double maxfactor = 0.0 , ratio , tempx , tempy , temp;
	int deltay, deltax, sy , sx, origy, origx , offsety = 0 , offsetx = 0;
	int nygrady , nxgrady , nygradx , nxgradx , noconv ;
	int NBINS , Nbins;
	int co , co_p , co_totalp , Nbinsnscale , offset , indj , indl , indv;

	if(maptable == 0)
	{
		Nbins              = 256;
		NBINS              = 256;
	}
	else if(maptable == 1)
	{
		if(useFF)
		{
			Nbins          = 38;
			NBINS          = 59;
		}
		else
		{
			Nbins          = 59;
			NBINS          = 59;
		}
	}
	else if(maptable == 2)
	{
		Nbins              = 36;
		NBINS              = 36;
	}
	else if(maptable == 3)
	{
		Nbins              = 10;
		NBINS              = 10;
	}
	if(improvedLBP)
	{
		Nbins             *= 2;
		NBINS             *= 2;
	}
	Nbinsnscale            = Nbins*nscale;
	d                      = nH*dimcolor*Nbinsnscale;

	if(useFF)
	{
		work               = fftw_malloc(8*sizeof(fftw_complex)); 
		plan               = fftw_plan_dft_1d(8 , work, work , FFTW_FORWARD, FFTW_ESTIMATE); 
		fftw_execute(plan); 
		Htemp              = (double *) malloc(NBINS*sizeof(double));
	}

	table                  = (int *) malloc((256*(improvedLBP+1))*sizeof(int));

	if(maptable == 0)
	{
		for (l = 0 ; l < improvedLBP+1 ; l++)
		{
			v = l*256;
			for (m = 0 ; m < 256 ; m++)
			{
				table[m + v] = table_normal[m]+v;
			}
		}
	}
	else if(maptable == 1)
	{
		for (l = 0 ; l < improvedLBP+1 ; l++)
		{
			v = l*256;
			i = l*59;

			for (m = 0 ; m < 256 ; m++)
			{
				table[m + v] = table_u2[m] + i;
			}
		}
	}
	else if(maptable == 2)
	{
		for (l = 0 ; l < improvedLBP+1 ; l++)
		{
			v = l*256;
			i = l*36;
			for (m = 0 ; m < 256 ; m++)
			{
				table[m + v] = table_ri[m] + i;
			}
		}
	}
	else if(maptable == 3)
	{
		for (l = 0 ; l < improvedLBP+1 ; l++)
		{
			v = l*256;
			i = l*10;

			for (m = 0 ; m < 256 ; m++)
			{
				table[m + v] = table_riu2[m] + i;
			}
		}
	}

	mag         = (double *)malloc(nynx*sizeof(double));
	II          = (double *)malloc(nynx*sizeof(double));
	IItemp      = (double *)malloc(nynx*sizeof(double));

	if(((kyy == 1) && (kxy == 1) && (kyx == 1) && (kxx == 1)) )
	{
		noconv      = 1;
	}
	else
	{
		noconv      = 0;

		nygrady     = ny + kyy - 1;
		nxgrady     = nx + kxy - 1;
		nygradx     = ny + kyx - 1;
		nxgradx     = nx + kxx - 1;

		grady       = (double *)malloc(nynx*sizeof(double));
		gradx       = (double *)malloc(nynx*sizeof(double));

		tempgrady   = (double *)malloc(nygrady*nxgrady*sizeof(double));
		tempgradx   = (double *)malloc(nygradx*nxgradx*sizeof(double));
	}

	stride      = Nbins*nH*nscale;
	co          = 0;
	co_totalp   = 0;

	if(useFF)
	{
		for (v = 0 ; v < dimcolor ; v++)
		{
			vnynx    = v*nynx;
			indv     = v*stride;

			if(noconv )
			{
				for (i = 0 ; i < nynx ; i++)
				{
					mag[i] = I[i + vnynx];
				}
			}
			else
			{
				for(i = 0 ; i < nygrady*nxgrady ; i++)
				{
					tempgrady[i] = 0.0;
				}
				conv2(I + vnynx , kernely , ny , nx , kyy , kxy , nygrady , nxgrady , grady , tempgrady);

				for(i = 0 ; i < nygradx*nxgradx ; i++)
				{
					tempgradx[i] = 0.0;
				}

				conv2(I + vnynx , kernelx , ny , nx , kyx , kxx , nygradx , nxgradx , gradx , tempgradx);
				for(i = 0 ; i < nynx ; i++)
				{
					tempy  = grady[i];
					tempx  = gradx[i];
					temp   = sqrt(tempy*tempy + tempx*tempx);
					mag[i] = temp;
				}
			}

			MakeIntegralImage(mag , II , nx, ny , IItemp);

			for (p = 0 ; p < nspyr ; p++)
			{
				maxfactor   = max(maxfactor , spyr[p + 0]*spyr[p + nspyr]);
			}

			for (p = 0 ; p < nspyr ; p++)
			{
				scaley      = (spyr[p + nspyr*2]);
				ly          = (int) ( (1 - spyr[p + 0])/scaley + 1);
				deltay      = (int) (ny*scaley);
				sy          = (int) (ny*spyr[p + 0]);
				offsety     = max(0 , (int) ( floor(ny - ( (ly-1)*deltay + sy + 1)) ));

				scalex      = (spyr[p + nspyr*3]);
				lx          = (int) ( (1 - spyr[p + nspyr*1])/scalex + 1);
				deltax      = (int) (nx*scalex);
				sx          = (int) (nx*spyr[p + nspyr*1]);
				offsetx     = max(0 , (int) ( floor(nx - ( (lx-1)*deltax + sx + 1)) ));

				ratio       = maxfactor/(spyr[p + 0]*spyr[p + nspyr]);
				co_p        = 0;
				offset      = co_totalp*Nbinsnscale + indv;

				for(l = 0 ; l < lx ; l++) /* Loop shift on x-axis  */
				{
					origx  = offsetx + l*deltax ;
					for(m = 0 ; m < ly ; m++)   /* Loop shift on y-axis  */
					{
						origy     = offsety + m*deltay ;
						compute_hmblbpff(II , ny , nx , sy , sx , origy , origx , scale , nscale , ratio , H + coscale , NBINS , table , work , plan , Htemp , options);
						coscale  += Nbinsnscale;
						co_p++;
					}
				}

				/* Normalization per pyramid level */

				if(norm_p == 1)
				{
					for (l = 0 ; l < nscale ; l++)
					{
						indl      = l*Nbins + offset;
						sum       = 0.0;
						for(j = 0 ; j < co_p ; j++)
						{
							indj = j*Nbinsnscale + indl;
							for(i = indj ; i < (Nbins + indj) ; i++)
							{
								sum   += H[i];
							}
						}

						sum = 1.0/(sum + tiny);
						for(j = 0 ; j < co_p ; j++)
						{
							indj = j*Nbinsnscale + indl;
							for(i = indj ; i < (Nbins + indj) ; i++)
							{
								H[i] *= sum;
							}
						}
					}
				}
				if(norm_p == 2)
				{
					for (l = 0 ; l < nscale ; l++)
					{
						indl      = l*Nbins + offset;
						sum       = 0.0;
						for(j = 0 ; j < co_p ; j++)
						{
							indj = j*Nbinsnscale + indl;
							for(i = indj ; i < (Nbins + indj) ; i++)
							{
								temp   = H[i];
								sum   += temp*temp;
							}
						}

						sum = 1.0/sqrt(sum + verytiny);
						for(j = 0 ; j < co_p ; j++)
						{
							indj = j*Nbinsnscale + indl;
							for(i = indj ; i < (Nbins + indj) ; i++)
							{
								H[i] *= sum;
							}
						}
					}
				}
				if(norm_p == 3)
				{
					for (l = 0 ; l < nscale ; l++)
					{
						indl      = l*Nbins + offset;
						sum       = 0.0;
						for(j = 0 ; j < co_p ; j++)
						{
							indj = j*Nbinsnscale + indl;
							for(i = indj ; i < (Nbins + indj) ; i++)
							{
								sum   += H[i];
							}
						}
						sum = 1.0/(sum + tiny);
						for(j = 0 ; j < co_p ; j++)
						{
							indj = j*Nbinsnscale + indl;
							for(i = indj ; i < (Nbins + indj) ; i++)
							{
								H[i]   = sqrt(H[i]*sum);
							}
						}
					}
				}
				if(norm_p == 4)
				{
					for (l = 0 ; l < nscale ; l++)
					{
						indl      = l*Nbins + offset;
						sum       = 0.0;
						for(j = 0 ; j < co_p ; j++)
						{
							indj = j*Nbinsnscale + indl;
							for(i = indj ; i < (Nbins + indj) ; i++)
							{
								temp   = H[i];
								sum   += temp*temp;
							}
						}

						sum = 1.0/sqrt(sum + verytiny);
						for(j = 0 ; j < co_p ; j++)
						{
							indj = j*Nbinsnscale + indl;
							for(i = indj ; i < (Nbins + indj) ; i++)
							{
								H[i]   *= sum;
								if(H[i] > clamp)
								{
									H[i] = clamp;
								}
							}
						}

						sum       = 0.0;
						for(j = 0 ; j < co_p ; j++)
						{
							indj = j*Nbinsnscale + indl;
							for(i = indj ; i < (Nbins + indj) ; i++)
							{
								temp   = H[i];
								sum   += temp*temp;
							}
						}

						sum = 1.0/sqrt(sum + verytiny);
						for(j = 0 ; j < co_p ; j++)
						{
							indj = j*Nbinsnscale + indl;
							for(i = indj ; i < (Nbins + indj) ; i++)
							{
								H[i] *= sum;
							}
						}
					}
				}

				co_totalp       += co_p;
			}
		}
	}
	else
	{
		for (v = 0 ; v < dimcolor ; v++)
		{
			vnynx    = v*nynx;
			indv     = v*nscale*stride;

			if(noconv )
			{
				for (i = 0 ; i < nynx ; i++)
				{
					mag[i] = I[i + vnynx];
				}
			}
			else
			{
				for(i = 0 ; i < nygrady*nxgrady ; i++)
				{
					tempgrady[i] = 0.0;
				}
				conv2(I + vnynx , kernely , ny , nx , kyy , kxy , nygrady , nxgrady , grady , tempgrady);

				for(i = 0 ; i < nygradx*nxgradx ; i++)
				{
					tempgradx[i] = 0.0;
				}

				conv2(I + vnynx , kernelx , ny , nx , kyx , kxx , nygradx , nxgradx , gradx , tempgradx);

				for(i = 0 ; i < nynx ; i++)
				{
					tempy  = grady[i];
					tempx  = gradx[i];
					temp   = sqrt(tempy*tempy + tempx*tempx);
					mag[i] = temp;
				}
			}

			MakeIntegralImage(mag , II , nx, ny , IItemp);

			for (p = 0 ; p < nspyr ; p++)
			{
				maxfactor   = max(maxfactor , spyr[p + 0]*spyr[p + nspyr]);
			}

			for (p = 0 ; p < nspyr ; p++)
			{
				scaley      = (spyr[p + nspyr*2]);
				ly          = (int) ( (1 - spyr[p + 0])/scaley + 1);
				deltay      = (int) (ny*scaley);
				sy          = (int) (ny*spyr[p + 0]);
				offsety     = max(0 , (int) ( floor(ny - ( (ly-1)*deltay + sy + 1)) ));

				scalex      = (spyr[p + nspyr*3]);
				lx          = (int) ( (1 - spyr[p + nspyr*1])/scalex + 1);
				deltax      = (int) (nx*scalex);
				sx          = (int) (nx*spyr[p + nspyr*1]);
				offsetx     = max(0 , (int) ( floor(nx - ( (lx-1)*deltax + sx + 1)) ));

				ratio       = maxfactor/(spyr[p + 0]*spyr[p + nspyr]);
				co_p        = 0;
				offset      = co_totalp*Nbinsnscale + indv;

				for(l = 0 ; l < lx ; l++) /* Loop shift on x-axis */
				{
					origx  = offsetx + l*deltax ;
					for(m = 0 ; m < ly ; m++)   /* Loop shift on y-axis  */
					{
						origy     = offsety + m*deltay ;
						compute_hmblbp(II , ny , nx , sy , sx , origy , origx , scale , nscale , ratio , H + coscale , Nbins , table , options);
						coscale  += Nbinsnscale;
						co_p++;
					}
				}

				/* Normalization per pyramid level */

				if(norm_p == 1)
				{
					for (l = 0 ; l < nscale ; l++)
					{
						indl      = l*Nbins + offset;
						sum       = 0.0;
						for(j = 0 ; j < co_p ; j++)
						{
							indj = j*Nbinsnscale + indl;
							for(i = indj ; i < (Nbins + indj) ; i++)
							{
								sum   += H[i];
							}
						}

						sum = 1.0/(sum + tiny);
						for(j = 0 ; j < co_p ; j++)
						{
							indj = j*Nbinsnscale + indl;
							for(i = indj ; i < (Nbins + indj) ; i++)
							{
								H[i] *= sum;
							}
						}
					}
				}
				if(norm_p == 2)
				{
					for (l = 0 ; l < nscale ; l++)
					{
						indl      = l*Nbins + offset;
						sum       = 0.0;
						for(j = 0 ; j < co_p ; j++)
						{
							indj = j*Nbinsnscale + indl;
							for(i = indj ; i < (Nbins + indj) ; i++)
							{
								temp   = H[i];
								sum   += temp*temp;
							}
						}

						sum = 1.0/sqrt(sum + verytiny);
						for(j = 0 ; j < co_p ; j++)
						{
							indj = j*Nbinsnscale + indl;
							for(i = indj ; i < (Nbins + indj) ; i++)
							{
								H[i] *= sum;
							}
						}
					}
				}
				if(norm_p == 3)
				{
					for (l = 0 ; l < nscale ; l++)
					{
						indl      = l*Nbins + offset;
						sum       = 0.0;
						for(j = 0 ; j < co_p ; j++)
						{
							indj = j*Nbinsnscale + indl;
							for(i = indj ; i < (Nbins + indj) ; i++)
							{
								sum   += H[i];
							}
						}
						sum = 1.0/(sum + tiny);
						for(j = 0 ; j < co_p ; j++)
						{
							indj = j*Nbinsnscale + indl;
							for(i = indj ; i < (Nbins + indj) ; i++)
							{
								H[i]   = sqrt(H[i]*sum);
							}
						}
					}
				}
				if(norm_p == 4)
				{
					for (l = 0 ; l < nscale ; l++)
					{
						indl      = l*Nbins + offset;
						sum       = 0.0;
						for(j = 0 ; j < co_p ; j++)
						{
							indj = j*Nbinsnscale + indl;
							for(i = indj ; i < (Nbins + indj) ; i++)
							{
								temp   = H[i];
								sum   += temp*temp;
							}
						}

						sum = 1.0/sqrt(sum + verytiny);
						for(j = 0 ; j < co_p ; j++)
						{
							indj = j*Nbinsnscale + indl;
							for(i = indj ; i < (Nbins + indj) ; i++)
							{
								H[i]   *= sum;
								if(H[i] > clamp)
								{
									H[i] = clamp;
								}
							}
						}

						sum       = 0.0;
						for(j = 0 ; j < co_p ; j++)
						{
							indj = j*Nbinsnscale + indl;
							for(i = indj ; i < (Nbins + indj) ; i++)
							{
								temp   = H[i];
								sum   += temp*temp;
							}
						}

						sum = 1.0/sqrt(sum + verytiny);
						for(j = 0 ; j < co_p ; j++)
						{
							indj = j*Nbinsnscale + indl;
							for(i = indj ; i < (Nbins + indj) ; i++)
							{
								H[i] *= sum;
							}
						}
					}
				}

				co_totalp       += co_p;
			}
		}
	}

	/* Normalization for full descriptor (d x 1) */

	if(norm_all == 1)
	{
		sum       = 0.0;
		for(i = 0 ; i < d ; i++)
		{
			sum   += H[i];
		}
		sum = 1.0/(sum + tiny);
		for(i = 0 ; i < d ; i++)
		{
			H[i]   *= sum;
		}
	}
	if(norm_all == 2)
	{
		sum       = 0.0;
		for(i = 0 ; i < d ; i++)
		{
			temp   = H[i];
			sum   += temp*temp;
		}
		sum = 1.0/sqrt(sum + verytiny);
		for(i = 0 ; i < d ; i++)
		{
			H[i]   *= sum;
		}
	}
	if(norm_all == 3)
	{
		sum       = 0.0;
		for(i = 0 ; i < d ; i++)
		{
			sum   += H[i];
		}
		sum = 1.0/(sum + tiny);
		for(i = 0 ; i < d ; i++)
		{
			H[i]   = sqrt(H[i]*sum);
		}
	}
	if(norm_all == 4)
	{
		sum       = 0.0;
		for(i = 0 ; i < d ; i++)
		{
			temp   = H[i];
			sum   += temp*temp;
		}
		sum = 1.0/sqrt(sum + verytiny);
		for(i = 0 ; i < d ; i++)
		{
			H[i]   *= sum;
			if(H[i] > clamp)
			{
				H[i] = clamp;
			}
		}
		sum       = 0.0;
		for(i = 0 ; i < d ; i++)
		{
			temp   = H[i];
			sum   += temp*temp;
		}
		sum = 1.0/sqrt(sum + verytiny);
		for(i = 0 ; i < d ; i++)
		{
			H[i]   *= sum;
		}
	}


	free(mag);
	free(II);
	free(IItemp);
	free(table);

	if(useFF)
	{
		fftw_destroy_plan(plan); 
		fftw_free(work);
		free(Htemp);
	}
	if(!noconv )
	{
		free(tempgrady);
		free(tempgradx);
		free(grady);
		free(gradx);
	}
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
void compute_hmblbpff(double *II , int ny , int nx , int sy , int sx , int origy , int origx , double *scale , int nscale , double ratio , double *H , int NBINS , int *table , fftw_complex *work , fftw_plan plan , double *Htemp , struct opts options )
{
	int i , j , l , s , xc , yc , xnw , ynw , xse , yse , sNbins , co1 , co2;
	int norm = (int) options.norm[2];

	int improvedLBP = options.improvedLBP , rmextremebins = options.rmextremebins ;
	int currentscale ;
	int Nbins = 38;

	double clamp = options.clamp;
	double sum, temp , re , im;
	double Ac , tmpA , sumA;
	unsigned short int valF;

	if(improvedLBP)
	{
		Nbins      *= 2;
	}

	for (s = 0 ; s < nscale ; s++)
	{
		currentscale = (int) scale[s];
		sNbins       = s*Nbins;

		for (i = 0 ; i < NBINS ; i++)
		{
			Htemp[i] = 0.0;
		}

		for (xc = currentscale + origx ; xc <= sx - 2*currentscale + origx ; xc++)
		{
			for (yc = currentscale + origy ; yc <= sy - 2*currentscale + origy ; yc++)
			{
				xnw   = xc - currentscale;
				ynw   = yc - currentscale;
				xse   = xc + currentscale;
				yse   = yc + currentscale;

				Ac    = Area(II , xc  , yc  , currentscale , currentscale , ny);

				valF  = 0;
				sumA  = Ac;

				tmpA  = Area(II , xnw , ynw , currentscale , currentscale , ny);
				if(tmpA > Ac)
				{
					valF |= 0x01;
				}

				sumA += tmpA;

				tmpA  = Area(II , xc  , ynw , currentscale , currentscale , ny);
				if(tmpA > Ac)
				{
					valF |= 0x02;
				}

				sumA += tmpA;

				tmpA  = Area(II , xse , ynw , currentscale , currentscale , ny);
				if(tmpA > Ac)
				{
					valF |= 0x04;
				}

				sumA += tmpA;

				tmpA  = Area(II , xse , yc  , currentscale , currentscale , ny);

				if(tmpA > Ac)
				{
					valF |= 0x08;
				}

				sumA += tmpA;

				tmpA  = Area(II , xse , yse , currentscale , currentscale , ny);
				if(tmpA > Ac)
				{
					valF |= 0x10;
				}

				sumA += tmpA;

				tmpA  =  Area(II , xc  , yse , currentscale , currentscale , ny);
				if(tmpA > Ac)
				{
					valF |= 0x20;
				}
				sumA += tmpA;

				tmpA =  Area(II , xnw , yse , currentscale , currentscale , ny);
				if(tmpA > Ac)
				{
					valF |= 0x40;
				}
				sumA += tmpA;

				tmpA =  Area(II , xnw , yc  , currentscale , currentscale , ny);
				if(tmpA > Ac)
				{
					valF |= 0x80;
				}

				sumA += tmpA;

				if(improvedLBP)
				{
					if(sumA > 9*Ac)
					{
						valF |= 0x100; 
					}
				}
				
				Htemp[table[valF]]++;
			}
		}


		for(i = 0 ; i < NBINS ; i++)
		{
			Htemp[i]  *= ratio;
		}

		if(norm == 1)
		{
			sum       = 0.0;
			for(i = 0 ; i < NBINS ; i++)
			{
				sum   += Htemp[i];
			}

			sum = 1.0/(sum + tiny);
			for(i = 0 ; i < NBINS ; i++)
			{
				Htemp[i]   *= sum;
			}
		}
		if(norm == 2)
		{
			sum       = 0.0;
			for(i = 0 ; i < NBINS ; i++)
			{
				temp   = Htemp[i];

				sum   += temp*temp;
			}

			sum = 1.0/sqrt(sum + verytiny);
			for(i = 0 ; i < NBINS ; i++)
			{
				Htemp[i]   *= sum;
			}
		}
		if(norm == 3)
		{
			sum       = 0.0;
			for(i = 0 ; i < NBINS ; i++)
			{
				sum   += Htemp[i];
			}

			sum = 1.0/(sum + tiny);
			for(i = 0 ; i < NBINS ; i++)
			{
				H[i]   = sqrt(Htemp[i]*sum);
			}
		}
		if(norm == 4)
		{
			sum       = 0.0;
			for(i = 0 ; i < NBINS ; i++)
			{
				temp   = Htemp[i];
				sum   += temp*temp;
			}

			sum = 1.0/sqrt(sum + verytiny);
			for(i = 0 ; i < NBINS ; i++)
			{
				Htemp[i]   *= sum;

				if(Htemp[i] > clamp)
				{
					Htemp[i] = clamp;
				}
			}

			sum       = 0.0;
			for(i = 0 ; i < NBINS ; i++)
			{
				temp   = Htemp[i];
				sum   += temp*temp;
			}

			sum = 1.0/sqrt(sum + verytiny);
			for(i = 0 ; i < NBINS ; i++)
			{
				Htemp[i]   *= sum;
			}
		}

		if(rmextremebins == 1)
		{
			if(improvedLBP)
			{
				Htemp[0] = Htemp[NBINS-1] = Htemp[NBINS ] = Htemp[Nbins-1] = 0.0;
			}
			else
			{
				Htemp[0] = Htemp[NBINS-1 ] = 0.0;
			}
		}

		co1                = 0;
		co2                = sNbins;

		for (l = 0 ; l < (1+improvedLBP) ; l++)
		{
			for(i = 0 ; i < 7 ; i++)
			{
				for(j = 0 ; j < 8 ; j++) 
				{ 
					work[j][0]   = Htemp[j + co1]; 
					work[j][1]   = 0.0; 
				} 

				fftw_execute(plan); 

				for (j = 0 ; j < 5 ; j++) 
				{ 
					re           = work[j][0];
					im           = work[j][1];
					H[j + co2]   = sqrt(re*re + im*im);
				} 

				co1             += 8;
				co2             += 5;
			}
			for(i = 0 ; i < 3 ; i++)
			{
				H[co2+i]         = Htemp[co1+i];
			}
		}
	}
}

/*----------------------------------------------------------------------------------------------------------------------------------------- */
void compute_hmblbp(double *II , int ny , int nx , int sy , int sx , int origy , int origx , double *scale , int nscale , double ratio , double *H , int NBINS , int *table , struct opts options )
{

	int i , s , xc , yc , xnw , ynw , xse , yse , sNbins;
	int norm = (int) options.norm[2];

	int improvedLBP = options.improvedLBP , rmextremebins = options.rmextremebins ;
	int currentscale ;
	int Nbins = NBINS;

	double clamp = options.clamp;
	double sum, temp;
	double Ac , tmpA , sumA;

	unsigned short int valF;

	if(improvedLBP)
	{
		Nbins      = NBINS/2;
	}

	for (s = 0 ; s < nscale ; s++)
	{
		currentscale = (int) scale[s];
		sNbins       = s*NBINS;

		for (xc = currentscale + origx ; xc <= sx - 2*currentscale + origx  ; xc++)
		{
			for (yc = currentscale + origy ; yc <= sy - 2*currentscale + origy  ; yc++)
			{
				xnw   = xc - currentscale;
				ynw   = yc - currentscale;
				xse   = xc + currentscale;
				yse   = yc + currentscale;

				Ac    = Area(II , xc  , yc  , currentscale , currentscale , ny);

				valF  = 0;
				sumA  = Ac;

				tmpA  = Area(II , xnw , ynw , currentscale , currentscale , ny);
				if(tmpA > Ac)
				{
					valF |= 0x01;
				}

				sumA += tmpA;

				tmpA  = Area(II , xc  , ynw , currentscale , currentscale , ny);
				if(tmpA > Ac)
				{
					valF |= 0x02;
				}

				sumA += tmpA;

				tmpA  = Area(II , xse , ynw , currentscale , currentscale , ny);
				if(tmpA > Ac)
				{
					valF |= 0x04;
				}

				sumA += tmpA;

				tmpA  = Area(II , xse , yc  , currentscale , currentscale , ny);
				if(tmpA > Ac)
				{
					valF |= 0x08;
				}

				sumA += tmpA;

				tmpA  = Area(II , xse , yse , currentscale , currentscale , ny);
				if(tmpA > Ac)
				{
					valF |= 0x10;
				}

				sumA += tmpA;

				tmpA  =  Area(II , xc  , yse , currentscale , currentscale , ny);
				if(tmpA > Ac)
				{
					valF |= 0x20;
				}
				sumA += tmpA;

				tmpA =  Area(II , xnw , yse , currentscale , currentscale , ny);
				if(tmpA > Ac)
				{
					valF |= 0x40;
				}
				sumA += tmpA;

				tmpA =  Area(II , xnw , yc  , currentscale , currentscale , ny);
				if(tmpA > Ac)
				{
					valF |= 0x80;
				}

				sumA += tmpA;

				if(improvedLBP)
				{
					if(sumA > 9*Ac)
					{
						valF |= 0x100; 
					}
				}
				H[table[valF] + sNbins]++;
			}
		}

		for(i = sNbins ; i < (NBINS + sNbins) ; i++)
		{
			H[i]  *= ratio;
		}

		if(norm == 1)
		{
			sum       = 0.0;
			for(i = sNbins ; i < (NBINS + sNbins) ; i++)
			{
				sum   += H[i];
			}

			sum = 1.0/(sum + tiny);
			for(i = sNbins ; i < (NBINS + sNbins) ; i++)
			{
				H[i]   *= sum;
			}
		}
		if(norm == 2)
		{
			sum       = 0.0;
			for(i = sNbins ; i < (NBINS + sNbins) ; i++)
			{
				temp   = H[i];
				sum   += temp*temp;
			}

			sum = 1.0/sqrt(sum + verytiny);
			for(i = sNbins ; i < (NBINS + sNbins) ; i++)
			{
				H[i]   *= sum;
			}
		}

		if(norm == 3)
		{
			sum       = 0.0;
			for(i = sNbins ; i < (NBINS + sNbins) ; i++)
			{
				sum   += H[i];
			}

			sum = 1.0/(sum + tiny);
			for(i = sNbins ; i < (NBINS + sNbins) ; i++)
			{
				H[i]   = sqrt(H[i]*sum);
			}
		}

		if(norm == 4)
		{
			sum       = 0.0;
			for(i = sNbins ; i < (NBINS + sNbins) ; i++)
			{
				temp   = H[i];
				sum   += temp*temp;
			}

			sum = 1.0/sqrt(sum + verytiny);
			for(i = sNbins ; i < (NBINS + sNbins) ; i++)
			{
				H[i]   *= sum;
				if(H[i] > clamp)
				{
					H[i] = clamp;
				}
			}

			sum       = 0.0;
			for(i = sNbins ; i < (NBINS + sNbins) ; i++)
			{
				temp   = H[i];
				sum   += temp*temp;
			}

			sum = 1.0/sqrt(sum + verytiny);
			for(i = sNbins ; i < (NBINS + sNbins) ; i++)
			{
				H[i]   *= sum;
			}
		}

		if(rmextremebins == 1)
		{
			if(improvedLBP)
			{
				H[0 + sNbins] = H[Nbins-1 + sNbins] = H[Nbins + sNbins] = H[NBINS-1 + sNbins] = 0.0;
			}
			else
			{
				H[0 + sNbins] = H[Nbins-1 + sNbins] = 0.0;
			}
		}
	}
}

/*----------------------------------------------------------------------------------------------------------------------------------------- */
int	number_histo_lbp(double *spyr , int nspyr)
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

void MakeIntegralImage(double *pIn, double *pOut, int iXmax, int iYmax , double *pTemp)

{
	/* Variable declaration */
	int x , y , indx = 0;

	for(x=0 ; x<iXmax ; x++)
	{
		pTemp[indx]     = pIn[indx];
		indx           += iYmax;
	}

	for(y = 1 ; y<iYmax ; y++)
	{
		pTemp[y]        = pTemp[y - 1] + pIn[y];
	}

	pOut[0]             = pIn[0];
	indx                = iYmax;

	for(x=1 ; x<iXmax ; x++)
	{
		pOut[indx]      = pOut[indx - iYmax] + pTemp[indx];
		indx           += iYmax;
	}
	for(y = 1 ; y<iYmax ; y++)
	{
		pOut[y]         = pOut[y - 1] + pIn[y];
	}

	/* Calculate integral image */

	indx                = iYmax;
	for(x = 1 ; x < iXmax ; x++)
	{
		for(y = 1 ; y < iYmax ; y++)
		{
			pTemp[y + indx]    = pTemp[y - 1 + indx] + pIn[y + indx];
			pOut[y + indx]     = pOut[y + indx - iYmax] + pTemp[y + indx];
		}
		indx += iYmax;
	}
}

/*----------------------------------------------------------------------------------------------------------------------------------------------*/

double Area(double *II , int x , int y , int w , int h , int Ny)
{
	int h1 = h-1 , w1 = w-1 , x1 = x-1, y1 = y-1;

	if( (x == 0) && (y==0))
	{
		return (II[h1 + w1*Ny]);
	}
	if( (x==0) ) 
	{
		return(II[(y+h1) + w1*Ny] - II[y1 + w1*Ny]);
	}
	if( (y==0) ) 
	{
		return(II[h1 + (x+w1)*Ny] - II[h1 + x1*Ny]);
	}
	else
	{
		return (II[(y+h1) + (x+w1)*Ny] - (II[y1 + (x+w1)*Ny] + II[(y+h1) + x1*Ny]) + II[y1 + x1*Ny]);
	}
}

/*----------------------------------------------------------------------------------------------------------------------------------------- */
void conv2(double *A, double *B, int nyA , int nxA , int nyB , int nxB , int nyD , int nxD , double *C , double *D)
{

	int p , q , index; 
	int lengthA = nyA*nxA , lengthB = nyB*nxB , lengthD = nyD*nxD , nyA1 = nyA - 1 , nyB1 = nyB - 1;
	int nyAtemp , nxAtemp ,  nyBtemp , nxBtemp , nyDtemp , nxDtemp;
	int nytemp = (int)ceil((1.0 + nyB)*0.5) - 1 , nxtemp = (int)ceil((1.0 + nxB)*0.5) - 1;


	for( p = 0 ; p < lengthD ; p++)
	{
		D[p] = 0.0;
	}

	nyAtemp  = -1;
	nxAtemp  = 0;

	for (p = 0 ; p < lengthA ; p++) 
	{
		nyAtemp  += 1;
		if (nyAtemp > nyA1)
		{ 
			nyAtemp    = 0; 
			nxAtemp   += 1; 
		}

		nyBtemp  = -1;
		nxBtemp  = 0;

		for (q = 0 ; q < lengthB ; q++) 
		{
			nyBtemp  += 1;
			if (nyBtemp > nyB1)
			{ 
				nyBtemp    = 0; 
				nxBtemp   += 1; 				
			}

			index           = nyAtemp + nyBtemp + (nxAtemp + nxBtemp)*nyD;
			D[index]       += A[p]*B[q];
		}
	}	

	nyDtemp = nytemp - 1;	 
	nxDtemp = nxtemp;

	nyA1   += nytemp;

	for (p = 0 ; p < lengthA ; p++) 
	{
		nyDtemp  += 1;

		if(nyDtemp > nyA1)
		{
			nyDtemp  = nytemp;
			nxDtemp += 1;
		}
		index     = nyDtemp + nxDtemp*nyD;
		C[p]      = D[index];
	}
}

/*--------------------------------------------------------------------------------------------------------------------------------------------*/
void rgb2gray(unsigned char *rgb , int Ny , int Nx , double *gray)
{
	/* 
	T    = inv([1.0 0.956 0.621; 1.0 -0.272 -0.647; 1.0 -1.106 1.703]);
	vect = T(1 , :);
	*/

	int i , NyNx = Ny*Nx , NyNx2 = 2*NyNx;

	for (i = 0 ; i  < NyNx ; i++)
	{
		gray[i]   = 0.298936021293776*rgb[i] + 0.587043074451121*rgb[i + NyNx] + 0.114020904255103*rgb[i + NyNx2];
	}
}

/*--------------------------------------------------------------------------------------------------------------------------------------------*/
void rgb2nrgb(unsigned char *rgb , int Ny , int Nx , double *nrgb)

{
	int i , NyNx = Ny*Nx , NyNx2 = 2*NyNx ;
	double r , g , b , cte;

	for (i = 0 ; i  < NyNx ; i++)
	{
		r                = (double)rgb[i];
		g                = (double)rgb[i + NyNx];
		b                = (double)rgb[i + NyNx2];

		cte              = (r + g + b);
		if(cte != 0.0)
		{
			cte          = 1.0/cte;
		}
		else
		{
			cte          = 1.0;
		}

		nrgb[i]          = r*cte;
		nrgb[i + NyNx]   = g*cte;
		nrgb[i + NyNx2]  = b*cte;
	}
}

/*--------------------------------------------------------------------------------------------------------------------------------------------*/
void rgb2opponent(unsigned char *rgb , int Ny , int Nx , double *opponent)

{
	int i , NyNx = Ny*Nx , NyNx2 = 2*NyNx ;
	double r , g , b ;

	for (i = 0 ; i  < NyNx ; i++)
	{
		r                    = (double)rgb[i];
		g                    = (double)rgb[i + NyNx];
		b                    = (double)rgb[i + NyNx2];

		opponent[i]          = (r - g)*invsqrt2;
		opponent[i + NyNx]   = (r + g - 2*b)*invsqrt6;
		opponent[i + NyNx2]  = (r + g + b)*invsqrt3;
	}
}


/*--------------------------------------------------------------------------------------------------------------------------------------------*/
void rgb2nopponent(unsigned char *rgb , int Ny , int Nx , double *nopponent)
{
	int i , NyNx = Ny*Nx , NyNx2 = 2*NyNx;
	double r , g , b , cte ;

	for (i = 0 ; i  < NyNx ; i++)
	{
		r                     = (double)rgb[i];
		g                     = (double)rgb[i + NyNx];
		b                     = (double)rgb[i + NyNx2];

		cte                   = (r + g + b);
		if(cte != 0.0)
		{
			cte               = 1.0/(cte*invsqrt2);
		}
		else
		{
			cte               = 1.0;
		}

		nopponent[i]          = sqrt3*(r - g)*cte;
		nopponent[i + NyNx]   = (r + g - 2*b)*cte;
	}
}

/*--------------------------------------------------------------------------------------------------------------------------------------------*/
void rgb2hue(unsigned char *rgb , int Ny , int Nx , double *hue)
{
	int i , NyNx = Ny*Nx , NyNx2 = 2*NyNx ;
	double r , g , b ;

	for (i = 0 ; i  < NyNx ; i++)
	{
		r         = (double)rgb[i];
		g         = (double)rgb[i + NyNx];
		b         = (double)rgb[i + NyNx2];
		hue[i]    = atan(sqrt3*(r - g)/(r + g - 2*b + verytiny));
	}
}
/*--------------------------------------------------------------------------------------------------------------------------------------------*/

