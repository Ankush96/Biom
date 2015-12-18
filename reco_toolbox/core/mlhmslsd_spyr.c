/*


Multi-Level Histograms of Color Multi-Scale Line Segment Detector with Spatial Pyramid.

Usage
-----


[H , segments] = mlhmslsd_spyr(I , [options] );


Inputs
------


I                              Input image (ny x nx x [3]) in UINT8 format. 


options
       q                       Bound to quantization error on the gradient norm (default q = 2.0)
	   d                       Gradient angle tolerance, tau = 180 degree / d (default d = 8.0)
	   eps                     Detection threshold, -log10(max. NFA) (default eps = 0.0)
       n_bins                  # bins in 'ordering' of gradient modulus (default n_bins = 16256)
	   max_grad                Gradient modulus in the highest bin (default max_grad = 260100)
       spyr                    Spatial Pyramid (nspyr x 4) (default [1 , 1 , 1 , 1] with nspyr = 1)
                               where spyr(i,1) is the ratio of ny in y axis of the blocks at level i (by(i) = spyr(i,1)*ny)
                               where spyr(i,2) is the ratio of nx in x axis of the blocks at level i (bx(i) = spyr(i,3)*nx)
                               where spyr(i,3) is the ratio of ny in y axis of the shifting at level i (deltay(i) = spyr(i,2)*ny)
                               where spyr(i,4) is the ratio of nx in x axis of the shifting at level i (deltax(i) = spyr(i,4)*nx)
       color                   0 : force gray-scale (dimcolor = 1, default), 1 : RGB (dimcolor = 3), 2 : nRGB (dimcolor = 3), 3 : Opponent color (dimcolor = 3), 
                               4 : nOpponent color (dimcolor = 2), 5 : Hue  (dimcolor = 1)
	   scale                   Gaussian filter scaling vector for image before processing (1 x nscale)
	   sigma_scale             Scaling factor to obtain the standard deviation of the Gaussian filter (sigma = sigma_scale/scale)
	   nori                    Number of bin's orientation (default nori = 9)
	   nlength                 Number of bin's length  (default nlength = 9)
 	   norm                    Normalization vector (1 x 3) : [for all subwindows, for each subwindows of a pyramid level, for each subwindows]
                               norm = 0 <=> no normalization, norm = 1 <=> v=v/(sum(v)+epsi), norm = 2 <=> v=v/sqrt(sum(v²)+epsi²),
	                           norm = 3 <=> v=sqrt(v/(sum(v)+epsi)) , norm = 4 <=> L2-clamped (default norm = [0 , 0 , 4])
	   clamp                   Clamping value (default clamp = 0.2)



Outputs
--------

H                                     Histogram of features (dimcolor*nscale*nH*nori*nlength x 1) where nH = sum(floor(((1 - spyr(:,1))./(spyr(:,3)) + 1)).*floor((1 - spyr(:,2))./(spyr(:,4)) + 1))
segments                              Matrix of detected segments (7 x nb_segments) where each column vector is [x1 , y1 , x2 , y2 , witdh , scale , dimcolor]

Compilation instructions

mex mlhmslsd.c 


mex -g -output mlhmslsd_spyr.dll mlhmslsd_spyr.c 


With the intel compiler, for a win32 dll :

mex -f mexopts_intel10.bat -output mlhmslsd_spyr.dll mlhmslsd_spyr.c 



Example1
-------

I                   = imread('lena.pgm');
nH                  = 12*12;
[H , out]           = mlhmslsd_spyr(I);

figure(1)
plot(1:nH , H)
axis([0 , nH+1 , min(H) , max(H)*1.1])


figure(2),imagesc(I)
colormap(gray)
hold on
line(out([2,4] , :) , out([1,3] , :))
hold off



Example2
-------

I                     = imread('02769_Right_StudentOffice.jpeg');



options.q           = 2;
options.d           = 8;
options.eps         = -15.0;
options.n_bins      = 16256;
options.max_grad    = 260100;
options.spyr        = [1 , 1 , 1 , 1 ; 1/2 , 1/2 , 1/4 , 1/4 ; 1/4 , 1/4 , 1/8 , 1/8];
options.scale       = [0.25 , 0.5 , 1];
options.sigma_scale = 0.6;
options.color       = 3;
options.nori        = 12;
options.nlength     = 12;
options.norm        = [0 , 0 , 4];
options.clamp       = 0.2;


nH                  = options.nori*options.nlength*sum(floor(((1 - options.spyr(:,1))./(options.spyr(:,3)) + 1)).*floor((1 - options.spyr(:,2))./(options.spyr(:,4)) + 1));



[H , out]           = mlhmslsd_spyr(I , options);
nb_segment          = histc(out(6 , :) , options.scale);


figure(1)
plot(1:length(H) , H)
axis([0 , length(H)+1 , min(H) , max(H)*1.1])

figure(2),imagesc(I)
colormap(gray)
hold on
h = line(out([2,4] , :) , out([1,3] , :));
hold off
set(h , 'Color' , [0 , 0 , 1]);


Author : Sébastien PARIS : sebastien.paris@lsis.org
-------  Date : 02/02/2010

Reference : [1] R. Grompone von Gioi, J. Jakubowicz, J.-M. Morel, G. Randall,
---------       "LSD: A Fast Line Segment Detector with a False Detection Control". 
                IEEE Trans. on PAMI, 19 Dec. 2008

Changelog : - add extra option for normalization
----------


*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "mex.h"


#ifndef FALSE
#define FALSE 0
#endif /* !FALSE */

#ifndef TRUE
#define TRUE 1
#endif /* !TRUE */


#define NOTDEF   -1000.0
#define BIG_NUMBER 1.0e+300
#define NOTUSED 0
#define USED    1
#define NOTINI  2
#define NV_EPS_MAXULPS 10
#define TABSIZE 100000

#define tiny 1e-8
#define verytiny 1e-15

#define M_LN10 2.30258509299404568402
#define M_PI   3.14159265358979323846
#define M_3_2_PI 4.71238898038
#define M_2__PI  6.28318530718

#define sqrt3 1.73205080756888
#define invsqrt2 0.707106781186547
#define invsqrt3 0.577350269189626
#define invsqrt6 0.408248290463863


#ifndef max
#define max(a,b) (a >= b ? a : b)
#define min(a,b) (a <= b ? a : b)
#endif

#define MY_ROUND(f) ( (float) floor( (double) (f) + 0.5 ) )
#define log_gamma(x) ((x)>15.0?log_gamma_windschitl(x):log_gamma_lanczos(x))




/*----------------------------------------------------------------------------*/
/* Global variables
*/
static float * sum_l;            /* weight sum on longitudinal direction */
static float * sum_w;            /* weight sum on lateral direction */
static int sum_res = 1;          /* resolution factor on weight sum */
static int sum_offset;           /* offset to center of weight sum */
static int verbose = FALSE;


struct opts
{
	float   q;
	float   d;
	double  eps;
	int     n_bins;
	int     max_grad;
	double  *spyr;
	int     nspyr;
	double  *scale;
	int     nscale;
	double  sigma_scale;
	int     color;
	int     nori;
	int     nlength;
	double  *norm;
	double  clamp;
};

/*----------------------------------------------------------------------------*/
struct coorlist
{
	int x,y;
	struct coorlist * next;
};

/*----------------------------------------------------------------------------*/
struct point {int x,y;};

/*----------------------------------------------------------------------------*/

typedef struct ntuple_float_s
{
	int size;
	int max_size;
	int dim;
	float * values;
} * ntuple_float;

/*----------------------------------------------------------------------------*/
struct rect /* line segment with width */
{
	float x1,y1,x2,y2;  /* first and second point of the line segment */
	float width;        /* rectangle width */
	float x,y;          /* center of the rectangle */
	float theta;        /* angle */
	float dx,dy;        /* vector with the line segment angle */
	float prec;         /* tolerance angle */
	double p;           /* probability of a point with angle within prec */
};


/*----------------------------------------------------------------------------*/
/*
rectangle points iterator
*/
typedef struct
{
	float vx[4];
	float vy[4];
	float ys,ye;
	int x,y;
} rect_iter;


/*----------------------------------------------------------------------------*/
typedef struct image_char_s
{
	unsigned char * data;
	int xsize,ysize;
} * image_char;


/*----------------------------------------------------------------------------*/
typedef struct image_float_s
{
	float * data;
	int xsize,ysize;
} * image_float;


