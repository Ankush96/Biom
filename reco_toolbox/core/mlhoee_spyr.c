
/*

Multi-Level of Color Histograms of Oriented Edge Energy with Spatial Pyramid

Usage
------

H = mlhoee_spyr(I , [options] );


Inputs
-------

I                                     Input image (ny x nx x [3]) in UINT8 format. 

options
       spyr                           Spatial Pyramid (nspyr x 4) (default [1 , 1 , 1 , 1] with nspyr = 1)
                                      where spyr(i,1) is the ratio of ny in y axis of the blocks at level i (by(i) = spyr(i,1)*ny)
                                      where spyr(i,2) is the ratio of nx in x axis of the blocks at level i (bx(i) = spyr(i,3)*nx)
                                      where spyr(i,3) is the ratio of ny in y axis of the shifting at level i (deltay(i) = spyr(i,2)*ny)
                                      where spyr(i,4) is the ratio of nx in x axis of the shifting at level i (deltax(i) = spyr(i,4)*nx)
       color                          0 : force gray-scale (dimcolor = 1, default), 1 : RGB (dimcolor = 3), 2 : nRGB (dimcolor = 3), 
	                                  3 : Opponent color (dimcolor = 3), 4 : nOpponent color (dimcolor = 2), 5 : Hue  (dimcolor = 1)
       norma                          Gradient normalization block size (default norma = [1 , 1] of the orginal image size)
	   kernelx                        Kernel in x-direction for computing the gradient (default kernelx = [-0.5 , 0 , 0.5])
	   kernely                        Kernel in y-direction for computing the gradient (default kernely = [-0.5 ; 0 ; 0.5])
       bndori                         bndori = 0 <=> angle in [-pi/2 , pi/2[), bndori = 1 angle in [-pi , pi[
	   nori                           Number of orientation (default nori = 9)
	   norm                           Normalization : norm = 0 <=> no normalization, norm = 1 <=> v=v/(sum(v)+epsi), norm = 2 <=> v=v/sqrt(sum(v²)+epsi²), 
	                                  norm = 3 <=> v=sqrt(v/(sum(v)+epsi)) , norm = 4 <=> L2-clamped (default norm = 1)
       interpolate                    Interpolate lineary values of energy : 1 if interpolate, 0 else (default = 1)
	   clamp                          Clamping value (default clamp = 0.2)


Output
------

H                                     mlhoee_spyr features (nH*nori*dimcolor x 1) in double format where nH is the total number of subwindows defined
                                      by the spatial pyramid spyr, i.e. nH = sum(floor(((1 - spyr(:,1))./(spyr(:,3)) + 1)).*floor((1 - spyr(:,2))./(spyr(:,4)) + 1)).





I                     = imread('image_0174.jpg');
%I                     = imread('03017_m_46.png');


options.spyr          = [1 , 1 , 1 , 1 ; 1/2 , 1/2 , 1/4 , 1/4 ; 1/4 , 1/4 , 1/8 , 1/8 ; 1/8 , 1/8 , 1/16 , 1/16];
options.norma         = [1/16 , 1/16];
options.kernelx       = [-0.5 , 0 , 0.5];
options.kernely       = [-0.5 ; 0 ; 0.5];
options.color         = 0;
options.nori          = 9;
options.bndori        = 1;
options.norm          = 1;
options.interpolate   = 1;

H                     = mlhoee_spyr(I , options);




Example 1
---------

load ped
I                     = uint8(I);



options.spyr          = [1/2 , 1 , 1/2 , 1 ; 1/4 , 1/2 , 1/4 , 1/2 ; 1/8 , 1/4 , 1/8 , 1/4 ; 1/16 , 1/8 , 1/16 , 1/8];
options.norma         = [1/8 , 1/4];
options.kernelx       = [0.0069 0.0101 0.0115 0.0101 0.0069;0.0684 0.1012 0.1152 0.1012 0.0684;-0 -0 -0 -0 -0;-0.0684 -0.1012 -0.1152 -0.1012 -0.0684;-0.0069 -0.0101 -0.0115 -0.0101 -0.0069];
options.kernely       = [0.0069 0.0101 0.0115 0.0101 0.0069;0.0684 0.1012 0.1152 0.1012 0.0684;-0 -0 -0 -0 -0;-0.0684 -0.1012 -0.1152 -0.1012 -0.0684;-0.0069 -0.0101 -0.0115 -0.0101 -0.0069]';
options.color         = 0;
options.nori          = 9;
options.bndori        = 1;
options.norm          = 1;
options.interpolate   = 1;

tic,H                 = mlhoee_spyr(I , options ); , toc

nH                    = sum(floor(((1 - options.spyr(:,1))./(options.spyr(:,3)) + 1)).*floor((1 - options.spyr(:,2))./(options.spyr(:,4)) + 1));



figure(1)
plot(reshape(H , nH , options.nori) )
figure(2)
imagesc(reshape(H , nH , options.nori) )


Example 2
---------


I                     = imread('02769_Right_StudentOffice.jpeg');

options.spyr          = [1/2 , 1 , 1/2 , 1 ; 1/4 , 1/2 , 1/4 , 1/2 ; 1/8 , 1/4 , 1/8 , 1/4 ; 1/16 , 1/8 , 1/16 , 1/8];
options.norma         = [1/32 , 1/32];
options.kernelx       = [-0.5 , 0 , 0.5];
options.kernely       = [-0.5 ; 0 ; 0.5];
options.color         = 3;
options.nori          = 9;
options.bndori        = 1;
options.norm          = 1;
options.interpolate   = 1;


H                     = mlhoee_spyr(I , options ); 

nH                    = sum(floor(((1 - options.spyr(:,1))./(options.spyr(:,3)) + 1)).*floor((1 - options.spyr(:,2))./(options.spyr(:,4)) + 1));


figure(1)
plot(1:length(H),H)
figure(2)
imagesc(reshape(H , nH , options.nori) )



Example 3
---------


load I

options.spyr          = [1/2 , 1/2 , 1/4 , 1/4 ; 1/4 , 1/4 , 1/8 , 1/8 ; 1/7 , 1/7 , 1/14 , 1/14];
options.norma         = [1 , 1];
options.kernelx       = [0.0069 0.0101 0.0115 0.0101 0.0069;0.0684 0.1012 0.1152 0.1012 0.0684;-0 -0 -0 -0 -0;-0.0684 -0.1012 -0.1152 -0.1012 -0.0684;-0.0069 -0.0101 -0.0115 -0.0101 -0.0069];;
options.kernely       = [0.0069 0.0101 0.0115 0.0101 0.0069;0.0684 0.1012 0.1152 0.1012 0.0684;-0 -0 -0 -0 -0;-0.0684 -0.1012 -0.1152 -0.1012 -0.0684;-0.0069 -0.0101 -0.0115 -0.0101 -0.0069]';
options.nori          = 12;
options.bndori        = 1;
options.norm          = 0;
options.interpolate   = 1;


H                     = mlhoee_spyr(I , options ); 
nH                    = sum(floor(((1 - options.spyr(:,1))./(options.spyr(:,3)) + 1)).*floor((1 - options.spyr(:,2))./(options.spyr(:,4)) + 1));


figure(1)
plot(reshape(H , nH , options.nori) )
figure(2)
imagesc(reshape(H , nH , options.nori) )



Example 4
---------


I                     = (rgb2gray(imread('02769_Right_StudentOffice.jpeg')));

options.spyr          = [1 , 1 , 1 , 1 ; 1/2 , 1/2 , 1/4 , 1/4 ; 1/4 , 1/4 , 1/8 , 1/8 ; 1/8 , 1/8 , 1/16 , 1/16 ; 1/16 , 1/16 , 1/16 , 1/16];
options.norma         = [1/16 , 1/16] ;
options.kernelx       = [0.0069 0.0101 0.0115 0.0101 0.0069;0.0684 0.1012 0.1152 0.1012 0.0684;-0 -0 -0 -0 -0;-0.0684 -0.1012 -0.1152 -0.1012 -0.0684;-0.0069 -0.0101 -0.0115 -0.0101 -0.0069];
options.kernely       = [0.0069 0.0101 0.0115 0.0101 0.0069;0.0684 0.1012 0.1152 0.1012 0.0684;-0 -0 -0 -0 -0;-0.0684 -0.1012 -0.1152 -0.1012 -0.0684;-0.0069 -0.0101 -0.0115 -0.0101 -0.0069]';
options.nori          = 12;
options.bndori        = 1;
options.norm          = 1;
options.interpolate   = 1;


H                     = mlhoee_spyr(I , options ); 
nH                    = sum(floor(((1 - options.spyr(:,1))./(options.spyr(:,3)) + 1)).*floor((1 - options.spyr(:,2))./(options.spyr(:,4)) + 1));


figure(1)
plot(reshape(H , nH , options.nori) )
figure(2)
imagesc(reshape(H , nH , options.nori) )



To compile
----------


mex  -g -output mlhoee_spyr.dll mlhoee_spyr.c

mex  -f mexopts_intel10.bat -output mlhoee_spyr.dll mlhoee_spyr.c


Author : Sébastien PARIS : sebastien.paris@lsis.org
-------  Date : 10/13/2009

Reference :   [1] Subhransu Maji and Alexander C. Berg and Jitendra Malik, "Classification Using Intersection Kernel Support Vector Machines is efficient"
---------         In Proceedings, CVPR 2008, Anchorage, Alaska


*/


