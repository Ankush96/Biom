
/*

Multi-Level Histograms of Color Multi-Scale Local Derivative Pattern with Spatial Pyramid.

Usage
------

H = mlhmsldp_spyr(I , [options] );


Inputs
------

I                              Input image (ny x nx x [3]) in UINT8 format. 
options 
       spyr                    Spatial Pyramid (nspyr x 4) (default [1 , 1 , 1 , 1] with nspyr = 1)
                               where spyr(i,1) is the ratio of ny in y axis of the blocks at level i (by(i) = spyr(i,1)*ny)
                               where spyr(i,2) is the ratio of nx in x axis of the blocks at level i (bx(i) = spyr(i,3)*nx)
                               where spyr(i,3) is the ratio of ny in y axis of the shifting at level i (deltay(i) = spyr(i,2)*ny)
                               where spyr(i,4) is the ratio of nx in x axis of the shifting at level i (deltax(i) = spyr(i,4)*nx)

      scale                    Multi-Scale vector (1 x nscale) (default scale = 1) where scale(i) = s is the size's factor to apply to each 9 blocks
                               in the LBP computation, i = 1,...,nscale

      n                        Derivative order of ldp (n > 1, default n = 2)

      ldporient                Orientation for ldp computation (1 x norient), where norien <= 4 and ldporient(i) = {0,1,2,3}
	                           ldporient(i) = 0 <=> (I(x_1+1,y_1)-I(x_1,y_1))*(I(x_0+1,y_0)-I(x_0,y_0))< 0
	                           ldporient(i) = 1 <=> (I(x_1+1,y_1+1)-I(x_1,y_1))*(I(x_0+1,y_0+1)-I(x_0,y_0))< 0
	                           ldporient(i) = 2 <=> (I(x_1,y_1+1)-I(x_1,y_1))*(I(x_0,y_0+1)-I(x_0,y_0))< 0
	                           ldporient(i) = 3 <=> (I(x_1-1,y_1+1)-I(x_1,y_1))*(I(x_0-1,y_0+1)-I(x_0,y_0))< 0
	                           (default ldporient = [0 , 1 , 2 , 3])

      color                    0 : force gray-scale (dimcolor = 1, default), 1 : RGB-LDP (dimcolor = 3), 2 : nRGB-LDP (dimcolor = 3), 3 : Opponent-LDP (dimcolor = 3), 
                               4 : nOpponent-LDP (dimcolor = 2), 5 : Hue-LDP (dimcolor = 1)


      improvedLDP              0 for default 8 bits encoding, 1 for 9 bits encoding (8 + central area)

      kernelx                  Kernel in x-direction for computing the gradient (default kernelx = [1])
      kernely                  Kernel in y-direction for computing the gradient (default kernely = [1])
                               If numel(kernelx) ~1 || numel(kernely) ~1, compute the magnitude, i.e. sqrt(dx*dx + dy*dy)
                               For example if kernelx = [1 2 1 ; 0 0 0 ; -1 -2 -1] and kernely = kernelx', this is equivalent to compute filter
                               the image with a sobel filter.

      rmextremebins            Force to zero bin = {0 , 255} if  rmextremebins = 1 (default rmextremebins = 1)

 	  norm                     Normalization vector (1 x 3) : [for all subwindows, for each subwindows of a pyramid level, for each subwindows]
                               norm = 0 <=> no normalization, norm = 1 <=> v=v/(sum(v)+epsi), norm = 2 <=> v=v/sqrt(sum(v²)+epsi²),
	                           norm = 3 <=> v=sqrt(v/(sum(v)+epsi)) , norm = 4 <=> L2-clamped (default norm = [0 , 0 , 4])

      clamp                    Clamping value (default clamp = 0.2)


Output
------

H                              mlhmsldp_spyr features (nbin*nH*nscale*dimcolor x 1) in double format where nbin = 256*(improvedLDP+1) and nH is the total number of subwindows defined
                               by the spatial pyramid spyr, i.e. nH = sum(floor(((1 - spyr(:,1))./(spyr(:,3)) + 1)).*floor((1 - spyr(:,2))./(spyr(:,4)) + 1)).

To compile
----------


mex  -g -output mlhmsldp_spyr.dll mlhmsldp_spyr.c

mex  -f mexopts_intel10.bat -output mlhmsldp_spyr.dll mlhmsldp_spyr.c


I                     = imread('image_0001.jpg');

options.spyr          = [1 , 1 , 1 , 1 ; 1/2 , 1/2 , 1/4 , 1/4];
options.scale         = [1  , 3 , 5 ];
options.ldporient     = [1 , 3];
options.n             = 3;
options.color         = 3;
options.rmextremebins = 1;
options.norm          = [0 , 4 , 0];
options.clamp         = 0.2;

nS                    = length(options.scale)*sum(floor(((1 - options.spyr(:,1))./(options.spyr(:,3)) + 1)).*floor((1 - options.spyr(:,2))./(options.spyr(:,4)) + 1));

H                     = mlhmsldp_spyr(I , options );



Example 1
---------

I                     = imread('02769_Right_StudentOffice.jpeg');

options.spyr          = [1 , 1 , 1 , 1 ; 1/2 , 1/2 , 1/4 , 1/4];
options.scale         = [1 , 2 , 3 , 4 ];

options.n             = 2;
options.ldporient     = [0 , 1 , 2 , 3];
options.rmextremebins = 1;
options.color         = 2;
options.norm          = [0 , 0 , 4];
options.clamp         = 0.2;


nH                    = length(options.ldporient)*length(options.scale)*sum(floor(((1 - options.spyr(:,1))./(options.spyr(:,3)) + 1)).*floor((1 - options.spyr(:,2))./(options.spyr(:,4)) + 1));

tic,H                 = mlhmsldp_spyr(I , options );,toc

figure(1)
plot(H)

figure(2)
imagesc(reshape(H , 256 , nH) )



Example 2
---------

I                     = imread('image_0001.jpg');

options.spyr          = [1 , 1 , 1 , 1 ; 1/2 , 1/2 , 1/4 , 1/4];
options.scale         = [1 , 2 , 3 , 4];
options.n             = 2;
options.rmextremebins = 1;
options.color         = 0;
options.norm          = [0 , 0 , 4];
options.clamp         = 0.2;




tic,H                 = mlhmsldp_spyr(I , options );,toc
figure(1)
plot(H)




Example 3
---------

I                     = imread('02769_Right_StudentOffice.jpeg');

options.spyr          = [1 , 1 , 1 , 1 ; 1/2 , 1/2 , 1/4 , 1/4];
options.scale         = [1 , 2];
options.ldporient     = [0 , 1 , 2 , 3];
options.kernelx       = [1 2 1 ; 0 0 0 ; -1 -2 -1];  %sobel mask
options.kernely       = [1 0 -1 ; 2 0 -2 ; 1 0 -1];  %sobel mask
options.improvedLDP   = 1;
options.color         = 5;
options.rmextremebins = 1;
options.norm          = [0 , 0 , 4];
options.clamp         = 0.2;



nH                    = length(options.ldporient)*length(options.scale)*sum(floor(((1 - options.spyr(:,1))./(options.spyr(:,3)) + 1)).*floor((1 - options.spyr(:,2))./(options.spyr(:,4)) + 1));


tic,H                 = mlhmsldp_spyr(I , options );,toc
figure(1)
plot(reshape(H , 256*(1+options.improvedLDP) , nH) )
figure(2)
imagesc(reshape(H , 256*(1+options.improvedLDP) , nH) )




Author : Sébastien PARIS : sebastien.paris@lsis.org
-------  Date : 09/06/2010



Reference : [1] Baochang Zhang, Yongsheng Gao, Sanqiang Zhao, and Jianzhuang Liu, 
---------       "Local Derivative Pattern Versus Local BinaryPattern: Face Recognition With High-Order Local Pattern Descriptor"
                IEEE TRANSACTIONS ON IMAGE PROCESSING, VOL. 19, NO. 2, FEBRUARY 2010


Changelog : - add extra option for normalization
----------

*/