void rgb2gray(unsigned char * , int , int , float *);
void rgb2nrgb(unsigned char * , int , int , float *);
void rgb2opponent(unsigned char * , int , int , float *);
void rgb2nopponent(unsigned char * , int , int , float *);
void rgb2hue(unsigned char * , int , int , float *);
static void error(char *);
static int float_equal(float , float );
static void free_ntuple_float(ntuple_float );
static ntuple_float new_ntuple_float(int );
static void free_image_char(image_char );
static image_char new_image_char(int  , int );
static image_char new_image_char_ini(int , int  , int );
static void free_image_float(image_float );
static image_float new_image_float(int , int );
static void gaussian_kernel( ntuple_float , float , float);
static image_float gaussian_sampler( image_float , float , float , int );
static image_float ll_angle( image_float , float , struct coorlist ** , void ** ,image_float * , int , int , int );
static int isaligned(int , int , image_float , float , float );
static float angle_diff(float , float );
static double log_gamma_lanczos(double );
static double log_gamma_windschitl(double );
static double nfa(int , int , double , double );
static void rect_copy(struct rect * , struct rect * );
static float inter_low(float , float , float , float , float );
static float inter_hi(float , float , float , float , float );
static void ri_del(rect_iter * );
static int ri_end(rect_iter * );
static void ri_inc(rect_iter * );
static rect_iter * ri_ini(struct rect * );
static double rect_nfa(struct rect * , image_float , double );
static float get_theta( struct point * , int , float , float , image_float , float , float , float *  );
static float region2rect( struct point * , int , image_float , float , float , double , struct rect * );
static void region_grow( int , int , image_float , struct point * , int * , float * , image_char , float , int ,image_float , double , int  );
static double rect_improve( struct rect * , image_float , double , double  );
double *LineSegmentDetection( image_float , struct opts , double * , int *, int , int);
void compute_hlsd(double * , int  , int  , int , int  , int , double , double * , struct opts );
int	number_subwindows(double *, int );