#include <time.h>
#include <math.h>
#include <mex.h>


#define tiny 1e-8
#define verytiny 1e-15
#define ptitiny 1e-13
#define ntitiny -1e-13
#define PI 3.14159265358979323846
#ifndef max
    #define max(a,b) (a >= b ? a : b)
    #define min(a,b) (a <= b ? a : b)
#endif
#define sign(x) ((x < ntitiny) ? -1 : ((x > ptitiny) ? 1 : 0) )
#define mymod(x,a) ((x < 0) ? a : ((x > a) ? 0 : x) )

#define sqrt3 1.73205080756888
#define invsqrt2 0.707106781186547
#define invsqrt3 0.577350269189626
#define invsqrt6 0.408248290463863


struct opts
{
	double  *spyr;
	int     nspyr;
	double *norma ;
	double *kernelx;
	int    kyx;
	int    kxx;
	double *kernely;
	int    kyy;
	int    kxy;
	int    color;
	int    nori;
	int    bndori;
	int    norm;
	int    interpolate;
	double clamp;
};


/*-------------------------------------------------------------------------------------------------------------- */

/* Function prototypes */

void rgb2gray(unsigned char * , int , int , double *);
void rgb2nrgb(unsigned char * , int , int , double *);
void rgb2opponent(unsigned char * , int , int , double *);
void rgb2nopponent(unsigned char * , int , int , double *);
void rgb2hue(unsigned char * , int , int , double *);

