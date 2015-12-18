
/*

denseSIFT compute SIFT (Scale-invariant feature transform) descriptors on a regular dense grid

Usage
------

[dsift , infodsift] = denseSIFT(I , [options] );


Inputs
-------

I                                     Input image (ny x nx x [3]) in UINT8 format. 

options
	   scale                          Scaling vector (1 x nscale). Extract descriptors at different scaling of the image (default scale = [1]).
	   sigma_scale                    Scaling factor to obtain the standard deviation of the Gaussian filter (sigma = sigma_scale/scale)(default sigma_scale = 0.6)
	   kernelx                        Kernel in x-direction for computing the gradient (default kernelx = [0.028 , 0.127 , 0.209 , 0.127 , 0.028 ; 0.026 , 0.116 , 0.192 , 0.116 , 0.026 ; 0 , 0 , 0 , 0 , 0 ; -0.026 , -0.116 ,  -0.192 , -0.116 -0.026 ; -0.028 -0.127 -0.209 -0.127 -0.028])
	   kernely                        Kernel in y-direction for computing the gradient (default kernely = [0.028 , 0.127 , 0.209 , 0.127 , 0.028 ; 0.026 , 0.116 , 0.192 , 0.116 , 0.026 ; 0 , 0 , 0 , 0 , 0 ; -0.026 , -0.116 ,  -0.192 , -0.116 -0.026 ; -0.028 -0.127 -0.209 -0.127 -0.028]')
       weightx                        Weights in x-direction for computing orientation responses (default weightx = [0 , 0.11 , 0.55 , 1 , 0.55 , 0.11 , 0 , 0 , 0])
	   weighty                        Weights in y-direction for computing orientation responses (default weightx = [0 , 0.11 , 0.55 , 1 , 0.55 , 0.11 , 0 , 0 , 0]')
	   deltax                         Division step in the x-axis for the grid (default deltax = floor(nx*min(scale))) 
	   deltay                         Division step in the y-axis for the grid (default deltay = floor(ny*min(scale)))
       color                          0 : force gray-scale (dimcolor = 1, default), 1 : RGB (dimcolor = 3), 2 : nRGB (dimcolor = 3), 3 : Opponent (dimcolor = 3), 
                                      4 : nOpponent (dimcolor = 2), 5 : Hue (dimcolor = 1)
	   nori                           Number of orientation (default nori = 8)
       alpha                          Parameter for attenuation of angles, must be odd (default alpha = 9)
	   nbins                          Number of bins for histograms (default nbins = 4)
	   patchsize                      Size of the square patch where the descriptor is computed (default patchsize = 2*nbins+1 ). SIFT are computed
	                                  spatially in the location p0+[-patchsize/2:patchsize/nbins:patchsize/2]
	   norm                           Normalization : norm = 0 <=> no normalization, norm = 1 <=> v=v/(sum(v)+epsi), norm = 2 <=> v=v/sqrt(sum(v²)+epsi²), 
	                                  norm = 3 <=> v=sqrt(v/(sum(v)+epsi)) , norm = 3 <=> L2-clamped (default norm = 1)
	   clamp                          Clamping value (default clamp = 0.2)
	   rmmean                         Remove mean value of the current Image at scale s_i: 0 don't remove, 1 remove (default = rmmean = 0)


Outputs
-------

dsift                                 SIFT descriptors (nbins*nbins*nori x nb_pts) where nb_pts = deltax*deltay*nscale*dimcolor
infodsift                             SIFT descriptors informations(7 x nb_pts)   where nb_pts = deltax*deltay*nscale*dimcolor
                                      infodsift(1,i) = y
									  infodsift(2,i) = x
									  infodsift(3,i) = scale
									  infodsift(4,i) = color
									  infodsift(5,i) = nyscale;
									  infodsift(6,i) = nxscale;
									  infodsift(7,i) = ||f||_{options.norm}


Example 1
---------


I                                    = imread('02769_Right_StudentOffice.jpeg');


options.scale                        = [0.5 , 0.75 , 1];
options.sigma_scale                  = 0.6;
options.deltax                       = 22;
options.deltay                       = 21;
options.color                        = 0;
options.nori                         = 8;
options.alpha                        = 9;
options.nbins                        = 4;
options.patchsize                    = 9;
options.norm                         = 4;
options.clamp                        = 0.2;
options.rmmean                       = 0;


options.sigma_edge                   = 1;
[options.kernely , options.kernelx]  = gen_dgauss(options.sigma_edge);
options.weightx                      = gen_weight(options.patchsize , options.nbins);
options.weighty                      = options.weightx';

[dsift , infodsift]                  = denseSIFT(I , options ); 

figure(1)
imagesc(dsift)


figure(2)
imagesc(I)
colormap(gray)
hold on
plot(infodsift(2 , :) , infodsift(1 , :) , 'r+')
hold off


Example 2
---------

I                                    = imread('image_0174.jpg');


options.scale                        = [1];
options.sigma_scale                  = 0.6;
options.deltax                       = 22;
options.deltay                       = 22;
options.color                        = 0;
options.nori                         = 8;
options.alpha                        = 9;
options.nbins                        = 4;
options.patchsize                    = 9;
options.norm                         = 2;
options.clamp                        = 0.2;
options.rmmean                       = 0;

options.sigma_edge                   = 1;
[options.kernely , options.kernelx]  = gen_dgauss(options.sigma_edge);
options.weightx                      = gen_weight(options.patchsize , options.nbins);
options.weighty                      = options.weightx';

[dsift , infodsift]                  = denseSIFT(I , options ); 

half                                 = options.patchsize/2;
xr                                   = [infodsift(2, :)-half ; infodsift(2, :)-half ; infodsift(2, :)+ half ; infodsift(2, :)+ half ; infodsift(2, :)-half] + 1.5;
yr                                   = [infodsift(1, :)-half ; infodsift(1, :)+half ; infodsift(1, :)+ half ; infodsift(1, :)- half ; infodsift(1, :)-half] + 1.5;


figure(1)
imagesc(I)
colormap(gray)
hold on
plot(infodsift(2 , :)+1.5 , infodsift(1 , :)+1.5 , 'r+')
plot(xr , yr , 'b')
hold off


figure(2)
imagesc(dsift)


To compile
----------

mex  -output denseSIFT.dll denseSIFT.c

mex  -g -output denseSIFT.dll denseSIFT.c

mex  -f mexopts_intel10.bat -output denseSIFT.dll denseSIFT.c


Author : Sébastien PARIS : sebastien.paris@lsis.org
-------  Date : 02/04/2010




References : [1] Lowe, David G. (1999). "Object recognition from local scale-invariant features". 
----------       Proceedings of the International Conference on Computer Vision. 2. pp. 1150–1157

             [2] Koen E. A. van de Sande, Theo Gevers and Cees G. M. Snoek, 
			     "Evaluating Color Descriptors for Object and Scene Recognition", 
				 IEEE Transactions on Pattern Analysis and Machine Intelligence, volume 32 (9), pages 1582-1596, 2010


*/