/*----------------------------------------------------------------------------*/
/*                                Matlab C Main                                 */
/*----------------------------------------------------------------------------*/

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) 
{
	unsigned char *I;
	const int *dimim;
	double *H , *segments;
	double  norm_default[3]      = {0 , 0 , 4};
	image_float in;
	int ny , nx , i;
	int tempint , nynx , dimcolor = 1 , nH = 1 , D , nbsegments=0;
	struct opts options;
	mxArray *mxtemp;
	double *tmp;
	float tempfloat;
	double tempdouble , temp;
	int dimsegments[2];


	options.q           = 2.0;
	options.d           = 8.0;
	options.eps         = 0.0;
	options.n_bins      = 16256;
	options.max_grad    = 260100;
	options.nspyr       = 1;
	options.nscale      = 1;
	options.sigma_scale = 0.6;
	options.color       = 0;
	options.nori        = 12;
	options.nlength     = 12;
	options.clamp       = 0.2;

	if( (nrhs < 1) )
	{
		mexErrMsgTxt("At least an Image I (Ny x Nx x 3) in UINT8 format");
	}

	/* Input 1  */

	if( (mxGetNumberOfDimensions(prhs[0]) > 3) || !mxIsUint8(prhs[0]) )
	{
		mexErrMsgTxt("I must be (ny x nx x [3]) in UINT8 format\n");
	}

	I                 = (unsigned char *)mxGetData(prhs[0]);
	dimim             = mxGetDimensions(prhs[0]);

	ny                = dimim[0];
	nx                = dimim[1];
	nynx              = ny*nx;


	/* Input 2  */


	if ((nrhs > 1) && !mxIsEmpty(prhs[1]) )
	{
		mxtemp                            = mxGetField(prhs[1] , 0 , "q");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			tempfloat                     = (float) tmp[0];
			if( (tempfloat < 0.0))
			{
				mexErrMsgTxt("q > 0");	
				options.q     = 2.0;
			}
			else
			{
				options.q     = tempfloat;
			}
		}

		mxtemp                            = mxGetField(prhs[1] , 0 , "d");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			tempfloat                     = (float) tmp[0];
			if( (tempfloat < 0.0))
			{
				mexErrMsgTxt("d > 0");	
				options.d     = 8.0;
			}
			else
			{
				options.d     = tempfloat;
			}
		}

		mxtemp                            = mxGetField(prhs[1] , 0 , "eps");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			tempdouble                    = tmp[0];
			options.eps                   = tempdouble;
		}

		mxtemp                            = mxGetField(prhs[1] , 0 , "n_bins");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			tempint                       = (int) tmp[0];

			if( (tempint < 1))
			{
				mexErrMsgTxt("n_bins > 0");	
				options.n_bins            = 16256;
			}
			else
			{
				options.n_bins            = tempint;
			}
		}

		mxtemp                            = mxGetField(prhs[1] , 0 , "max_grad");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			tempint                       = (int) tmp[0];

			if( (tempint < 1))
			{
				mexErrMsgTxt("max_grad > 0");	
				options.max_grad          = 260100;
			}
			else
			{
				options.max_grad          = tempint;
			}
		}

		mxtemp                            = mxGetField(prhs[1] , 0 , "spyr");
		if(mxtemp != NULL)
		{
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

		mxtemp                            = mxGetField(prhs[1] , 0 , "scale");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			options.nscale                = mxGetN(mxtemp);
			for(i = 0 ; i < options.nscale ; i++)
			{
				if( (tmp[i] < 0.0))
				{
					mexErrMsgTxt("Elements of scale must be > 0");	
				}
			}
			options.scale                  = tmp;
		}

		else
		{
			options.nscale                = 1;
			options.scale                 = mxMalloc(sizeof(double));
			options.scale[0]              = 1.0;
		}

		mxtemp                            = mxGetField(prhs[1] , 0 , "sigma_scale");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			tempdouble                    = tmp[0];
			if( (tempdouble < 0.0))
			{
				mexPrintf("sigma_scale > 0");	
				options.sigma_scale       = 0.6;
			}
			else
			{
				options.sigma_scale      = tempdouble;
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

		mxtemp                            = mxGetField(prhs[1] , 0 , "nori");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			tempint                       = (int) tmp[0];

			if( (tempint < 0))
			{
				mexPrintf("nori > 0");	
				options.nori              = 12;
			}
			else
			{
				options.nori            = tempint;
			}
		}

		mxtemp                            = mxGetField(prhs[1] , 0 , "nlength");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			tempint                       = (int) tmp[0];

			if( (tempint < 1))
			{
				mexPrintf("length > 0");	
				options.nlength           = 12;
			}
			else
			{
				options.nlength           = tempint;
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
				mexPrintf("clamp must be >= 0, force to 0.2");	

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
		options.nscale                = 1;

		options.scale                 = mxMalloc(sizeof(double));
		options.scale[0]              = 1.0;

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

	in                                = (image_float) malloc(sizeof(struct image_float_s));

	if((mxGetNumberOfDimensions(prhs[0]) == 2))
	{
		options.color  = 0;

		in->data         = (float *)malloc(nynx*sizeof(float));
		dimcolor         = 1;
		for (i = 0 ; i < nynx ; i++)
		{
			in->data[i]  = (float)I[i];
		}
	}
	else
	{
		if((options.color == 0) )
		{
			in->data         = (float *)malloc(nynx*sizeof(float));
			rgb2gray(I , ny , nx , in->data);
			dimcolor         = 1;
		}
		else if (options.color == 1)
		{
			in->data        = (float *)malloc(3*nynx*sizeof(float));
			for (i = 0 ; i < 3*nynx ; i++)
			{
				in->data[i] = (float)I[i];
			}
			dimcolor        = 3;
		}
		else if (options.color == 2)
		{
			in->data        = (float *)malloc(3*nynx*sizeof(float));
			rgb2nrgb(I , ny , nx , in->data);
			dimcolor        = 3;
			options.q       = options.q/(255);
		}

		else if (options.color == 3)
		{
			in->data        = (float *)malloc(3*nynx*sizeof(float));
			rgb2opponent(I , ny , nx , in->data);
			dimcolor        = 3;
		}
		else if(options.color == 4)
		{
			in->data        = (float *)malloc(2*nynx*sizeof(float));
			rgb2nopponent(I , ny , nx , in->data);
			dimcolor        = 2;
			options.q       = options.q/(255);
		}
		else if(options.color == 5)
		{
			in->data        = (float *)malloc(nynx*sizeof(float));
			rgb2hue(I , ny , nx , in->data);
			dimcolor        = 1;
			options.q       = options.q/(255);
		}
	}


	in->xsize         = ny;
	in->ysize         = nx;


	nH                =  number_subwindows(options.spyr , options.nspyr);
	D                 =  dimcolor*options.nscale*nH*options.nori*options.nlength;

	plhs[0]           =  mxCreateDoubleMatrix(D , 1 , mxREAL);
	H                 =  mxGetPr(plhs[0]);

	plhs[1]           =  mxCreateDoubleMatrix(0 , 0 , mxREAL);

	/* ----------- Main Call -----------*/

	segments          = LineSegmentDetection(in , options , H , &nbsegments , nH , dimcolor);

	/* ---------------------------------*/

	dimsegments[0]    = 7;
	dimsegments[1]    = nbsegments;
	mxSetDimensions(plhs[1] , dimsegments , 2);
	mxSetPr(plhs[1] , segments);


	free(in->data);
	free(in);

	if( (nrhs > 1) && !mxIsEmpty(prhs[1]) )
	{
		if((mxGetField(prhs[1] , 0 , "scale")) == NULL)
		{
			mxFree(options.scale);
		}
		if((mxGetField(prhs[1] , 0 , "spyr")) == NULL)
		{
			mxFree(options.spyr);
		}
		if ( (mxGetField( prhs[1] , 0 , "norm" )) == NULL )
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
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*-------------------------- LINE SEGMENT DETECTION --------------------------*/
/*----------------------------------------------------------------------------*/

double * LineSegmentDetection( image_float in , struct opts options , double *H , int *nbsegment , int nH , int dimcolor)
{
	float d=options.d , q=options.q , sigma_scale=(float)options.sigma_scale ;   
	double *scale=options.scale , *spyr = options.spyr;
	double eps=options.eps ;          
	int n_bins=options.n_bins, max_grad=options.max_grad, nspyr = options.nspyr, nscale = options.nscale , nori = options.nori , nlength = options.nlength;   

	float currentscale , rho = q / (float) sin( M_PI / (double) d );   /* gradient threshold */
	struct coorlist * list_p;
	void * mem_p;
	image_float modgrad;
	image_float filtered_in = NULL;
	image_float angles;
	image_char used;
	int xsize,ysize;
	struct point * reg;
	int reg_size;
	float reg_angle;
	float prec = (float) (M_PI / d);
	double p = 1.0 / (double) d;
	double nfa_val;
	double logNT;
	struct rect rec;
	int radius = 1;
	int i , s , u , l , m , v;
	int min_reg_size;
	int segnum = *nbsegment , ind , ny = in->xsize , nx = in->ysize;
	int nynx   = ny*nx , vnynx;

	double *segments;
	double scaley, scalex;
	double maxfactor = 0.0 , ratio;
	int deltay, deltax, sy , sx, origy, origx , offsety = 0 , offsetx = 0;
	int ly, lx , coscale , Nbins = nori*nlength;
	int norm_all = (int) options.norm[0] , norm_p = (int) options.norm[1] , D;
	int co_p , offset , indj;
	double clamp = options.clamp , sum , temp;

	segments         = (double *)mxMalloc(7*sizeof(double));

	for (u = 0 ; u < nspyr ; u++)
	{
		maxfactor   = max(maxfactor , spyr[u + 0]*spyr[u + nspyr]);
	}

	D           =  dimcolor*Nbins*nH*nscale;

	coscale     = 0;

	for (v = 0 ; v < dimcolor ; v++)
	{
		vnynx      = v*nynx;
		for (s = 0 ; s < nscale ; s++)
		{
			currentscale = (float) scale[s];
			/* scale (if necesary) and angles computation */
			if( !float_equal(currentscale,1.0) )
			{
				filtered_in = gaussian_sampler(in , currentscale, sigma_scale ,  vnynx);
				angles      = ll_angle(filtered_in, rho, &list_p, &mem_p, &modgrad, n_bins, max_grad , 0);
				xsize       = filtered_in->xsize;
				ysize       = filtered_in->ysize;
			}
			else
			{
				angles      = ll_angle(in , rho , &list_p , &mem_p , &modgrad , n_bins , max_grad ,  vnynx);
				xsize       = in->xsize;
				ysize       = in->ysize;
			}

			used            = new_image_char_ini(xsize , ysize , NOTUSED);
			reg             = (struct point *) calloc(xsize * ysize, sizeof(struct point));
			sum_offset      = sum_res * (ceil( sqrt( (double) xsize * xsize + (double) ysize * ysize) )) + 2;
			sum_l           = (float *) calloc(2*sum_offset,sizeof(float));
			sum_w           = (float *) calloc(2*sum_offset,sizeof(float));

			if( !reg || !sum_l || !sum_w ) 
			{
				error("Not enough memory!\n");
			}

			logNT           = 2.5 * ( log10( (double) xsize ) + log10( (double) ysize ) );
			min_reg_size    = -logNT/log10(p); 

			for(;list_p; list_p = list_p->next )
				if( ( used->data[ list_p->x + list_p->y * used->xsize ] == NOTUSED && !float_equal(angles->data[ list_p->x + list_p->y * angles->xsize ], NOTDEF) ) )
				{
					region_grow( list_p->x, list_p->y, angles, reg, &reg_size, &reg_angle, used, prec, radius , modgrad, p, min_reg_size );

					if( reg_size < min_reg_size )
					{
						for(i=0; i<reg_size; i++)
							used->data[reg[i].x+reg[i].y*used->xsize] = NOTINI;
						continue;
					}

					region2rect(reg,reg_size,modgrad,reg_angle,prec,p,&rec);
					nfa_val = rect_improve(&rec,angles,logNT,eps);

					if( nfa_val > eps )
					{
						rec.x1 += 0.5; rec.y1 += 0.5;
						rec.x2 += 0.5; rec.y2 += 0.5;
						if( !float_equal(currentscale,1.0) )
						{
							rec.x1    /= currentscale;
							rec.y1    /= currentscale;
							rec.x2    /= currentscale;
							rec.y2    /= currentscale;
							rec.width /= currentscale;
						}

						ind                    = segnum*7;
						segments               = (double *) mxRealloc((double *)segments , (ind + 7)*sizeof(double));
						segments[0 + ind]      = (double)rec.x1;
						segments[1 + ind]      = (double)rec.y1;
						segments[2 + ind]      = (double)rec.x2;
						segments[3 + ind]      = (double)rec.y2;
						segments[4 + ind]      = (double)rec.width;
						segments[5 + ind]      = (double)currentscale;
						segments[6 + ind]      = (double)v;
						++segnum;
					}
					else
					{
						for(i=0; i<reg_size; i++)
						{
							used->data[reg[i].x + reg[i].y*used->xsize] = NOTINI;
						}
					}
				}

				if( (filtered_in != NULL) && !float_equal(currentscale,1.0) ) free_image_float(filtered_in);

				if( used != NULL ) 
				{
					free_image_char(used);
				}
				if( reg   != NULL ) 
				{
					free( (void *) reg );
				}
				if( sum_l != NULL ) 
				{
					free( (void *) sum_l );
				}
				if( sum_w != NULL ) 
				{
					free( (void *) sum_w );
				}
				if( angles != NULL ) 
				{
					free_image_float(angles);
				}
				if( modgrad != NULL ) 
				{
					free_image_float(modgrad);	
				}
				if( mem_p != NULL ) 
				{
					free( (void *) mem_p );
				}
				if( list_p != NULL ) 
				{
					free( (void *)list_p);
				}

				for (u = 0 ; u < nspyr ; u++)
				{
					scaley      = (spyr[u + nspyr*2]);
					ly          = (int) ( (1 - spyr[u + 0])/scaley + 1);
					deltay      = (int) (ny*scaley);
					sy          = (int) (ny*spyr[u + 0]);
					offsety     = max(0 , (int) ( floor(ny - ( (ly-1)*deltay + sy + 1)) ));

					scalex      = (spyr[u + nspyr*3]);
					lx          = (int) ( (1 - spyr[u + nspyr*1])/scalex + 1);
					deltax      = (int) (nx*scalex);
					sx          = (int) (nx*spyr[u + nspyr*1]);
					offsetx     = max(0 , (int) ( floor(nx - ( (lx-1)*deltax + sx + 1)) ));

					co_p        = 0;
					ratio       = maxfactor/(spyr[u + 0]*spyr[u + nspyr]);

					for(l = 0 ; l < lx ; l++) // Loop shift on x-axis 
					{
						origx  = offsetx + l*deltax ;
						for(m = 0 ; m < ly ; m++)   // Loop shift on y-axis  
						{
							origy     = offsety + m*deltay ;
							compute_hlsd(segments , segnum , sy , sx , origy , origx , ratio , H + coscale , options);
							coscale  += Nbins;
							co_p++;
						}
					}

					/* Normalization per pyramid level */

					if(norm_p == 1)
					{
						sum       = 0.0;
						indj      = coscale - Nbins*co_p;
						for(i = indj ; i < (Nbins*co_p + indj) ; i++)
						{
							sum   += H[i];
						}
						sum       = 1.0/(sum + tiny);
						for(i = indj ; i < (Nbins*co_p + indj) ; i++)
						{
							H[i] *= sum;
						}
					}
					if(norm_p == 2)
					{
						sum       = 0.0;
						indj      = coscale - Nbins*co_p;
						for(i = indj ; i < (Nbins*co_p + indj) ; i++)
						{
							temp   = H[i];
							sum   += temp*temp;
						}
						sum = 1.0/sqrt(sum + verytiny);
						for(i = indj ; i < (Nbins*co_p + indj) ; i++)
						{
							H[i] *= sum;
						}
					}
					if(norm_p == 3)
					{
						sum       = 0.0;
						indj      = coscale - Nbins*co_p;
						for(i = indj ; i < (Nbins*co_p + indj) ; i++)
						{
							sum   += H[i];
						}
						sum = 1.0/(sum + tiny);
						for(i = indj ; i < (Nbins*co_p + indj) ; i++)
						{
							H[i]   = sqrt(H[i]*sum);
						}
					}
					if(norm_p == 4)
					{
						sum       = 0.0;
						indj      = coscale - Nbins*co_p;
						for(i = indj ; i < (Nbins*co_p + indj) ; i++)
						{
							temp   = H[i];
							sum   += temp*temp;
						}

						sum = 1.0/sqrt(sum + verytiny);
						for(i = indj ; i < (Nbins*co_p + indj) ; i++)
						{
							H[i]   *= sum;
							if(H[i] > clamp)
							{
								H[i] = clamp;
							}
						}

						sum       = 0.0;
						for(i = indj ; i < (Nbins*co_p + indj) ; i++)
						{
							temp   = H[i];
							sum   += temp*temp;
						}

						sum = 1.0/sqrt(sum + verytiny);
						for(i = indj ; i < (Nbins*co_p + indj) ; i++)
						{
							H[i] *= sum;
						}
					}

				}

				/* Add 1 to lines for matlab */

				for (u = 0 ; u < segnum ; u++)
				{
					ind        = u*7;

					segments[0 + ind]++;
					segments[1 + ind]++;
					segments[2 + ind]++;
					segments[3 + ind]++;
					segments[6 + ind]++;
				}
		}
	}

	/* Normalization for full descriptor (D x 1) */

	if(norm_all == 1)
	{
		sum       = 0.0;
		for(i = 0 ; i < D ; i++)
		{
			sum   += H[i];
		}

		sum = 1.0/(sum + tiny);
		for(i = 0 ; i < D ; i++)
		{
			H[i]   *= sum;
		}
	}
	if(norm_all == 2)
	{
		sum       = 0.0;
		for(i = 0 ; i < D ; i++)
		{
			temp   = H[i];
			sum   += temp*temp;
		}

		sum = 1.0/sqrt(sum + verytiny);
		for(i = 0 ; i < D ; i++)
		{
			H[i]   *= sum;
		}
	}
	if(norm_all == 3)
	{
		sum       = 0.0;
		for(i = 0 ; i < D ; i++)
		{
			sum   += H[i];
		}

		sum = 1.0/(sum + tiny);
		for(i = 0 ; i < D ; i++)
		{
			H[i]   = sqrt(H[i]*sum);
		}
	}
	if(norm_all == 4)
	{
		sum       = 0.0;
		for(i = 0 ; i < D ; i++)
		{
			temp   = H[i];
			sum   += temp*temp;
		}

		sum = 1.0/sqrt(sum + verytiny);
		for(i = 0 ; i < D ; i++)
		{
			H[i]   *= sum;
			if(H[i] > clamp)
			{
				H[i] = clamp;
			}
		}

		sum       = 0.0;
		for(i = 0 ; i < D ; i++)
		{
			temp   = H[i];
			sum   += temp*temp;
		}

		sum = 1.0/sqrt(sum + verytiny);
		for(i = 0 ; i < D ; i++)
		{
			H[i]   *= sum;
		}
	}


	*nbsegment = segnum;
	return segments;

}
/*----------------------------------------------------------------------------*/
void compute_hlsd(double *segments , int segnum  , int sy , int sx , int origy , int origx , double ratio , double *H , struct opts options)
{
	int s , ind , index , bin;
	int norm = (int) options.norm[2];
	int nlength = options.nlength , nori = options.nori , nori2 = nori - 1, Nbin = nlength*nori;
	double x1 , y1 , x2 , y2 , tmpx , tmpy , dist , diag , delta_dist , angle , delta_angle;
	double	clamp = options.clamp , min_bin = -M_PI , diff_bin = 1.0/M_2__PI , temp , sum;

	diag       = sqrt(sy*sy + sx*sx);

	delta_dist  = nlength/diag;
	delta_angle = nori*diff_bin ;
	ind         = 0;

	for (s = 0 ; s < segnum ; s++)
	{
		x1  = segments[0 + ind];
		y1  = segments[1 + ind];

		x2  = segments[2 + ind];
		y2  = segments[3 + ind];

		if( (y1 >= origy ) && (y1 <= (origy + sy)) && (x1 >= origx ) && (x1 <= (origx + sx)) && (y2 >= origy ) && (y2 <= (origy + sy)) && (x2 >= origx ) && (x2 <= (origx + sx)) )
		{
			tmpy    = (y2 - y1);
			tmpx    = (x2 - x1);
			dist    = sqrt(tmpx*tmpx + tmpy*tmpy);
			angle   = atan2(tmpy , tmpx); 
			bin     = min(nori2 , max(0 , (int)floor(nori*(angle - min_bin)*diff_bin)));
			index   = floor(dist*delta_dist) + bin*nlength;
			H[index]++;
		}
		ind    += 6;
	}

	for(s = 0 ; s < Nbin ; s++)
	{
		H[s] *= ratio;
	}

	if(norm  == 1)
	{
		sum       = 0.0;
		for(s = 0 ; s < Nbin ; s++)
		{
			sum   += H[s];
		}
		sum = 1.0/(sum + tiny);
		for(s = 0 ; s < Nbin ; s++)
		{
			H[s]   *= sum;
		}
	}
	if(norm  == 2)
	{
		sum       = 0.0;
		for(s = 0 ; s < Nbin ; s++)
		{
			temp   = H[s];
			sum   += temp*temp;
		}
		sum = 1.0/sqrt(sum + verytiny);
		for(s = 0 ; s < Nbin ; s++)
		{
			H[s]   *= sum;
		}
	}
	if(norm  == 3)
	{
		sum       = 0.0;
		for(s = 0 ; s < Nbin ; s++)
		{
			sum   += H[s ];
		}
		sum = 1.0/(sum + tiny);
		for(s = 0 ; s < Nbin ; s++)
		{
			H[s ]   = sqrt(H[s]*temp);
		}
	}
	if(norm  == 4)
	{
		sum       = 0.0;
		for(s = 0 ; s < Nbin ; s++)
		{
			temp   = H[s];
			sum   += temp*temp;
		}
		sum = 1.0/sqrt(sum + verytiny);
		for(s = 0 ; s < Nbin ; s++)
		{
			H[s]   *= sum;

			if(H[s] > clamp)
			{
				H[s] = clamp;
			}
		}
		sum       = 0.0;
		for(s = 0 ; s < Nbin ; s++)
		{
			temp   = H[s];
			sum   += temp*temp;
		}
		sum = 1.0/sqrt(sum + verytiny);
		for(s = 0 ; s < Nbin ; s++)
		{
			H[s]   *= sum;
		}
	}
}

/*----------------------------------------------------------------------------*/
static void error(char * msg)
{
	fprintf(stderr,"%s\n",msg);
	exit(EXIT_FAILURE);
}


/*----------------------------------------------------------------------------*/

static int float_equal(float a, float b)
{
	return (  (a == b) || (abs( (*(int *)&a) - (*(int *)&b) ) <= NV_EPS_MAXULPS) );
}


/*----------------------------------------------------------------------------*/
static void free_ntuple_float(ntuple_float in)
{
	if( in == NULL ) error("free_ntuple_float: NULL input.");
	if( in->values == NULL ) error("free_ntuple_float: NULL values in input.");
	free((void *)in->values);
	free((void *)in);
}

/*----------------------------------------------------------------------------*/
static ntuple_float new_ntuple_float(int dim)
{
	ntuple_float n_tuple;

	n_tuple = (ntuple_float) malloc(sizeof(struct ntuple_float_s));
	if( n_tuple == NULL ) error("Not enough memory.");
	n_tuple->size = 0;
	n_tuple->max_size = 1;
	n_tuple->dim = dim;
	n_tuple->values = (float *) malloc( dim*n_tuple->max_size * sizeof(float) );
	if( n_tuple->values == NULL ) error("Not enough memory.");
	return n_tuple;
}

/*----------------------------------------------------------------------------*/

static void free_image_char(image_char i)
{
	if( i == NULL ) error("free_image_char: NULL input.");
	if( i->data == NULL ) error("free_image_char: input image have no data.");
	free((void *)i->data);
	free((void *)i);
}

static image_char new_image_char(int xsize,int ysize)
{
	image_char image;

	image = (image_char) malloc(sizeof(struct image_char_s));
	if( image == NULL ) error("Not enough memory.");
	image->data = (unsigned char *) calloc(xsize*ysize, sizeof(unsigned char));
	if( image->data == NULL ) error("Not enough memory.");

	image->xsize = xsize;
	image->ysize = ysize;

	return image;
}

static image_char new_image_char_ini(int xsize,int ysize,int ini)
{
	image_char image = new_image_char(xsize,ysize);
	int N = xsize*ysize;
	int i;
	for(i=0; i<N; i++) image->data[i] = ini;
	return image;
}

/*----------------------------------------------------------------------------*/
static void free_image_float(image_float i)
{
	if( i == NULL ) error("free_image_float: NULL input.");
	if( i->data == NULL ) error("free_image_float: input image have no data.");
	free( (void *) i->data );
	free( (void *) i );
}

/*----------------------------------------------------------------------------*/
static image_float new_image_float(int xsize,int ysize)
{
	image_float image;

	image = (image_float) malloc(sizeof(struct image_float_s));
	if( image == NULL ) error("Not enough memory.");
	image->data = (float *) calloc(xsize*ysize, sizeof(float));
	if( image->data == NULL ) error("Not enough memory.");

	image->xsize = xsize;
	image->ysize = ysize;

	return image;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------- Image Operations -----------------------------*/
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
static void gaussian_kernel( ntuple_float kernel, float sigma, float offset )
{
	int i;
	float val;
	float sum = 0.0;

	/* compute gaussian kernel */
	kernel->size = 1;
	for(i=0;i<kernel->dim;i++)
	{
		val = ( (float) i - offset ) / sigma;
		kernel->values[i] = (float) exp( (double) -0.5 * val * val );
		sum += kernel->values[i];
	}

	/* normalization */
	for(i=0;i<kernel->dim;i++) kernel->values[i] /= sum;
}

/*----------------------------------------------------------------------------*/
static image_float gaussian_sampler( image_float in, float scale, float sigma_scale , int offset )
{
	int N = floor( (double) (in->xsize) * scale ); /* final x-size */
	int M = floor( (double) (in->ysize) * scale ); /* final y-size */
	image_float aux = new_image_float(N,in->ysize);
	image_float out = new_image_float(N,M);
	float sigma = scale < 1.0 ? sigma_scale / scale : sigma_scale;
	int h = ceil( (double) sigma * sqrt( log(10.0) * 6.0 ) );
	int n = 1+2*h;
	ntuple_float kernel = new_ntuple_float(n);
	int x,y,xc,yc,i,j;
	float xx,yy,sum;
	int nx2 = 2 * in->xsize , idx;
	int ny2 = 2 * in->ysize;

	/* x axis convolution */
	for(x=0;x<aux->xsize;x++)
	{
		xx = (float) x / scale;           /* corresponding x */
		xc = floor( (double) xx + 0.5 );  /* corresponding x pixel */
		gaussian_kernel( kernel, sigma, (float) h + xx - (float) xc );

		for(y=0;y<aux->ysize;y++)
		{
			sum   = 0.0;
			idx   =  y * in->xsize + offset;

			for(i=0;i<n;i++)
			{
				j = xc - h + i;
				while(j<0) j+=nx2;
				while(j>=nx2) j-=nx2;
				if(j>=in->xsize) j = nx2-1-j;
				sum += in->data[ j + idx] * kernel->values[i];
			}
			aux->data[ x + y * aux->xsize ] = sum;
		}
	}

	/* y axis convolution */
	for(y=0;y<out->ysize;y++)
	{
		yy = (float) y / scale;           /* corresponding y */
		yc = floor( (double) yy + 0.5 );  /* corresponding y pixel */
		gaussian_kernel( kernel, sigma, (float) h + yy - (float) yc );

		for(x=0;x<out->xsize;x++)
		{
			sum = 0.0;
			for(i=0;i<n;i++)
			{
				j = yc - h + i;
				while(j<0) j+=ny2;
				while(j>=ny2) j-=ny2;
				if(j>=in->ysize) j = ny2-1-j;
				sum += aux->data[x + j * aux->xsize ] * kernel->values[i];
			}
			out->data[ x + y * out->xsize ] = sum;
		}
	}

	/* free memory */
	free_ntuple_float(kernel);
	free_image_float(aux);

	return out;
}

/*----------------------------------------------------------------------------*/
/*------------------------------ Gradient Angle ------------------------------*/
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*
compute the direction of the level line at each point.
it returns:

- an image_float with the angle at each pixel or NOTDEF.
- the image_float 'modgrad' (a pointer is passed as argument)
with the gradient magnitude at each point.
- a list of pixels 'list_p' roughly ordered by gradient magnitude.
(the order is made by classing points into bins by gradient magnitude.
the parameters 'n_bins' and 'max_grad' specify the number of
bins and the gradient modulus at the highest bin.)
- a pointer 'mem_p' to the memory used by 'list_p' to be able to
free the memory.
*/
static image_float ll_angle( image_float in, float threshold, struct coorlist ** list_p, void ** mem_p,image_float * modgrad, int n_bins, int max_grad  , int offset)
{
	image_float g = new_image_float(in->xsize,in->ysize);
	int n,p,x,y,adr,i , ind;
	float com1,com2,gx,gy,norm2;
	/* variables used in pseudo-ordering of gradient magnitude */
	float f_n_bins   = (float) n_bins;
	float f_max_grad = (float) (1.0/(double)max_grad);
	int list_count = 0;
	struct coorlist * list;
	struct coorlist ** range_l_s;
	struct coorlist ** range_l_e;
	struct coorlist * start;
	struct coorlist * end;

	if(verbose) fprintf(stderr,"gradient magnitude threshold: %g\n",threshold);

	threshold *= 4.0 * threshold;

	n         = in->ysize;
	p         = in->xsize;

	/* get memory for the image of gradient modulus */

	*modgrad  = new_image_float(in->xsize,in->ysize);

	/* get memory for "ordered" coordinate list */
	list      = (struct coorlist *) calloc(n*p,sizeof(struct coorlist));
	*mem_p    = (void *) list;
	range_l_s = (struct coorlist **) calloc(n_bins,sizeof(struct coorlist *));
	range_l_e = (struct coorlist **) calloc(n_bins,sizeof(struct coorlist *));

	if( !list || !range_l_s || !range_l_e ) 
	{
		error("Not enough memory.");
	}

	for(i=0;i<n_bins;i++) 
	{
		range_l_s[i]       = range_l_e[i] = NULL;
	}

	/* undefined on downright boundary */
	ind                    = (n-1)*p;

	for(x=ind;x<p+ind;x++) 
	{
		g->data[x]         = NOTDEF; //(n-1)*p+x
	}
	for(y=0;y<n;y++) 
	{
		g->data[p*y+p-1]   = NOTDEF;
	}

	/*** remaining part ***/
	for(x=0;x<p-1;x++)
	{
		for(y=0;y<n-1;y++)
		{
			adr                   = x + y*p;

			/* norm 2 computation */
			com1                  = in->data[adr+p+1+offset] - in->data[adr+offset];
			com2                  = in->data[adr+1+offset]   - in->data[adr+p+offset];
			gx                    = com1+com2;
			gy                    = com1-com2;
			norm2                 = gx*gx+gy*gy;

			(*modgrad)->data[adr] = (float) sqrt( (double) norm2 * 0.25 );

			if(norm2 <= threshold) 
			{
				/* norm too small, gradient no defined */
				g->data[adr]      = NOTDEF;
			}
			else
			{
				/* angle computation */
				g->data[adr]      = (float) atan2(gx,-gy);

				/* store the point in the right bin,
				according to its norm */
				i                 = (int) (norm2 * f_n_bins * f_max_grad);
				if(i>=n_bins) 
				{
					i             = n_bins-1;
				}
				if( range_l_e[i]==NULL )
				{
					range_l_s[i]  = range_l_e[i] = list+list_count++;
				}
				else
				{
					range_l_e[i]->next = list+list_count;
					range_l_e[i]       = list+list_count++;
				}
				range_l_e[i]->x    = x;
				range_l_e[i]->y    = y;
				range_l_e[i]->next = NULL;
			}
		}
	}

	/* make the list of points "ordered" by norm value */
	for(i=n_bins-1; i>0 && range_l_s[i]==NULL; i--);

	start = range_l_s[i];
	end   = range_l_e[i];
	if(start!=NULL)
	{
		for(i--;i>0; i--)
			if( range_l_s[i] != NULL )
			{
				end->next = range_l_s[i];
				end       = range_l_e[i];
			}
			*list_p = start;

			/* free memory */
			free(range_l_s);
			free(range_l_e);

			return g;
	}
}

/*----------------------------------------------------------------------------*/
/*
find if the point x,y in angles have angle theta up to precision prec
*/
static int isaligned(int x, int y, image_float angles, float theta, float prec)
{
	float a = angles->data[ x + y * angles->xsize ];

	if( float_equal(a,NOTDEF) ) return FALSE;

	/* it is assumed that theta and a are in the range [-pi,pi] */
	theta -= a;
	if( theta < 0.0 ) theta = -theta;
	if( theta > M_3_2_PI )
	{
		theta -= M_2__PI;
		if( theta < 0.0 ) theta = -theta;
	}

	return theta < prec;
}

/*----------------------------------------------------------------------------*/
static float angle_diff(float a, float b)
{
	a -= b;
	while( a <= -M_PI ) a += M_2__PI;
	while( a >   M_PI ) a -= M_2__PI;
	if( a < 0.0 ) a = -a;
	return a;
}

/*----------------------------------------------------------------------------*/
/*----------------------------- NFA computation ------------------------------*/
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*
Calculates the natural logarithm of the absolute value of
the gamma function of x using the Lanczos approximation,
see http://www.rskey.org/gamma.htm.

The formula used is
\Gamma(x) = \frac{ \sum_{n=0}^{N} q_n x^n }{ \Pi_{n=0}^{N} (x+n) }
(x+5.5)^(x+0.5) e^{-(x+5.5)}
so
\log\Gamma(x) = \log( \sum_{n=0}^{N} q_n x^n ) + (x+0.5) \log(x+5.5)
- (x+5.5) - \sum_{n=0}^{N} \log(x+n)
and
q0 = 75122.6331530
q1 = 80916.6278952
q2 = 36308.2951477
q3 = 8687.24529705
q4 = 1168.92649479
q5 = 83.8676043424
q6 = 2.50662827511
*/
static double log_gamma_lanczos(double x)
{
	static double q[7] = { 75122.6331530, 80916.6278952, 36308.2951477,
		8687.24529705, 1168.92649479, 83.8676043424,
		2.50662827511 };
	double a = (x+0.5) * log(x+5.5) - (x+5.5);
	double b = 0.0;
	int n;

	for(n=0;n<7;n++)
	{
		a -= log( x + (double) n );
		b += q[n] * pow( x, (double) n );
	}
	return a + log(b);
}

/*----------------------------------------------------------------------------*/
/*
Calculates the natural logarithm of the absolute value of
the gamma function of x using Robert H. Windschitl method,
see http://www.rskey.org/gamma.htm.

The formula used is
\Gamma(x) = \sqrt(\frac{2\pi}{x}) ( \frac{x}{e}
\sqrt{ x\sinh(1/x) + \frac{1}{810x^6} } )^x
so
\log\Gamma(x) = 0.5\log(2\pi) + (x-0.5)\log(x) - x
+ 0.5x\log( x\sinh(1/x) + \frac{1}{810x^6} ).

This formula is good approximation when x > 15.
*/
static double log_gamma_windschitl(double x)
{
	return 0.918938533204673 + (x-0.5)*log(x) - x + 0.5*x*log( x*sinh(1/x) + 1/(810.0*pow(x,6.0)) );
}

/*----------------------------------------------------------------------------*/
/*
Computes the logarithm of NFA to base 10.

NFA = NT.b(n,k,p)
the return value is log10(NFA)

n,k,p - binomial parameters.
logNT - logarithm of Number of Tests
*/
static double nfa(int n, int k, double p, double logNT)
{
	static double inv[TABSIZE];   /* table to keep computed inverse values */
	double tolerance = 0.1;       /* an error of 10% in the result is accepted */
	double log1term,term,bin_term,mult_term,bin_tail,err;
	double p_term = p / (1.0-p);
	int i;

	if( n<0 || k<0 || k>n || p<0.0 || p>1.0 )
		error("Wrong n, k or p values in nfa()");

	if( n==0 || k==0 ) return -logNT;
	if( n==k ) return -logNT - (double) n * log10(p);

	log1term = log_gamma((double)n+1.0) - log_gamma((double)k+1.0) - log_gamma((double)(n-k)+1.0) + (double) k * log(p) + (double) (n-k) * log(1.0-p);

	term = exp(log1term);
	if( float_equal(term,0.0) )              /* the first term is almost zero */
	{
		if( (double) k > (double) n * p )    /* at begin or end of the tail? */
			return -log1term / M_LN10 - logNT; /* end: use just the first term */
		else
			return -logNT;                     /* begin: the tail is roughly 1 */
	}

	bin_tail = term;
	for(i=k+1;i<=n;i++)
	{
		bin_term = (double) (n-i+1) * ( i<TABSIZE ? ( float_equal(inv[i],0.0) ? inv[i] : (inv[i]=1.0/(double)i)) : 1.0/(double)i );
		mult_term = bin_term * p_term;
		term     *= mult_term;
		bin_tail += term;
		if(bin_term<1.0)
		{
			/* when bin_term<1 then mult_term_j<mult_term_i for j>i.
			then, the error on the binomial tail when truncated at
			the i term can be bounded by a geometric series of form
			term_i * sum mult_term_i^j.                            */
			err = term * ( ( 1.0 - pow( mult_term, (double) (n-i+1) ) ) / (1.0-mult_term) - 1.0 );

			/* one wants an error at most of tolerance*final_result, or:
			tolerance * abs(-log10(bin_tail)-logNT).
			now, the error that can be accepted on bin_tail is
			given by tolerance*final_result divided by the derivative
			of -log10(x) when x=bin_tail. that is:
			tolerance * abs(-log10(bin_tail)-logNT) / (1/bin_tail)
			finally, we truncate the tail if the error is less than:
			tolerance * abs(-log10(bin_tail)-logNT) * bin_tail        */
			if( err < tolerance * fabs(-log10(bin_tail)-logNT) * bin_tail ) break;
		}
	}
	return -log10(bin_tail) - logNT;
}

/*----------------------------------------------------------------------------*/
static void rect_copy(struct rect * in, struct rect * out)
{
	out->x1     = in->x1;
	out->y1     = in->y1;
	out->x2     = in->x2;
	out->y2     = in->y2;
	out->width  = in->width;
	out->x      = in->x;
	out->y      = in->y;
	out->theta  = in->theta;
	out->dx     = in->dx;
	out->dy     = in->dy;
	out->prec   = in->prec;
	out->p      = in->p;
}

/*----------------------------------------------------------------------------*/
static float inter_low(float x, float x1, float y1, float x2, float y2)
{
	if( x1 > x2 || x < x1 || x > x2 )
	{
		fprintf(stderr,"inter_low: x %g x1 %g x2 %g.\n",x,x1,x2);
		error("Impossible situation.");
	}
	if( float_equal(x1,x2) && y1<y2 ) return y1;
	if( float_equal(x1,x2) && y1>y2 ) return y2;
	return y1 + (x-x1) * (y2-y1) / (x2-x1);
}

/*----------------------------------------------------------------------------*/
static float inter_hi(float x, float x1, float y1, float x2, float y2)
{
	if( x1 > x2 || x < x1 || x > x2 )
	{
		fprintf(stderr,"inter_hi: x %g x1 %g x2 %g.\n",x,x1,x2);
		error("Impossible situation.");
	}
	if( float_equal(x1,x2) && y1<y2 ) return y2;
	if( float_equal(x1,x2) && y1>y2 ) return y1;
	return y1 + (x-x1) * (y2-y1) / (x2-x1);
}

/*----------------------------------------------------------------------------*/
static void ri_del(rect_iter * iter)
{
	free(iter);
}

/*----------------------------------------------------------------------------*/
static int ri_end(rect_iter * i)
{
	return (float)(i->x) > i->vx[2];
}

/*----------------------------------------------------------------------------*/
static void ri_inc(rect_iter * i)
{
	if( (float) (i->x) <= i->vx[2] ) i->y++;

	while( (float) (i->y) > i->ye && (float) (i->x) <= i->vx[2] )
	{
		/* new x */
		i->x++;

		if( (float) (i->x) > i->vx[2] ) return; /* end of iteration */

		/* update lower y limit for the line */
		if( (float) i->x < i->vx[3] )
			i->ys = inter_low((float)i->x,i->vx[0],i->vy[0],i->vx[3],i->vy[3]);
		else i->ys = inter_low((float)i->x,i->vx[3],i->vy[3],i->vx[2],i->vy[2]);

		/* update upper y limit for the line */
		if( (float)i->x < i->vx[1] )
			i->ye = inter_hi((float)i->x,i->vx[0],i->vy[0],i->vx[1],i->vy[1]);
		else i->ye = inter_hi( (float)i->x,i->vx[1],i->vy[1],i->vx[2],i->vy[2]);

		/* new y */
		i->y = (float) ceil( (double) i->ys );
	}
}

/*----------------------------------------------------------------------------*/
static rect_iter * ri_ini(struct rect * r)
{
	float vx[4],vy[4] , rtemp=r->width * 0.5;
	int n,offset;
	rect_iter * i;

	i = (rect_iter *) malloc(sizeof(rect_iter));
	if(!i) error("ri_ini: Not enough memory.");

	vx[0] = r->x1 - r->dy * rtemp;
	vy[0] = r->y1 + r->dx * rtemp;
	vx[1] = r->x2 - r->dy * rtemp;
	vy[1] = r->y2 + r->dx * rtemp;
	vx[2] = r->x2 + r->dy * rtemp;
	vy[2] = r->y2 - r->dx * rtemp;
	vx[3] = r->x1 + r->dy * rtemp;
	vy[3] = r->y1 - r->dx * rtemp;

	if( r->x1 < r->x2 && r->y1 <= r->y2 ) offset = 0;
	else if( r->x1 >= r->x2 && r->y1 < r->y2 ) offset = 1;
	else if( r->x1 > r->x2 && r->y1 >= r->y2 ) offset = 2;
	else offset = 3;
	/* else if( r->x1 <= r->x2 && r->y1 > r->y2 ) offset = 3; */

	for(n=0; n<4; n++)
	{
		i->vx[n] = vx[(offset+n)%4];
		i->vy[n] = vy[(offset+n)%4];
	}

	/* starting point */
	i->x  = ceil( (double) (i->vx[0]) ) - 1;
	i->y  = ceil( (double) (i->vy[0]) );
	i->ys = i->ye = -BIG_NUMBER;

	/* advance to the first point */
	ri_inc(i);

	return i;
}

/*----------------------------------------------------------------------------*/
static double rect_nfa(struct rect * rec, image_float angles, double logNT)
{
	rect_iter * i;
	int pts = 0;
	int alg = 0;
	double nfa_val;

	for(i=ri_ini(rec); !ri_end(i); ri_inc(i))
		if( i->x>=0 && i->y>=0 && i->x<angles->xsize && i->y<angles->ysize )
		{
			if(verbose) fprintf(stderr,"| %d %d ",i->x,i->y);
			++pts;
			if( isaligned(i->x,i->y,angles,rec->theta,rec->prec) )
			{
				if(verbose) fprintf(stderr,"yes ");
				++alg;
			}
		}
		ri_del(i);

		nfa_val = nfa(pts,alg,rec->p,logNT);
		if(verbose) fprintf(stderr,"\npts %d alg %d p %g nfa %g\n",
			pts,alg,rec->p,nfa_val);

		return nfa_val;
}

/*----------------------------------------------------------------------------*/
/*---------------------------------- REGIONS ---------------------------------*/
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
static float get_theta( struct point * reg, int reg_size, float x, float y, image_float modgrad, float reg_angle, float prec, float * elongation )
{
	int i;
	float Ixx = 0.0;
	float Iyy = 0.0;
	float Ixy = 0.0;
	float lambda1,lambda2,tmp;
	float theta;
	float weight,sum;
	float lx , ly;

	if(reg_size <= 1) error("get_theta: region size <= 1.");


	/*----------- theta ---------------------------------------------------*/
	/*
	Region inertia matrix A:
	Ixx Ixy
	Ixy Iyy
	where
	Ixx = \sum_i y_i^2
	Iyy = \sum_i x_i^2
	Ixy = -\sum_i x_i y_i

	lambda1 and lambda2 are the eigenvalues, with lambda1 >= lambda2.
	They are found by solving the characteristic polynomial
	det(\lambda I - A) = 0.

	To get the line segment direction we want to get the eigenvector of
	the smaller eigenvalue. We have to solve a,b in:
	a.Ixx + b.Ixy = a.lambda2
	a.Ixy + b.Iyy = b.lambda2
	We want the angle theta = atan(b/a). I can be computed with
	any of the two equations:
	theta = atan( (lambda2-Ixx) / Ixy )
	or
	theta = atan( Ixy / (lambda2-Iyy) )

	When |Ixx| > |Iyy| we use the first, otherwise the second
	(just to get better numeric precision).
	*/
	sum = 0.0;
	for(i=0; i<reg_size; i++)
	{
		weight = modgrad->data[ reg[i].x + reg[i].y * modgrad->xsize ];
		ly     = ((float)reg[i].y - y);
		lx     = ((float)reg[i].x - x);
		Ixx   += ly * ly * weight;
		Iyy   += lx * lx * weight;
		Ixy   -= lx * ly * weight;
		sum   += weight;
	}
	if( sum <= 0.0 ) error("get_theta: weights sum less or equal to zero.");
	sum  = 1.0/sum;

	Ixx *= sum;
	Iyy *= sum;
	Ixy *= sum;


	lambda1 = ( Ixx + Iyy + (float) sqrt( (double) (Ixx - Iyy) * (Ixx - Iyy) + 4.0 * Ixy * Ixy ) ) * 0.5;
	lambda2 = ( Ixx + Iyy - (float) sqrt( (double) (Ixx - Iyy) * (Ixx - Iyy) + 4.0 * Ixy * Ixy ) ) * 0.5;
	if( fabs(lambda1) < fabs(lambda2) )
	{
		fprintf(stderr,"Ixx %g Iyy %g Ixy %g lamb1 %g lamb2 %g - lamb1 < lamb2\n",
			Ixx,Iyy,Ixy,lambda1,lambda2);
		tmp     = lambda1;
		lambda1 = lambda2;
		lambda2 = tmp;
	}
	if(verbose) fprintf(stderr,"Ixx %g Iyy %g Ixy %g lamb1 %g lamb2 %g\n", Ixx,Iyy,Ixy,lambda1,lambda2);

	*elongation = lambda1/lambda2;

	if( fabs(Ixx) > fabs(Iyy) )
		theta = (float) atan2( (double) lambda2 - Ixx, (double) Ixy );
	else
		theta = (float) atan2( (double) Ixy, (double) lambda2 - Iyy );

	/* the previous procedure don't cares orientations,
	so it could be wrong by 180 degrees.
	here is corrected if necessary */
	if( angle_diff(theta,reg_angle) > prec ) theta += M_PI;

	return theta;
}

/*----------------------------------------------------------------------------*/
static float region2rect( struct point * reg, int reg_size,image_float modgrad, float reg_angle, float prec, double p, struct rect * rec )
{
	float x,y,dx,dy,l,w,lf,lb,wl,wr,theta,weight,sum,sum_th,s,elongation;
	float lx,ly;
	int i,n;
	int l_min,l_max,w_min,w_max;

	/* center */
	x = y = sum = 0.0;
	for(i=0; i<reg_size; i++)
	{
		weight = modgrad->data[ reg[i].x + reg[i].y * modgrad->xsize ];
		x += (float) reg[i].x * weight;
		y += (float) reg[i].y * weight;
		sum += weight;
	}
	if( sum <= 0.0 ) error("region2rect: weights sum equal to zero.");
	x /= sum;
	y /= sum;
	if(verbose) fprintf(stderr,"center x %g y %g\n",x,y);

	/* theta */
	theta = get_theta(reg,reg_size,x,y,modgrad,reg_angle,prec,&elongation);
	if(verbose) fprintf(stderr,"theta %g\n",theta);

	/* length and width */
	lf = lb = wl = wr = 0.5;
	l_min = l_max = w_min = w_max = 0;
	dx = (float) cos( (double) theta );
	dy = (float) sin( (double) theta );
	for(i=0; i<reg_size; i++)
	{
		lx                     =  ((float)reg[i].x - x);
		ly                     =  ((float)reg[i].y - y);
		l                      =  lx * dx + ly * dy;
		w                      = -lx * dy + ly * dx;
		weight                 = modgrad->data[ reg[i].x + reg[i].y * modgrad->xsize ];

		n                      = (int) MY_ROUND( l * (float) sum_res );
		if(n<l_min) l_min      = n;
		if(n>l_max) l_max      = n;
		sum_l[sum_offset + n] += weight;

		n                      = (int) MY_ROUND( w * (float) sum_res );
		if(n<w_min) w_min      = n;
		if(n>w_max) w_max      = n;
		sum_w[sum_offset + n] += weight;
	}

	sum_th = 0.01 * sum; /* weight threshold for selecting region */
	for(s=0.0,i=l_min; s<sum_th && i<=l_max; i++) s += sum_l[sum_offset + i];
	lb = ( (float) (i-1) - 0.5 ) / (float) sum_res;
	for(s=0.0,i=l_max; s<sum_th && i>=l_min; i--) s += sum_l[sum_offset + i];
	lf = ( (float) (i+1) + 0.5 ) / (float) sum_res;

	sum_th = 0.01 * sum; /* weight threshold for selecting region */
	for(s=0.0,i= w_min  ; s<sum_th && i<=w_max ; i++) s += sum_w[sum_offset + i];
	wr = ( (float) (i-1) - 0.5 ) / (float) sum_res;
	for(s=0.0,i=w_max; s<sum_th && i>=w_min; i--) s += sum_w[sum_offset + i];
	wl = ( (float) (i+1) + 0.5 ) / (float) sum_res;

	if(verbose) fprintf(stderr,"lb %g lf %g wr %g wl %g\n",lb,lf,wr,wl);

	/* free vector */
	for(i=l_min + sum_offset; i<=l_max + sum_offset; i++) sum_l[i] = 0.0;
	for(i=w_min + sum_offset; i<=w_max + sum_offset; i++) sum_w[i] = 0.0;

	rec->x1    = x + lb * dx;
	rec->y1    = y + lb * dy;
	rec->x2    = x + lf * dx;
	rec->y2    = y + lf * dy;
	rec->width = wl - wr;
	rec->x     = x;
	rec->y     = y;
	rec->theta = theta;
	rec->dx    = dx;
	rec->dy    = dy;
	rec->prec  = prec;
	rec->p     = p;

	if( rec->width < 1.0 ) rec->width = 1.0;

	return elongation;
}

/*----------------------------------------------------------------------------*/
static void region_grow( int x, int y, image_float angles, struct point * reg, int * reg_size, float * reg_angle, image_char used, float prec, int radius,image_float modgrad, double p, int min_reg_size )
{
	float sumdx,sumdy;
	int xx,yy,i;
	double tmp;

	/* first point of the region */
	*reg_size  = 1;
	reg[0].x   = x;
	reg[0].y   = y;
	*reg_angle = angles->data[x+y*angles->xsize];
	sumdx      = (float) cos( (double) (*reg_angle) );
	sumdy      = (float) sin( (double) (*reg_angle) );
	used->data[x+y*used->xsize] = USED;

	/* try neighbors as new region points */
	for(i=0; i<*reg_size; i++)
		for(xx=reg[i].x-radius; xx<=reg[i].x+radius; xx++)
			for(yy=reg[i].y-radius; yy<=reg[i].y+radius; yy++)
				if( xx>=0 && yy>=0 && xx<used->xsize && yy<used->ysize && used->data[xx+yy*used->xsize] != USED && isaligned(xx,yy,angles,*reg_angle,prec) )
				{
					/* add point */
					used->data[xx+yy*used->xsize] = USED;
					reg[*reg_size].x = xx;
					reg[*reg_size].y = yy;
					++(*reg_size);

					/* update reg_angle */
					tmp        = (double) angles->data[xx+yy*angles->xsize];
					sumdx     += (float) cos( tmp );
					sumdy     += (float) sin( tmp );
					*reg_angle = (float) atan2( (double) sumdy, (double) sumdx );
				}

				if(verbose) /* print region points */
				{
					fprintf(stderr,"region found: %d points\n",*reg_size);
					for(i=0; i<*reg_size; i++) fprintf(stderr,"| %d %d ",reg[i].x,reg[i].y);
					fprintf(stderr,"\n");
				}
}

/*----------------------------------------------------------------------------*/
static double rect_improve( struct rect * rec, image_float angles, double logNT, double eps )
{
	struct rect r;
	double nfa_val,nfa_new;
	float delta = 0.5;
	float delta_2 = delta * 0.5;
	int n;

	nfa_val = rect_nfa(rec,angles,logNT);

	if( nfa_val > eps ) return nfa_val;

	rect_copy(rec,&r);
	for(n=0; n<5; n++)
	{
		r.p *= 0.5;
		r.prec = M_PI * r.p;
		nfa_new = rect_nfa(&r,angles,logNT);
		if( nfa_new > nfa_val )
		{
			nfa_val = nfa_new;
			rect_copy(&r,rec);
		}
	}

	if( nfa_val > eps ) return nfa_val;

	rect_copy(rec,&r);
	for(n=0; n<5; n++)
	{
		if( (r.width - delta) >= 0.5 )
		{
			r.width -= delta;
			nfa_new = rect_nfa(&r,angles,logNT);
			if( nfa_new > nfa_val )
			{
				rect_copy(&r,rec);
				nfa_val = nfa_new;
			}
		}
	}

	if( nfa_val > eps ) return nfa_val;

	rect_copy(rec,&r);
	for(n=0; n<5; n++)
	{
		if( (r.width - delta) >= 0.5 )
		{
			r.x1    += -r.dy * delta_2;
			r.y1    +=  r.dx * delta_2;
			r.x2    += -r.dy * delta_2;
			r.y2    +=  r.dx * delta_2;
			r.width -= delta;
			nfa_new = rect_nfa(&r,angles,logNT);
			if( nfa_new > nfa_val )
			{
				rect_copy(&r,rec);
				nfa_val = nfa_new;
			}
		}
	}

	if( nfa_val > eps ) return nfa_val;

	rect_copy(rec,&r);
	for(n=0; n<5; n++)
	{
		if( (r.width - delta) >= 0.5 )
		{
			r.x1    -= -r.dy * delta_2;
			r.y1    -=  r.dx * delta_2;
			r.x2    -= -r.dy * delta_2;
			r.y2    -=  r.dx * delta_2;
			r.width -= delta;
			nfa_new  = rect_nfa(&r,angles,logNT);
			if( nfa_new > nfa_val )
			{
				rect_copy(&r,rec);
				nfa_val = nfa_new;
			}
		}
	}

	if( nfa_val > eps ) return nfa_val;

	rect_copy(rec,&r);
	for(n=0; n<5; n++)
	{
		r.p *= 0.5;
		r.prec = M_PI * r.p;
		nfa_new = rect_nfa(&r,angles,logNT);
		if( nfa_new > nfa_val )
		{
			nfa_val = nfa_new;
			rect_copy(&r,rec);
		}
	}
	return nfa_val;
}



/*----------------------------------------------------------------------------------------------------------------------------------------- */


int	number_subwindows(double *spyr , int nspyr)
{

	int l , nH = 0 , ly , lx ;

	for (l = 0 ; l < nspyr ; l++)
	{
		ly          = (int) ( (1 - spyr[l + 0]) /(spyr[l + nspyr*2]) + 1);  //spyr[l + 0]*
		lx          = (int) ( (1 - spyr[l + nspyr*1])/(spyr[l + nspyr*3]) + 1); //spyr[l + nspyr*1]*
		nH         += ly*lx;
	}


	return nH;
}



/*--------------------------------------------------------------------------------------------------------------------------------------------*/


void rgb2gray(unsigned char *rgb , int Ny , int Nx , float *gray)

{

	/* T    = inv([1.0 0.956 0.621; 1.0 -0.272 -0.647; 1.0 -1.106 1.703]);

	vect = T(1 , :);

	*/

	int i , NyNx = Ny*Nx , NyNx2 = 2*NyNx;

	for (i = 0 ; i  < NyNx ; i++)
	{
		gray[i]   = (0.298936021293776*rgb[i] + 0.587043074451121*rgb[i + NyNx] + 0.114020904255103*rgb[i + NyNx2]);
	}
}



/*--------------------------------------------------------------------------------------------------------------------------------------------*/


void rgb2nrgb(unsigned char *rgb , int Ny , int Nx , float *nrgb)

{
	int i , NyNx = Ny*Nx , NyNx2 = 2*NyNx ;
	float r , g , b , cte;

	for (i = 0 ; i  < NyNx ; i++)
	{
		r                = (float)rgb[i];
		g                = (float)rgb[i + NyNx];
		b                = (float)rgb[i + NyNx2];

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


void rgb2opponent(unsigned char *rgb , int Ny , int Nx , float *opponent)

{
	int i , NyNx = Ny*Nx , NyNx2 = 2*NyNx ;
	float r , g , b ;

	for (i = 0 ; i  < NyNx ; i++)
	{

		r                    = (float)rgb[i];
		g                    = (float)rgb[i + NyNx];
		b                    = (float)rgb[i + NyNx2];
		opponent[i]          = (r - g)*invsqrt2;
		opponent[i + NyNx]   = (r + g - 2*b)*invsqrt6;
		opponent[i + NyNx2]  = (r + g + b)*invsqrt3;
	}
}


/*--------------------------------------------------------------------------------------------------------------------------------------------*/


void rgb2nopponent(unsigned char *rgb , int Ny , int Nx , float *nopponent)

{
	int i , NyNx = Ny*Nx , NyNx2 = 2*NyNx;
	float r , g , b , cte ;

	for (i = 0 ; i  < NyNx ; i++)
	{
		r                     = (float)rgb[i];
		g                     = (float)rgb[i + NyNx];
		b                     = (float)rgb[i + NyNx2];

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


void rgb2hue(unsigned char *rgb , int Ny , int Nx , float *hue)

{

	int i , NyNx = Ny*Nx , NyNx2 = 2*NyNx ;

	float r , g , b ;

	for (i = 0 ; i  < NyNx ; i++)
	{
		r         = (float)rgb[i];
		g         = (float)rgb[i + NyNx];
		b         = (float)rgb[i + NyNx2];
		hue[i]    = atan(sqrt3*(r - g)/(r + g - 2*b + verytiny));
	}
}
/*--------------------------------------------------------------------------------------------------------------------------------------------*/