void padarray(double * , int , int  , int , int , double *);
void conv2(double *, double *, int  , int , int  , int , int , int , double *);
int	number_subwindows(double * , int);
void mlhoee_spyr(double *  , double * , int  , int  , int , int , struct opts  );

/*-------------------------------------------------------------------------------------------------------------- */
void mexFunction( int nlhs, mxArray *plhs[] , int nrhs, const mxArray *prhs[] )
{  
	unsigned char *im;
	double *H;
	const int *dimim;
	double *I;
	int ny , nx , nynx , dimcolor = 1 , nH = 1 , i;
	double	norma_default[2]    = {1 , 1};
	double	kernelx_default[3]  = {-0.5 , 0 , 0.5};
	double	kernely_default[3]  = {-0.5 , 0 , 0.5};

	struct opts options;
	mxArray *mxtemp;
	double *tmp , temp;
	int tempint;


    options.nspyr         = 1;
    options.nori          = 9;
	options.bndori        = 0;
	options.norm          = 1;
	options.kyx           = 1;
	options.kxx           = 3;
	options.kyy           = 3;
	options.kxy           = 1;
	options.color         = 0;
	options.interpolate   = 1;
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
		mxtemp                            = mxGetField(prhs[1] , 0 , "spyr");
		if(mxtemp != NULL)
		{
			if( mxGetN(mxtemp) != 4 )
			{
				mexErrMsgTxt("spyr must be (nscale x 4) in double format");
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

		mxtemp                             = mxGetField( prhs[1], 0, "norma" );
		if(mxtemp != NULL)
		{
			if((mxGetM(mxtemp) != 1)  && (mxGetN(mxtemp) != 2) )
			{
				mexErrMsgTxt("options.norma must be (1 x 2) in double format");
			}
			options.norma                  = mxGetPr(mxtemp);
		}
		else
		{
			options.norma                 = (double *)mxMalloc(2*sizeof(double));
			for(i = 0 ; i < 2 ; i++)
			{
				options.norma[i]          = norma_default[i];
			}	
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
			options.kernelx               = (double *)mxMalloc(3*sizeof(double));
			for(i = 0 ; i < 3 ; i++)
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
			options.kernely               = (double *)mxMalloc(3*sizeof(double));
			for(i = 0 ; i < 3 ; i++)
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

		mxtemp                            = mxGetField(prhs[1] , 0 , "nori");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			tempint                       = (int) tmp[0];

			if( (tempint < 1))
			{
				mexPrintf("nori > 0, force to 9");	
				options.nori              = 9;
			}
			else
			{
				options.nori              = tempint;
			}
		}

		mxtemp                            = mxGetField(prhs[1] , 0 , "norm");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			tempint                       = (int) tmp[0];
			if( (tempint < 0) || (tempint > 4) )
			{
				mexPrintf("norm = {0 , 1 , 2 , 3 , 4}, force to 1");	

				options.norm              = 1;
			}
			else
			{
				options.norm              = tempint;
			}
		}

		mxtemp                            = mxGetField(prhs[1] , 0 , "bndori");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			tempint                       = (int) tmp[0];
			if( (tempint < 0) || (tempint > 1) )
			{
				mexErrMsgTxt("bndori = {0 , 1}, force to 1");	
				options.bndori              = 0;
			}
			else
			{
				options.bndori              = tempint;
			}
		}

		mxtemp                            = mxGetField(prhs[1] , 0 , "interpolate");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			tempint                       = (int) tmp[0];
			if( (tempint < 0) || (tempint > 1))
			{
				mexPrintf("interpolate = {0,1}, force to 1");	
				options.interpolate       = 1;
			}
			else
			{
				options.interpolate       = tempint;
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
		options.nspyr                 = 1;
		options.spyr                  = (double *)mxMalloc(4*sizeof(double));

		options.spyr[0]               = 1.0;
		options.spyr[1]               = 1.0;
		options.spyr[2]               = 1.0;
		options.spyr[3]               = 1.0;

		options.norma                 = (double *)mxMalloc(2*sizeof(double));

		for(i = 0 ; i < 2 ; i++)
		{
			options.norma[i]          = norma_default[i];
		}	

		options.kernelx               = (double *)mxMalloc(3*sizeof(double));
		options.kernely               = (double *)mxMalloc(3*sizeof(double));

		for(i = 0 ; i < 3 ; i++)
		{
			options.kernelx[i]        = kernelx_default[i];
			options.kernely[i]        = kernely_default[i];
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

	/*----------------------- Outputs -------------------------------*/

	nH                 =  number_subwindows(options.spyr , options.nspyr );
	plhs[0]            =  mxCreateDoubleMatrix(dimcolor*nH*options.nori , 1 , mxREAL);
	H                  =  mxGetPr(plhs[0]);


	/*------------------------ Main Call ----------------------------*/

	mlhoee_spyr(I , H  , ny , nx  , nH , dimcolor , options );

	/*--------------------------- Free memory -----------------------*/


	if ( (nrhs > 1) && !mxIsEmpty(prhs[1]) )
	{
		if ( (mxGetField( prhs[1] , 0 , "spyr" )) == NULL )
		{
			mxFree(options.spyr);
		}
		if ( (mxGetField( prhs[1] , 0 , "norma" )) == NULL )
		{
			mxFree(options.norma);
		}
		if ( (mxGetField( prhs[1] , 0 , "kernelx" )) == NULL )
		{
			mxFree(options.kernelx);
		}
		if ( (mxGetField( prhs[1] , 0 , "kernely" )) == NULL )
		{
			mxFree(options.kernely);
		}
	}
	else
	{
		mxFree(options.spyr);
		mxFree(options.norma);
		mxFree(options.kernelx);
		mxFree(options.kernely);
	}
	mxFree(I);
}

/*----------------------------------------------------------------------------------------------------------------------------------------- */
void mlhoee_spyr(double *I , double *H, int ny , int nx  , int nH , int dimcolor , struct opts options  )
{
	double *spyr = options.spyr , *norma = options.norma  , *kernelx = options.kernelx , *kernely = options.kernely;
	double min_bin , max_bin , diff_bin ;
	double bin_size , center_offset , mini_b , f , absf;
	double clamp = options.clamp , temp , scaley , scalex, angle , mag , tempsx , tempsy  , ratiox , ratioy  , maxfactor = 0.0  , ratio;

	int i , j , p , l , x , y , o , q , v ;
	int kyy = options.kyy , kxy = options.kxy , kyx = options.kyx , kxx = options.kxx , bndori = options.bndori;
	int nspyr = options.nspyr , nori = options.nori  , norm = options.norm , nori2 = nori - 1 , interpolate = options.interpolate;
	int by , bx , ly , lx, pady , padx , nypad , nxpad , nypadnxpad , nynew , nxnew , nynxnew ,  nx1, ny1 , ny1nx1 , nygrady , nxgrady , nygradx , nxgradx; 	
	int indjx , indjy , indjm , indi , indj , index , indexo , indexp  , indtmpx , indtmpy , bin , nhd_bin;
	int deltay, deltax, sy , sx, origy, origx , offsety , offsetx , offy , offx , extraoy , extraox , offye , offxe, x0 , y0 , x1 , y1;

	int offyx , eyx , offxx , exx , offyy , eyy , offxy , exy , sf;
	int nynx = ny*nx , norinH = nori*nH , vnynx , vnorinH;

	double *Ipaded;
	double *grady , *gradx  , *R , *m  , *sat , *cell;

	if(bndori == 0)
	{
		min_bin  = -PI/2;
		max_bin  = PI/2;		    
	}
	else	
	{
		min_bin = -PI;
		max_bin = PI;
	}

	diff_bin      = (max_bin - min_bin);
    bin_size      = diff_bin/nori;
	center_offset = bin_size*0.5;
	mini_b        = min_bin+center_offset;



	for (p = 0 ; p < nspyr ; p++)
	{
		maxfactor   = max(maxfactor , spyr[p + 0]*spyr[p + nspyr]);
	}


    by          = (int) ceil(ny*norma[0]);
	bx          = (int) ceil(nx*norma[1]);

    nynew       = ((int) ceil(ny/(double)by))*by;
    nxnew       = ((int) ceil(nx/(double)bx))*bx;

	nynxnew     = nynew*nxnew;

	ratioy      = (nynew - ny)/2.0;
	offy        = (int) floor(ratioy);
	extraoy     = (int) ceil((ratioy - offy)/2.0);
	offye       = offy + extraoy;

	ratiox      = (nxnew - nx)/2.0;
	offx        = (int) floor(ratiox);
	extraox     = (int) ceil((ratiox - offx)/2.0);
	offxe       = offx + extraox;

/*
	pady        = max(1  , max(offy , max(kyx , kyy)));
    padx        = max(1  , max(offx , max(kxx , kxy)));

*/
	pady        = max(1  , max(offye , max(kyx , kyy)));
    padx        = max(1  , max(offxe , max(kxx , kxy)));


	nypad       = ny + 2*pady;
	nxpad       = nx + 2*padx;
	nypadnxpad  = nypad*nxpad; 

	offyx       = (int) (floor((kyx + 1)/2));
	eyx         = 2*offyx - kyx;

	offxx       = (int) (floor((kxx + 1)/2));
	exx         = 2*offxx - kxx;

	offyy       = (int) (floor((kyy + 1)/2));
	eyy         = 2*offyy - kyy;

	offxy       = (int) (floor((kxy + 1)/2));
	exy         = 2*offxy - kxy;


	Ipaded      = (double *)malloc(nypadnxpad*sizeof(double));


    nygrady     = nypad + kyy - 1;
	nxgrady     = nxpad + kxy - 1;
	nygradx     = nypad + kyx - 1;
	nxgradx     = nxpad + kxx - 1;


	grady       = (double *)malloc(nygrady*nxgrady*sizeof(double));
	gradx       = (double *)malloc(nygradx*nxgradx*sizeof(double));


	R           = (double *)malloc(nynxnew*nori*sizeof(double));
	m           = (double *)malloc(nynxnew*sizeof(double));

	nx1         = nxnew + 1;
	ny1         = nynew + 1;
	ny1nx1      = ny1*nx1;

	sat         = (double *)malloc(ny1nx1*sizeof(double));
	cell        = (double *)malloc(5*nH*sizeof(double));


	for (v = 0 ; v < dimcolor ; v++)
	{
		vnynx      = v*nynx;
		vnorinH    = v*norinH;


		/* Pading */

		for(i = 0 ; i < nypadnxpad ; i++)
		{
			Ipaded[i] = 0.0;
		}


		padarray(I + vnynx , ny , nx , pady , padx , Ipaded);

		/* Gradient computation */

		conv2(Ipaded , kernely , nypad , nxpad , kyy , kxy , nygrady , nygrady , grady);
		conv2(Ipaded , kernelx , nypad , nxpad , kyx , kxx , nygradx , nxgradx , gradx);
/*
		indtmpx     = max(0,padx + offxx - exx - offxe); 
		indtmpy     = max(0,pady + offxy - exy - offxe);

		indtmpx     = padx + offxx - exx - offxe; 
		indtmpy     = pady + offxy - exy - offxe;
*/


		indtmpx     = max(0,padx + offxx - exx - offxe); 
		indtmpy     = max(0,padx + offxy - exy - offxe);

		for (i = 0 ; i < nynxnew*nori ; i++)
		{
			R[i] = 0.0;
		}

		if(bndori == 0)
		{
			for(j = 0  ; j < nxnew  ; j++)
			{		
				indjx                         = pady + offyx - eyx - offye  + (j + indtmpx)*nygradx;
				indjy                         = pady + offyy - eyy - offye  + (j + indtmpy)*nygrady;
				indjm                         = j*nynew;

				for(i = 0 ; i < nynew ; i++)
				{
					tempsx                       = gradx[i + indjx];
					tempsy                       = grady[i + indjy];

					angle                        = atan(tempsy/(tempsx + verytiny));  
					mag                          = sqrt((tempsx*tempsx) + (tempsy*tempsy)); 

					bin                          = min(nori2 , max(0 , (int)floor(nori*(angle - min_bin)/diff_bin)));

					if(interpolate)
					{

						f                              = (angle - (mini_b + bin*bin_size))/bin_size;
						absf                           = fabs(f);
						sf                             = sign(f);
						nhd_bin                        = mymod(bin + sf  , nori2);
						R[i + indjm + bin*nynxnew]     = mag*(1.0 - absf);
						R[i + indjm + nhd_bin*nynxnew] = mag*absf;

					}
					else
					{
						R[i + indjm + bin*nynxnew]   = mag;
					}

					m[i + indjm]                 = mag; 

				}
			}
		}
		else
		{
			for(j = 0  ; j < nxnew  ; j++)
			{		
				indjx                         = pady + offyx - eyx - offye  + (j + indtmpx)*nygradx;
				indjy                         = pady + offyy - eyy - offye + (j + indtmpy)*nygrady;
				indjm                         = j*nynew;

				for(i = 0 ; i < nynew ; i++)
				{
					tempsx                       = gradx[i + indjx];
					tempsy                       = grady[i + indjy];

					angle                        = atan2(-tempsy , -(tempsx + verytiny));  
					mag                          = sqrt((tempsx*tempsx) + (tempsy*tempsy)); 

					bin                          = min(nori2 , max(0 , (int)floor(nori*(angle - min_bin)/diff_bin)));

					if(interpolate)
					{
						f                              = (angle - (mini_b + bin*bin_size))/bin_size;
						absf                           = fabs(f);
						sf                             = sign(f);
						nhd_bin                        = mymod(bin + sf  , nori2);
						R[i + indjm + bin*nynxnew]     = mag*(1.0 - absf);
						R[i + indjm + nhd_bin*nynxnew] = mag*absf;

					}
					else
					{
						R[i + indjm + bin*nynxnew]   = mag;
					}

					m[i + indjm]                 = mag; 

				}
			}
		}


		/* L1 block-normalization via the Integral Image*/


		for(i = 0 ; i < ny1nx1 ; i++)
		{
			sat[i]   = 0.0;
		}

		for (j=1 ; j < nx1; j++)
		{
			indj  =  j*ny1;
			index =  indj - ny1;
			indi  =  (j-1)*nynew;
			for (i = 1 ; i < ny1 ; i++)
			{
				sat[i + indj] = sat[i-1 + indj] + sat[i + index] - sat[(i-1) + index] + m[(i-1) + indi]; 
			}
		}

		for(l = 0 ; l < (nxnew/bx)  ; l++)
		{
			x  = l*bx;
			for(p = 0 ; p < (nynew/by)  ; p++)
			{
				y     = p*by;
				x1    = x + bx;
				y1    = y + by;

				temp  = (sat[y1 + x1*ny1] - (sat[y1 + x*ny1] + sat[y + x1*ny1]) + sat[y + x*ny1]);
				if(temp != 0.0)
				{
					temp  = 1.0/temp; 
				}
				else
				{
					temp = 1.0;
				}
				for (o = 0 ; o < nori ; o++)
				{
					index = o*nynxnew;
					for(i = x ; i < x + bx ; i++)
					{
						indi = i*nynew + index;
						for(j = y ; j < y + by ; j++)
						{
							R[j + indi] *= temp;
						}
					}
				}
			}
		}


		/*---- Orientation block definitions ----  */

		index          = 0;
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

			for(l = 0 ; l < lx ; l++) /* Loop shift on x-axis */
			{
				origx  = offsetx + l*deltax ;

				for(q = 0 ; q < ly ; q++)   /* Loop shift on y-axis  */
				{
					origy             = offsety + q*deltay ;

					cell[0 + index]   = origy + offye;
					cell[1 + index]   = origx + offxe;
					cell[2 + index]   = origy + sy;
					cell[3 + index]   = origx + sx;
					cell[4 + index]   = ratio;
					index            += 5;

				}
			}
		}

		/*---- Compute Oriented Edge Energy via the cumulated Integral Image over each orientations ----  */

		for(i = 0 ; i < ny1nx1 ; i++)
		{
			sat[i] = 0.0;
		}

		for (o = 0 ; o < nori ; o++)
		{
			indexo = o*nynxnew;
			for (j = 1 + offxe; j<nx1; j++)
			{
				indj  =  j*ny1;
				index =  indj - ny1;
				indi  = (j-1)*nynew;

				for (i = 1 + offye ; i<ny1; i++)
				{
					sat[i + indj] = sat[i-1 + indj] + sat[i + index] - sat[(i-1) + index] + R[(i-1) + indi + indexo]; 
				}
			}

			index  = 0;
			indexp = o*nH + vnorinH; 

			for (j=0 ; j < nH ; j++)
			{
				y0                     = (int) cell[0 + index];
				x0                     = (int) cell[1 + index];
				y1                     = (int) cell[2 + index];
				x1                     = (int) cell[3 + index];

				H[j + indexp]          = (sat[y1 + x1*ny1] - (sat[y1 + x0*ny1] + sat[y0 + x1*ny1]) + sat[y0 + x0*ny1])*cell[4 + index];
				index                 += 5;
			}
		}

		/*---- Normalization ----  */

		if(norm == 1) /* L1-norm */
		{		
			for(j = 0 ; j < nH ; j++)
			{
				indj  = j*nori + vnorinH;
				temp  = 0.0;
				for(i = indj ; i < (nori+indj); i++)
				{
					temp   += H[i];
				}

				temp  = 1.0/(temp + tiny);

				for(i = indj ; i < (nori+indj); i++)
				{
					H[i]  *= temp;

				}
			}
		}
		if(norm == 2)  /* L2-norm */
		{
			for(j = 0 ; j < nH ; j++)
			{
				indj  = j*nori + vnorinH;
				temp  = 0.0;

				for(i = indj ; i < (nori+indj); i++)
				{
					temp   += (H[i]*H[i]);
				}

				temp  = 1.0/sqrt(temp + verytiny);
				for(i = indj ; i < (nori+indj); i++)
				{
					H[i]  *= temp;

				}
			}
		}
		if(norm == 3)  /* L1-sqrt */
		{
			for(j = 0 ; j < nH ; j++)
			{
				indj  = j*nori + vnorinH;
				temp  = 0.0;

				for(i = indj ; i < (nori+indj); i++)
				{
					temp   += H[i];
				}

				temp  = 1.0/(temp + tiny);
				for(i = indj ; i < (nori+indj); i++)
				{
					H[i]  = sqrt(H[i]*temp);

				}
			}
		}
		if(norm == 4)  /* L2-clamped */
		{
			for(j = 0 ; j < nH ; j++)
			{
				indj  = j*nori + vnorinH;
				temp  = 0.0;
				for(i = indj ; i < (nori+indj); i++)
				{
					temp   += (H[i]*H[i]);
				}
				temp  = 1.0/sqrt(temp + verytiny);
				for(i = indj ; i < (nori+indj); i++)
				{
					H[i]  *= temp;

					if(H[i] > clamp)
					{
						H[i] = clamp;
					}
				}
				temp  = 0.0;
				for(i = indj ; i < (nori+indj); i++)
				{
					temp   += (H[i]*H[i]);
				}

				temp  = 1.0/sqrt(temp + verytiny);
				for(i = indj ; i < (nori+indj); i++)
				{
					H[i]  *= temp;
				}
			}
		}
	}
	free(Ipaded);
	free(grady);
	free(gradx);
	free(R);
	free(m);
	free(sat);
	free(cell);
}

/*----------------------------------------------------------------------------------------------------------------------------------------- */
void padarray(double *I , int ny , int nx , int pady , int padx , double * Ipadded)
{
	int i , j , nypady = ny + 2*pady , nxpadx = nx + 2*padx , jnypady , jindy;
	int *indy , *indx;

	indy  = (int*)malloc(nypady*sizeof(int));
	indx  = (int*)malloc(nxpadx*sizeof(int));

	for(i = 0 ; i < pady ; i++)
	{
		indy[i] = 0;
	}
	for(i = pady ; i < ny + pady ; i++)
	{
		indy[i] = i - pady;
	}

	for(i = ny + pady ; i < nypady  ; i++)
	{
		indy[i] = ny - 1;
	}
	for(i = 0 ; i < padx ; i++)
	{
		indx[i] = 0;
	}
	for(i = padx ; i < nx + padx ; i++)
	{
		indx[i] = i - padx;
	}
	for(i = nx + padx ; i < nxpadx ; i++)
	{
		indx[i] = nx - 1;
	}
	for(j = 0 ; j < nxpadx ; j++)
	{
		jnypady = j*nypady;
		jindy   = indx[j]*ny;
		for(i = 0 ; i < nypady ; i++)
		{
			Ipadded[i + jnypady] = I[indy[i] + jindy];
		}
	}

	free(indy);
	free(indx);
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

/*----------------------------------------------------------------------------------------------------------------------------------------- */
void conv2(double *A, double *B, int nyA , int nxA , int nyB , int nxB , int nyD , int nxD , double *D)
{
	int p , q , index; 
	int lengthA = nyA*nxA , lengthB = nyB*nxB , lengthD = nyD*nxD , nyA1 = nyA - 1 , nyB1 = nyB - 1;
	int nyAtemp , nxAtemp ,  nyBtemp , nxBtemp;

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