#include <time.h>
#include <math.h>
#include <mex.h>


#define tiny 1e-8
#define verytiny 1e-15
#define PI 3.14159265358979323846
#define sqrt3 1.73205080756888
#define invsqrt2 0.707106781186547
#define invsqrt3 0.577350269189626
#define invsqrt6 0.408248290463863

#ifndef max
    #define max(a,b) (a >= b ? a : b)
    #define min(a,b) (a <= b ? a : b)
#endif
#define CTE 3.71692218884984   /* sqrt( log(10.0) * 6.0 ) )*/


struct opts
{
	double  *scale;
	int     nscale;

    double  sigma_scale;

	double *kernelx;
	int    kyx;
	int    kxx;
	double *kernely;
	int    kyy;
	int    kxy;

	double *weightx;
	int    wyx;
	int    wxx;
	double *weighty;
	int    wyy;
	int    wxy;

	int    patchsize;
	int    deltax;
	int    deltay;

	int    color;
	int    nori;
	int    alpha;

	int    nbins;

	int    norm;
	double clamp;

	int    rmmean;
};

/*----------------------------------------------------------------------------------------------------------------------------------------- */
/*----------------------------------------------------------------------------------------------------------------------------------------- */
/*----------------------------------------------------------------------------------------------------------------------------------------- */