#include <time.h>
#include <math.h>
#include <mex.h>


#define Nbins 256
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
	int     nscale;
	double  *spyr;
	int     nspyr;
	double *kernelx;
	int    kyx;
	int    kxx;
	double *kernely;
	int    kyy;
	int    kxy;
	int    n;
	int    *ldporient;
	int    norient;
	int    color;
	int    improvedLDP;
	int    rmextremebins;
	double *norm;
	double clamp;
};


/*-------------------------------------------------------------------------------------------------------------- */

/* Function prototypes */

int	number_histo_ldp(double * , int);
void rgb2gray(unsigned char * , int , int , double *);
void rgb2nrgb(unsigned char * , int , int , double *);
void rgb2opponent(unsigned char * , int , int , double *);
void rgb2nopponent(unsigned char * , int , int , double *);
void rgb2hue(unsigned char * , int , int , double *);

void MakeIntegralImage(double *, double *, int , int , double *);
double ld(double * , int , int , int , int , int , int );
double Area(double * , int , int , int , int , int );
void conv2(double *, double *, int  , int , int  , int , int , int , double * , double * );
void compute_hmbldp(double *, int , int , int , int , int  , int  , double * , int , double , double * , struct opts  );
void mlhmsldp_spyr(double * , double * , int  , int   , int , int , struct opts  );