/* Function prototypes */

void rgb2gray(unsigned char * , int , int , double *);
void rgb2nrgb(unsigned char * , int , int , double *);
void rgb2opponent(unsigned char * , int , int , double *);
void rgb2nopponent(unsigned char * , int , int , double *);
void rgb2hue(unsigned char * , int , int , double *);

void gaussian_kernel( double *, double  , int  , int  );
void gaussian_sampler( double *  , int , int , double , double , double * , double *, double *);
void conv2(double *, double *, int  , int , int  , int , int , int , double * , double * );
void denseSIFT(double * , int  , int , struct opts , double * , double *  , int , int);

/*----------------------------------------------------------------------------------------------------------------------------------------- */
/*----------------------------------------------------------------------------------------------------------------------------------------- */
/*----------------------------------------------------------------------------------------------------------------------------------------- */

void mexFunction( int nlhs, mxArray *plhs[] , int nrhs, const mxArray *prhs[] )
{  
	unsigned char *im;
	const int *dimim;
	double *I;
	double *feat , *des;
	int ny , nx , nynx , dimcolor = 1 , npts , i , d;
	double	scale_default[1]    = {1};
	double	kernelx_default[25] = {0.0284, 0.0261, 0, -0.0261, -0.0284, 0.1274, 0.1168, 0, -0.1168, -0.1274, 0.2100, 0.1927, 0, -0.1927, -0.2100, 0.1274, 0.1168, 0, -0.1168, -0.1274, 0.0284, 0.0261, 0, -0.0261, -0.0284};
	double	kernely_default[25] = {0.0284, 0.1274, 0.2100, 0.1274, 0.0284, 0.0261, 0.1168, 0.1927, 0.1168, 0.0261, 0, 0, 0, 0, 0, -0.0261, -0.1168, -0.1927, -0.1168, -0.0261, -0.0284, -0.1274, -0.2100, -0.1274, -0.0284};
	double	weightx_default[9]  = {0, 0.1111, 0.5556, 1.0, 0.5556, 0.1111, 0, 0, 0};
	double	weighty_default[9]  = {0, 0.1111, 0.5556, 1.0, 0.5556, 0.1111, 0, 0, 0};
	struct opts options;

	mxArray *mxtemp;
	double *tmp , temp , scalemin = 1.0;
	int tempint , dummy;

	options.nscale        = 1;
	options.sigma_scale   = 0.6;

	options.kyx           = 5;
	options.kxx           = 5;
	options.kxy           = 5;
	options.kyy           = 5;


	options.wyx           = 1;
	options.wxx           = 9;
	options.wyy           = 9;
	options.wxy           = 1;

	options.deltax        = 10;
	options.deltay        = 10;

    options.color         = 0;
    options.nori          = 8;
	options.alpha         = 9;

	options.nbins         = 4;
	options.patchsize     = 9;

	options.norm          = 4;
	options.clamp         = 0.2;
	options.rmmean        = 0;

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
		mxtemp                             = mxGetField( prhs[1], 0, "scale" );
		if(mxtemp != NULL)
		{
			if((mxGetM(mxtemp) != 1)  && (mxGetN(mxtemp) != 2) )
			{
				mexErrMsgTxt("options.scale must be (1 x nscale) in double format");
			}

			options.scale                  = mxGetPr(mxtemp);
			options.nscale                 = (int) mxGetN(mxtemp);

			for (i = 0 ; i < options.nscale ; i++)
			{
				if((options.scale[i] < 0.0) || (options.scale[i] > 1.0))
				{
					mexErrMsgTxt("0.0 <= options.scale(i)  <= 1.0 ");
				}
				scalemin = min(scalemin , options.scale[i]);
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

		mxtemp                            = mxGetField(prhs[1] , 0 , "sigma_scale");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			temp                          = tmp[0];
			if( (temp < 0.0) )
			{
				mexPrintf("sigma_scale > 0 ");	
				options.sigma_scale       = 0.6;
			}
			else
			{
				options.sigma_scale       = temp;
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
			options.kernelx               = (double *)mxMalloc(25*sizeof(double));
			for(i = 0 ; i < 25 ; i++)
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
			options.kernely               = (double *)mxMalloc(25*sizeof(double));
			for(i = 0 ; i < 25 ; i++)
			{
				options.kernely[i]        = kernely_default[i];
			}	
		}

		mxtemp                            = mxGetField( prhs[1], 0, "weightx" );
		if(mxtemp != NULL)
		{
			options.weightx               = mxGetPr(mxtemp);
			options.wyx                   = (int) mxGetM(mxtemp);
			options.wxx                   = (int) mxGetN(mxtemp);
		}

		else
		{
			options.weightx               = (double *)mxMalloc(9*sizeof(double));
			for(i = 0 ; i < 9 ; i++)
			{
				options.weightx[i]        = weightx_default[i];
			}	
		}

		mxtemp                            = mxGetField( prhs[1], 0, "weighty" );
		if(mxtemp != NULL)
		{
			options.weighty               = mxGetPr(mxtemp);
			options.wyy                   = (int) mxGetM(mxtemp);
			options.wxy                   = (int) mxGetN(mxtemp);
		}
		else
		{
			options.weighty               = (double *)mxMalloc(9*sizeof(double));
			for(i = 0 ; i < 9 ; i++)
			{
				options.weighty[i]        = weighty_default[i];
			}	
		}

		mxtemp                            = mxGetField(prhs[1] , 0 , "deltax");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			tempint                       = (int) tmp[0];
			dummy                         = (int) floor(nx*scalemin);

			if( (tempint < 1) || (tempint > dummy) )
			{
				mexPrintf("deltax must be > 0 and deltax < floor(nx*min(scale)), force to 1");	
				options.deltax           = dummy;
			}
			else
			{
				options.deltax         = tempint;
			}
		}

		mxtemp                            = mxGetField(prhs[1] , 0 , "deltay");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			tempint                       = (int) tmp[0];
			dummy                         = (int) floor(ny*scalemin);

			if( (tempint < 1))
			{
				mexPrintf("deltay must be > 0 and deltay < floor(ny*min(scale)), force to 1");	
				options.deltay           = dummy;
			}
			else
			{
				options.deltay         = tempint;
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

		mxtemp                            = mxGetField(prhs[1] , 0 , "alpha");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			tempint                       = (int) tmp[0];
			if( (tempint < 0)  || !(tempint%2) )
			{
				mexPrintf("alpha >0 and odd , force to 9");	
				options.alpha              = 0;
			}
			else
			{
				options.alpha              = tempint;
			}
		}

		mxtemp                            = mxGetField(prhs[1] , 0 , "norm");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			tempint                       = (int) tmp[0];
			if( (tempint < 0) || (tempint > 4) )
			{
				mexPrintf("norm = {0 , 1 , 2 , 3 , 4}, force to 0");	
				options.norm              = 0;
			}
			else
			{
				options.norm              = tempint;
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

		mxtemp                            = mxGetField(prhs[1] , 0 , "nbins");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			tempint                       = (int) tmp[0];
			if( (tempint < 1))
			{
				mexPrintf("nbins > 0, force to 4");	
				options.nbins            = 4;
			}
			else
			{
				options.nbins            = tempint;
			}
		}

		mxtemp                            = mxGetField(prhs[1] , 0 , "patchsize");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			tempint                       = (int) tmp[0];
			if( (tempint < 2*options.nbins+1))
			{
				mexPrintf("patchsize > 2*nbins+1, force to 2*nbins+1");	
				options.patchsize         = 2*options.nbins+1;
			}
			else
			{
				options.patchsize         = tempint;
			}
		}

		mxtemp                            = mxGetField(prhs[1] , 0 , "rmmean");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			tempint                       = (int) tmp[0];

			if( (tempint < 0) || (tempint > 1))
			{
				mexPrintf("rmmean = {0,1}, force to 0\n");	
				options.rmmean            = 0;
			}
			else
			{
				options.rmmean           = tempint;
			}
		}
	}
	else
	{
		options.scale                 = (double *)mxMalloc(options.nscale*sizeof(double));
		for(i = 0 ; i < options.nscale ; i++)
		{
			options.scale[i]          = scale_default[i];
		}	

		options.kernelx               = (double *)mxMalloc(3*sizeof(double));
		options.kernely               = (double *)mxMalloc(3*sizeof(double));

		for(i = 0 ; i < 3 ; i++)
		{
			options.kernelx[i]        = kernelx_default[i];
			options.kernely[i]        = kernely_default[i];
		}	

		options.weightx               = (double *)mxMalloc(8*sizeof(double));
		options.weighty               = (double *)mxMalloc(8*sizeof(double));
		for(i = 0 ; i < 3 ; i++)
		{
			options.weightx[i]        = weightx_default[i];
			options.weighty[i]        = weighty_default[i];
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

	d                  =  options.nbins*options.nbins*options.nori;
	npts               =  options.deltay*options.deltax*options.nscale*dimcolor;

	plhs[0]            =  mxCreateDoubleMatrix(d , npts , mxREAL);
	feat               =  mxGetPr(plhs[0]);

	plhs[1]            =  mxCreateDoubleMatrix(7 , npts , mxREAL);
	des                =  mxGetPr(plhs[1]);

	/*------------------------ Main Call ----------------------------*/

	denseSIFT(I , ny , nx , options , feat , des , npts , dimcolor);

	/*--------------------------- Free memory -----------------------*/

	if ( (nrhs > 1) && !mxIsEmpty(prhs[1]) )
	{
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
		if ( (mxGetField( prhs[1] , 0 , "weightx" )) == NULL )
		{
			mxFree(options.weightx);
		}
		if ( (mxGetField( prhs[1] , 0 , "weighty" )) == NULL )
		{
			mxFree(options.weighty);
		}
	}
	else
	{
		mxFree(options.scale);
		mxFree(options.kernelx);
		mxFree(options.kernely);
		mxFree(options.weightx);
		mxFree(options.weighty);
	}

	mxFree(I);
}

/*----------------------------------------------------------------------------------------------------------------------------------------- */
void denseSIFT(double *I , int ny , int nx , struct opts options , double *feat , double *des  , int npts , int dimcolor)
{
	double *scale = options.scale , *kernelx = options.kernelx , *kernely = options.kernely , *weightx = options.weightx , *weighty = options.weighty;
	int nscale = options.nscale , kyx = options.kyx , kxx = options.kxx , kyy = options.kyy , kxy = options.kxy ;
	int wyx = options.wyx , wxx = options.wxx , wyy = options.wyy , wxy = options.wxy;
	int patchsize = options.patchsize , deltax = options.deltax , deltay = options.deltay;
	int nori = options.nori  , alpha = options.alpha , nbins = options.nbins , norm = options.norm , rmmean = options.rmmean;
	double clamp = options.clamp;

	int s , i , j , x , y ,  a , v , nycurrent , nxcurrent , nynxcurrent , nymax , nxmax , nynxmax , half_patch = (patchsize - 1)/2 , half_patch1 = -half_patch + 1, nynx = ny*nx , vnynx;
	int hmax, nmax  , indi , indj , inda , indx , indy , indf , indd, d = nbins*nbins*nori;
	int nygrady , nxgrady , nygradx , nxgradx;
	int nynew , nxnew  , lynew , lxnew , offsety , offsetx , eta , co , cof;

	double sigma_scale = options.sigma_scale  ;
	double scalecurrent , scalemax = 0.0  , scalemin = 1.0, sigmamax , angle_step = 2*PI/(double)nori , angle , mag , theta , tempx , tempy;
	double cos_theta, sin_theta, tmp , sum;

	double *kernel;
	double *aux  , *I_orientation;
	double *I_filtered;
	double *grady , *gradx , *tempgrady , *tempgradx , *cos_angles , *sin_angles;

	for (s = 0 ; s < nscale ; s++)
	{
		scalemax                     = max(scalemax , scale[s]);
		scalemin                     = min(scalemin , scale[s]);
	}

	nymax                            = (int) floor(ny*scalemax);
	nxmax                            = (int) floor(nx*scalemax);
	nynxmax                          = nymax*nxmax;

	nygrady                          = nymax + max(kyy,wyy) - 1;
	nxgrady                          = nxmax + max(kxy,wxy) - 1;
	nygradx                          = nymax + max(kyx,wyx) - 1;
	nxgradx                          = nxmax + max(kxx,wxx) - 1;

	sigmamax                         = scalemin < 1.0 ? sigma_scale / scalemin : sigma_scale;
	hmax                             = (int) ceil( sigmamax * CTE );
	nmax                             = 1 + 2*hmax;

	kernel                           = (double *)malloc(nmax*sizeof(double));

	I_filtered                       = (double *)malloc(nynxmax*sizeof(double));
	I_orientation                    = (double *)malloc(nynxmax*nori*sizeof(double));

	tempgrady                        = (double *)malloc(nygrady*nxgrady*sizeof(double));
	tempgradx                        = (double *)malloc(nygradx*nxgradx*sizeof(double));
	aux                              = (double *)malloc(nymax*nx*sizeof(double));

	grady                            = (double *)malloc(nynxmax*sizeof(double));
	gradx                            = (double *)malloc(nynxmax*sizeof(double));

	cos_angles                       = (double *)malloc(nori*sizeof(double));
	sin_angles                       = (double *)malloc(nori*sizeof(double));


	for (a = 0 ; a < nori ; a++)
	{
		angle                        = a*angle_step;
		cos_angles[a]                = cos(angle);
		sin_angles[a]                = sin(angle);
	}

	co                               = 0;

	for (v = 0 ; v < dimcolor ; v++)
	{
		vnynx    = v*nynx;
		for (s = 0 ; s < nscale ; s++)
		{
			scalecurrent                     = scale[s];
			if(scalecurrent != 1.0)
			{
				nycurrent                    = (int) floor(ny*scalecurrent);
				nxcurrent                    = (int) floor(nx*scalecurrent);
				gaussian_sampler(I + vnynx , ny , nx , scalecurrent, sigma_scale , kernel , I_filtered , aux);
			}
			else
			{
				nycurrent                    = ny;
				nxcurrent                    = nx;
				for(i = 0 ; i < nynx ; i++)
				{
					I_filtered[i]            = (double) I[i + vnynx];
				}
			}

			nynxcurrent                      = nycurrent*nxcurrent;
			if(rmmean)
			{
				tmp                          = 0.0;
				for (i = 0 ; i < nynxcurrent ; i++)
				{
					tmp                     += I_filtered[i];
				}

				tmp                          /= nynxcurrent;
				for (i = 0 ; i < nynxcurrent ; i++)
				{
					I_filtered[i]           -= tmp;
				}
			}

			nygrady                      = nycurrent + kyy - 1;
			nxgrady                      = nxcurrent + kxy - 1;
			nygradx                      = nycurrent + kyx - 1;
			nxgradx                      = nxcurrent + kyx - 1;

			conv2(I_filtered , kernelx , nycurrent , nxcurrent , kyx , kxx , nygradx , nxgradx , gradx , tempgradx);	
			conv2(I_filtered , kernely , nycurrent , nxcurrent , kyy , kxy , nygrady , nxgrady , grady , tempgrady);	

			for(i = 0 ; i < nxcurrent ; i++)
			{
				indi = i*nycurrent;

				for (j = 0 ; j < nycurrent ; j++)
				{
					indj      = j + indi;

					tempx     = gradx[indj];
					tempy     = grady[indj];

					mag       = sqrt(tempx*tempx + tempy*tempy);
					theta     = atan2(tempy , tempx);

					cos_theta = cos(theta);
					sin_theta = sin(theta);

					for (a = 0 ; a < nori ; a++)
					{
						tmp                  = powf((cos_theta*cos_angles[a] + sin_theta*sin_angles[a]) , alpha); 
						if(tmp > 0)
						{
							I_orientation[indj + a*nynxcurrent] = tmp*mag;
						}
						else
						{
							I_orientation[indj + a*nynxcurrent] = 0.0;
						}
					}
				}
			}

			nygrady                      = nycurrent + wyy - 1;
			nxgrady                      = nxcurrent + wxy - 1;

			nygradx                      = nycurrent + wyx - 1;
			nxgradx                      = nxcurrent + wxx - 1;

			for (a = 0 ; a < nori ; a++)
			{
				inda                     = a*nynxcurrent;
				conv2(I_orientation + inda , weightx , nycurrent , nxcurrent , wyx , wxx , nygradx , nxgradx , gradx                , tempgradx);	
				conv2(gradx                , weighty , nycurrent , nxcurrent , wyy , wxy , nygrady , nxgrady , I_orientation + inda , tempgrady);
			}

			nynew        = max(1 , nycurrent - patchsize);
			nxnew        = max(1 , nxcurrent - patchsize);
			lynew        = (int) floor(nynew/(deltay - 1) ) ;
			lxnew        = (int) floor(nxnew/(deltax - 1) ) ;
			offsety      = max(0 , (int) ( floor((nynew - (deltay - 1)*lynew)/2))) + half_patch - 1;
			offsetx      = max(0 , (int) ( floor((nxnew - (deltax - 1)*lxnew)/2))) + half_patch - 1;

			eta          = (int) floor((double)(patchsize/nbins));

			for(i = 0 ; i < deltax ; i++)
			{
				indi = offsetx + i*lxnew ;

				for (j = 0 ; j < deltay ; j++)
				{
					indj  = offsety + j*lynew;
					indf  = co*d;
					indd  = co*7;	
					cof   = indf;

					for(a = 0 ; a < nori ; a++)
					{		
						inda       = a*nynxcurrent;

						for(x = 0 ; x < nbins ; x++)
						{
							indx = (half_patch1 + x*eta + indi)*nycurrent + inda + indj;
							for(y = 0 ; y < nbins ; y++)
							{
								indy      = half_patch1 + y*eta ;
								feat[cof] = I_orientation[indy + indx];
								cof++;
							}
						}
					}

					tmp  = 0.0;
					if(norm == 1)
					{
						cof   = indf;
						for(x = cof ; x < (cof + d) ; x++)
						{
							tmp += feat[x];
						}

						sum = 1.0/(tmp + tiny);
						for(x = cof ; x < (cof + d) ; x++)
						{
							feat[x] *= sum;
						}
					}

					if(norm == 2)
					{
						cof   = indf;
						for(x = cof ; x < (cof + d) ; x++)
						{
							tmp   += (feat[x]*feat[x]);
						}

						sum  = 1.0/sqrt(tmp + verytiny);
						for(x = cof ; x < (cof + d) ; x++)
						{
							feat[x]  *= sum;
						}
					}

					if(norm == 3)
					{
						cof   = indf;
						for(x = cof ; x < (cof + d) ; x++)
						{
							tmp   += feat[x];
						}

						sum  = 1.0/(tmp + tiny);
						for(x = cof ; x < (cof + d) ; x++)
						{
							feat[x]  *= sqrt(feat[x]*sum);
						}
					}

					if(norm == 4)
					{
						cof   = indf;
						for(x = cof ; x < (cof + d) ; x++)
						{
							tmp   += (feat[x]*feat[x]);
						}

						sum  = 1.0/sqrt(tmp + verytiny);
						for(x = cof ; x < (cof + d) ; x++)
						{
							feat[x]  *= sum;
							if(feat[x] > clamp)
							{
								feat[x] = clamp;
							}
						}

						sum  = 0.0;
						for(x = cof ; x < (cof + d) ; x++)
						{
							sum   += (feat[x]*feat[x]);
						}
						sum  = 1.0/sqrt(sum + verytiny);

						for(x = cof ; x < (cof + d) ; x++)
						{
							feat[x]  *= sum;
						}
					}

					des[0 + indd] = (double)(indj + 1);
					des[1 + indd] = (double)(indi + 1);
					des[2 + indd] = scalecurrent;
					des[3 + indd] = (double) (v+1);
					des[4 + indd] = (double) (nycurrent);
					des[5 + indd] = (double) (nxcurrent);
					des[6 + indd] = tmp;
					co++;
				}
			}
		}
	}

	free(I_filtered);
	free(I_orientation);
	free(gradx);
	free(grady);
	free(tempgrady);
	free(tempgradx);
	free(kernel);
	free(aux);
	free(cos_angles);
	free(sin_angles);
}


/*----------------------------------------------------------------------------------------------------------------------------------------- */
/*----------------------------------------------------------------------------------------------------------------------------------------- */
/*----------------------------------------------------------------------------------------------------------------------------------------- */
/*----------------------------------------------------------------------------------------------------------------------------------------- */
void gaussian_sampler( double *I  , int ny , int nx , double scalecurrent, double sigma_scale, double *kernel , double *I_filtered , double *aux)
{
	int h , n;
	int nynew ,  nxnew , indx , indxnew ;
	int x,y,xc,yc,i,j;
	double xx,yy,sum;
	int nx2 = 2 * nx;
	int ny2 = 2 * ny;
	double sigmacurrent , tempscale = 1.0/scalecurrent;

	nynew                        = (int) floor(ny*scalecurrent);
	nxnew                        = (int) floor(nx*scalecurrent);

	sigmacurrent                 = scalecurrent < 1.0 ? sigma_scale / scalecurrent : sigma_scale;

	h                            = (int) ceil( sigmacurrent * CTE );
	n                            = 1 + 2*h;

    gaussian_kernel( kernel , sigmacurrent , h , n );

	for(y=0 ; y < nynew ; y++)
	{
		yy      = (double) y * tempscale;          
		yc      = (int) floor( yy + 0.5 ); 

		indx    = 0;
		indxnew = 0;

		for(x=0 ; x < nx ;x++)
		{
			sum      = 0.0;
			for(i = 0 ; i < n ; i++)
			{
				j                = yc - h + i;
				while(j<0)    j +=ny2;
				while(j>=ny2) j -=ny2;
				if(j>=ny)     j  = ny2-1-j;
				sum             += I[ j + indx ] * kernel[i];

			}
			aux[y + indxnew ] = sum;
			indx             += ny;
			indxnew          += nynew;
		}
	}

	indx =  0;
	for(x=0 ; x < nxnew ; x++)
	{
		xx   = (double) x * tempscale;           
		xc   = (int) floor( xx + 0.5 ); 

		for(y=0 ; y < nynew ; y++)
		{
			sum = 0.0;
			for(i=0 ; i < n ; i++)
			{
				j                = xc - h + i;
				while(j<0)    j +=nx2;
				while(j>=nx2) j -=nx2;
				if(j>=nx)     j  = nx2-1-j;
				sum             += aux[ y + j * nynew ] * kernel[i];
			}
			I_filtered[ y + indx ] = sum;
		}
		indx    += nynew;
	}
}

/*----------------------------------------------------------------------------------------------------------------------------------------- */
/*----------------------------------------------------------------------------------------------------------------------------------------- */
/*----------------------------------------------------------------------------------------------------------------------------------------- */
void gaussian_kernel( double *kernel, double sigma , int h , int n )
{
	int i;
	double val , temp_sigma = 1.0/sigma;
	double sum = 0.0;

	/* compute gaussian kernel */
	for(i = 0 ; i < n ; i++)
	{
		val       = (double)( i - h ) * temp_sigma;
		kernel[i] = exp( -0.5 * val * val );
		sum      += kernel[i];
	}

	sum = 1.0/sum;
	/* normalization */
	for(i=0 ; i < n ; i++) 
	{
		kernel[i] *= sum;
	}
}

/*----------------------------------------------------------------------------------------------------------------------------------------- */
/*----------------------------------------------------------------------------------------------------------------------------------------- */
/*----------------------------------------------------------------------------------------------------------------------------------------- */
void conv2(double *A, double *B, int nyA , int nxA , int nyB , int nxB , int nyD , int nxD , double *C , double *D)
{
	int p , q , index; 
	int lengthA = nyA*nxA , lengthB = nyB*nxB , lengthD = nyD*nxD , nyA1 = nyA - 1 , nyB1 = nyB - 1;
	int nyAtemp , nxAtemp ,  nyBtemp , nxBtemp , nyDtemp , nxDtemp;
	int nytemp = (int) (ceil((1.0 + nyB)*0.5) - 1) , nxtemp = (int) (ceil((1.0 + nxB)*0.5) - 1);


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