/*-------------------------------------------------------------------------------------------------------------- */

#ifdef MATLAB_MEX_FILE
void mexFunction( int nlhs, mxArray *plhs[] , int nrhs, const mxArray *prhs[] )
{  
	unsigned char *im;
	const int *dimim;
	double *I;
	double *H;
	double	kernelx_default[1]   = {1};
	double	kernely_default[1]   = {1};
	double  norm_default[3]      = {0 , 0 , 4};
	int     ldporient_default[4] = {0,1,2,3};
	int i , ny , nx , nynx , dimcolor = 1 , nH = 1 , npt_ker = 1 , npt_ldp = 4;
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
	options.n             = 2;
	options.norient       = 4;
	options.color         = 0;
	options.improvedLDP   = 0;
	options.rmextremebins = 1;
	options.clamp         = 0.2;

	/* Input 1  */

	if( (mxGetNumberOfDimensions(prhs[0]) > 3) || !mxIsUint8(prhs[0]) )
	{
		mexErrMsgTxt("I must be (ny x nx x [3]) in UINT8 format");
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

		mxtemp                            = mxGetField(prhs[1] , 0 , "n");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			tempint                       = (int) tmp[0];
			if( (tempint < 2) )
			{
				mexPrintf("n > 1, force to 2\n");	
				options.n                = 2;
			}
			else
			{
				options.n                = tempint;
			}
		}

		mxtemp                            = mxGetField( prhs[1], 0, "ldporient" );
		if(mxtemp != NULL)
		{
			options.norient               = (int) mxGetN(mxtemp);
			options.ldporient             = (int *)mxMalloc(options.norient*sizeof(int));
			tmp                           = mxGetPr(mxtemp);

			for(i = 0 ; i < options.norient ; i++)
			{
				tempint                     = (int)tmp[i];
				if((tempint < 0) || (tempint > 3))
				{
					mexErrMsgTxt("ldporient vector must be in {0,1,2,3}");
				}
				else
				{
				options.ldporient[i]        = tempint;
				}
			}	
		}
		else
		{
			options.ldporient               = (int *)mxMalloc(npt_ldp*sizeof(int));
			for(i = 0 ; i < npt_ldp ; i++)
			{
				options.ldporient[i]        = ldporient_default[i];
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

		mxtemp                            = mxGetField(prhs[1] , 0 , "improvedLDP");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			tempint                       = (int) tmp[0];
			if( (tempint < 0) || (tempint > 1))
			{
				mexPrintf("improvedLDP = {0,1}, force to 0\n");	
				options.improvedLDP      = 0;
			}
			else
			{
				options.improvedLDP      = tempint;
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

		options.ldporient             = (int *) mxMalloc(npt_ldp*sizeof(int));

		for(i = 0 ; i < npt_ldp ; i++)
		{
			options.ldporient[i]         = ldporient_default[i];
		}
		options.norm                  = (double *)mxMalloc(3*sizeof(double));
		for(i = 0 ; i < 3 ; i++)
		{
			options.norm[i]           = norm_default[i];
		}	
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

	nH                 = number_histo_ldp(options.spyr , options.nspyr);

	/*----------------------- Outputs -------------------------------*/

	plhs[0]            =  mxCreateDoubleMatrix((1+options.improvedLDP)*options.norient*Nbins*nH*dimcolor*options.nscale , 1 , mxREAL);
	H                  =  mxGetPr(plhs[0]);

	/*------------------------ Main Call ----------------------------*/

	mlhmsldp_spyr(I , H  ,  ny , nx  , nH , dimcolor , options );

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

		if ( (mxGetField( prhs[1] , 0 , "ldporient" )) == NULL )
		{
			mxFree(options.ldporient);
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
		mxFree(options.ldporient);
		mxFree(options.norm);
	}
	mxFree(I);
}

#else


#endif

/*----------------------------------------------------------------------------------------------------------------------------------------- */
void mlhmsldp_spyr(double *I , double *H , int ny , int nx , int nH , int dimcolor , struct opts options  )
{
	double *kernelx = options.kernelx , *kernely = options.kernely;
	int i , j , p , l , m , v;
	int ly, lx,  nynx = ny*nx  , offsetH = 0 , nscale = options.nscale , nspyr = options.nspyr , NBINS = Nbins;
	int kyy = options.kyy , kxy = options.kxy , kyx = options.kyx , kxx = options.kxx ;
	int norient = options.norient , improvedLDP = options.improvedLDP;
	int norm_all = (int) options.norm[0] , norm_p = (int) options.norm[1] , d;
	int co , co_p , co_totalp , offset , indj , indl , indv;
	int stride , nhisto;

	double clamp = options.clamp , sum;
	double scaley, scalex;
	double maxfactor = 0.0 , ratio , tempx , tempy , temp ;
	int deltay, deltax, sy , sx, origy, origx , offsety = 0 , offsetx = 0;
	int nygrady , nxgrady , nygradx , nxgradx , noconv , vnynx , stepH;
	double *II , *IItemp;
	double *spyr = options.spyr , *scale = options.scale;
	double *grady, *gradx , *tempgrady , *tempgradx , *mag;

	if(improvedLDP)
	{
		NBINS      = 2*Nbins;
	}

	nhisto      = NBINS*norient;
	stepH       = nscale*nhisto;
	stride      = nH*stepH;
	d           = dimcolor*stride;

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

	co          = 0;
	co_totalp   = 0;


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
			offset      = co_totalp*stepH + indv;

			for(l = 0 ; l < lx ; l++) /* Loop shift on x-axis  */
			{
				origx  = offsetx + l*deltax ;
				for(m = 0 ; m < ly ; m++)   /* Loop shift on y-axis  */
				{
					origy     = offsety + m*deltay ;
					compute_hmbldp(II , ny , nx , sy , sx , origy , origx , scale , nscale , ratio , H + offsetH , options);
					offsetH  += stepH;
					co_p++;
				}
			}

				/* Normalization per pyramid level */

			if(norm_p == 1)
			{
				for (l = 0 ; l < nscale ; l++)
				{
					indl      = l*nhisto + offset;
					sum       = 0.0;
					for(j = 0 ; j < co_p ; j++)
					{
						indj = j*stepH + indl;
						for(i = indj ; i < (nhisto + indj) ; i++)
						{
							sum   += H[i];
						}
					}
					sum = 1.0/(sum + tiny);
					for(j = 0 ; j < co_p ; j++)
					{
						indj = j*stepH + indl;
						for(i = indj ; i < (nhisto + indj) ; i++)
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
					indl      = l*nhisto + offset;
					sum       = 0.0;
					for(j = 0 ; j < co_p ; j++)
					{
						indj = j*stepH + indl;
						for(i = indj ; i < (nhisto + indj) ; i++)
						{
							temp   = H[i];
							sum   += temp*temp;
						}
					}
					sum = 1.0/sqrt(sum + verytiny);
					for(j = 0 ; j < co_p ; j++)
					{
						indj = j*stepH + indl;
						for(i = indj ; i < (nhisto + indj) ; i++)
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
					indl      = l*nhisto + offset;
					sum       = 0.0;
					for(j = 0 ; j < co_p ; j++)
					{
						indj = j*stepH + indl;
						for(i = indj ; i < (nhisto + indj) ; i++)
						{
							sum   += H[i];
						}
					}
					sum = 1.0/(sum + tiny);
					for(j = 0 ; j < co_p ; j++)
					{
						indj = j*stepH + indl;
						for(i = indj ; i < (nhisto + indj) ; i++)
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
					indl      = l*nhisto + offset;
					sum       = 0.0;
					for(j = 0 ; j < co_p ; j++)
					{
						indj = j*stepH + indl;
						for(i = indj ; i < (nhisto + indj) ; i++)
						{
							temp   = H[i];
							sum   += temp*temp;
						}
					}
					sum = 1.0/sqrt(sum + verytiny);
					for(j = 0 ; j < co_p ; j++)
					{
						indj = j*stepH + indl;
						for(i = indj ; i < (nhisto + indj) ; i++)
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
						indj = j*stepH + indl;
						for(i = indj ; i < (nhisto + indj) ; i++)
						{
							temp   = H[i];
							sum   += temp*temp;
						}
					}
					sum = 1.0/sqrt(sum + verytiny);
					for(j = 0 ; j < co_p ; j++)
					{
						indj = j*stepH + indl;
						for(i = indj ; i < (nhisto + indj) ; i++)
						{
							H[i] *= sum;
						}
					}
				}
			}
			co_totalp       += co_p;
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

	if(!noconv )
	{
        free(tempgrady);
		free(tempgradx);
		free(grady);
		free(gradx);
	}
}

/*----------------------------------------------------------------------------------------------------------------------------------------- */
void compute_hmbldp(double *II , int ny , int nx , int sy , int sx , int origy , int origx , double *scale , int nscale , double ratio , double *H , struct opts options )
{
	int i , s , a , xc , yc , xnw , ynw , xse , yse  , sNbins ;
	int norm = (int) options.norm[2], improvedLDP = options.improvedLDP , rmextremebins = options.rmextremebins, n = options.n , n1 = n - 1;
	int norient = options.norient , *ldporient =options.ldporient , orient;
	int currentscale , NBINS = Nbins;
	int startxc, startyc, endxc, endyc;
	double clamp = options.clamp;
	double Ac , tmpA , sumA;
	double sum, temp;
	unsigned short int valF;

	if(improvedLDP)
	{
		NBINS      = 2*Nbins;
	}

	sNbins        = 0;

	for (s = 0 ; s < nscale ; s++)
	{
		currentscale = (int) scale[s];
		for(a = 0 ; a < norient ; a++)
		{
			orient = ldporient[a];
			if(orient == 0)
			{
				startxc = 0;
				endxc   = n1;
				startyc = 0;
				endyc   = 0;
			}
			else if(orient == 1)
			{
				startxc = 0;
				endxc   = n1;
				startyc = 0;
				endyc   = n1;
			}
			else if(orient == 2)
			{
				startxc = 0;
				endxc   = 0;
				startyc = 0;
				endyc   = n1;
			}
			else if(orient == 3)
			{
				startxc = n1;
				endxc   = 0;
				startyc = 0;
				endyc   = n1;
			}

			for (xc = (1 + startxc)*currentscale + origx ; xc <= sx - (2 + endxc)*currentscale + origx ; xc++)
			{
				for (yc = (1 + startyc)*currentscale + origy ; yc <= sy - (2 + endyc)*currentscale + origy ; yc++)
				{
					xnw   = xc - currentscale;
					ynw   = yc - currentscale;
					xse   = xc + currentscale;
					yse   = yc + currentscale;

					Ac    = ld(II , xc  , yc  , currentscale , n1 , orient , ny);

					valF  = 0;
					sumA  = Ac;

					tmpA  = ld(II , xnw , ynw , currentscale , n1 , orient , ny);
					if(tmpA*Ac <= 0)
					{
						valF |= 0x01;
					}
					sumA += tmpA;

					tmpA  = ld(II , xc  , ynw , currentscale , n1 , orient , ny);
					if(tmpA*Ac <= 0)
					{
						valF |= 0x02;
					}
					sumA += tmpA;

					tmpA  = ld(II , xse , ynw , currentscale , n1 , orient , ny);
					if(tmpA*Ac <= 0)
					{
						valF |= 0x04;
					}
					sumA += tmpA;

					tmpA  = ld(II , xse , yc  , currentscale , n1 , orient , ny);
					if(tmpA*Ac <= 0)
					{
						valF |= 0x08;
					}
					sumA += tmpA;

					tmpA  = ld(II , xse , yse , currentscale , n1 , orient , ny);
					if(tmpA*Ac <= 0)
					{
						valF |= 0x10;
					}
					sumA += tmpA;

					tmpA  =  ld(II , xc  , yse , currentscale , n1 , orient , ny);
					if(tmpA*Ac <= 0)
					{
						valF |= 0x20;
					}
					sumA += tmpA;

					tmpA =  ld(II , xnw , yse , currentscale , n1 , orient , ny);
					if(tmpA*Ac <= 0)
					{
						valF |= 0x40;
					}
					sumA += tmpA;

					tmpA =  ld(II , xnw , yc , currentscale , n1 , orient , ny);
					if(tmpA*Ac <= 0)
					{
						valF |= 0x80;
					}
					sumA += tmpA;
					if(improvedLDP)
					{
						if(sumA > 9*Ac)
						{
							valF |= 0x100; 
						}
					}
					H[valF + sNbins]++;
				}
			}
			if(rmextremebins == 1)
			{
				if(improvedLDP)
				{
					H[0 + sNbins] = H[Nbins-1 + sNbins] = H[Nbins + sNbins] = H[NBINS-1 + sNbins] = 0.0;
				}
				else
				{
					H[0 + sNbins] = H[Nbins-1 + sNbins] = 0.0;
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

			sNbins  += NBINS;
		}
	}
}

/*----------------------------------------------------------------------------------------------------------------------------------------- */
int	number_histo_ldp(double *spyr , int nspyr)
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
double ld(double *II , int x , int y , int s , int n , int a , int Ny)
{
	if (n > 1)
	{
		if(a == 0)
		{
			return (ld(II , x , y , s , n - 1 , a , Ny) - ld(II , x + s , y , s , n - 1 , a , Ny));
		}
		else if(a == 1)
		{
			return (ld(II , x , y , s , n - 1 , a , Ny) - ld(II , x + s , y + s , s , n - 1 , a , Ny));
		}
		else if(a == 2)
		{
			return (ld(II , x , y , s , n - 1 , a , Ny) - ld(II , x  , y + s , s , n - 1 , a , Ny));
		}
		else
		{
			return (ld(II , x , y , s , n - 1 , a , Ny) - ld(II , x - s , y + s , s , n - 1 , a , Ny));
		}
	}
	else
	{
		if(a == 0)
		{
			return (Area(II , x , y , s  , s  , Ny) - Area(II , x + s , y , s  , s  , Ny));
		}
		else if(a == 1)
		{
			return (Area(II , x , y , s  , s  , Ny) - Area(II , x + s , y + s , s  , s  , Ny));
		}
		else if(a == 2)
		{
			return (Area(II , x , y , s  , s  , Ny) - Area(II , x  , y + s , s  , s  , Ny));
		}
		else
		{
			return (Area(II , x , y , s  , s  , Ny) - Area(II , x - s , y + s , s  , s  , Ny));
		}
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

	/* T    = inv([1.0 0.956 0.621; 1.0 -0.272 -0.647; 1.0 -1.106 1.703]);

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
