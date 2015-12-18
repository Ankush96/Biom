
/*

denseMBLBP extract Histograms of Multi-Block LBP on a regular dense grid and computed on image I after color projection


Usage
------

[dmblbp , infodmblbp] =  denseMBLBP(I , [options] );


Inputs
-------

I                              Input image (ny x nx x [3]) in UINT8 format. 
options 

       scale                   Multi-Scale vector (1 x nscale) (default scale = 1) where scale(i) = s is the size's factor to apply to each 9 blocks
                               in the LBP computation, i = 1,...,nscale

	   deltax                  Division step in the x-axis for the grid (default deltax = floor(nx*min(scale))) 
	   deltay                  Division step in the y-axis for the grid (default deltay = floor(ny*min(scale)))

       color                   0 : force gray-scale (dimcolor = 1, default), 1 : RGB-LBP (dimcolor = 3), 2 : nRGB-LBP (dimcolor = 3), 3 : Opponent-LBP (dimcolor = 3), 
                               4 : nOpponent-LBP (dimcolor = 2), 5 : Hue-LBP (dimcolor = 1)

	   patchsize               Size of the patch where the descriptor is computed (default patchsize = 16 )	  

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

       norm                    Normalization : norm = 0 <=> no normalization, norm = 1 <=> v=v/(sum(v)+epsi), norm = 2 <=> v=v/sqrt(sum(v²)+epsi²), 
                               norm = 3 <=> v=sqrt(v/(sum(v)+epsi)) , norm = 3 <=> L2-clamped (default norm = 1)

       clamp                   Clamping value (default clamp = 0.2)


Outputs
-------

dmblbp                         MBLBP patches descriptors (nbin x nb_pts) nb_pts = deltax*deltay*nscale*dimcolor 
                               in double format where nbin = ([256,59,10]*(improvedLBP+1)) (depending on improvedLBP) 
							   

infodmblbp                     MBLBP patches informations(6 x nb_pts) where nb_pts = deltax*deltay*nscale*dimcolor
                               infodmblbp(1,i) = y
						       infodmblbp(2,i) = x
							   infodmblbp(3,i) = scale
							   infodmblbp(4,i) = color    
							   infodmblbp(5,i) = nyscale                 
							   infodmblbp(6,i) = nxscale                


To compile
----------


mex  -g -output denseMBLBP.dll denseMBLBP.c libfftw_x32.lib.lib

Link with FFTW

mex  -f mexopts_intel10.bat -output denseMBLBP.dll denseMBLBP.c libfftw_x32.lib

Link with MKL32

mex  -f mexopts_intel10.bat -output denseMBLBP.dll denseMBLBP.c "C:\Program Files\Intel\Compiler\11.1\065\mkl\ia32\lib\mkl_core.lib" "C:\Program Files\Intel\Compiler\11.1\065\mkl\ia32\lib\mkl_intel_c.lib" "C:\Program Files\Intel\Compiler\11.1\065\mkl\ia32\lib\mkl_intel_thread.lib"

Link with MKL64

mex -f mexopts_intel11_64.bat -output denseMBLBP.mexw64 denseMBLBP.c "C:\Program Files (x86)\Intel\Compiler\11.1\065\mkl\em64t\lib\mkl_core.lib" "C:\Program Files (x86)\Intel\Compiler\11.1\065\mkl\em64t\lib\mkl_intel_lp64.lib" "C:\Program Files (x86)\Intel\Compiler\11.1\065\mkl\em64t\lib\mkl_intel_thread.lib"




Example 1
---------

I                     = imread('02769_Right_StudentOffice.jpeg');
%I                     = imread('image_0174.jpg');

options.scale         = [1 , 2];
options.deltax        = 10;
options.deltay        = 10;
options.color         = 0;
options.patchsize     = 16;
options.maptable      = 0;
options.useFF         = 0;
options.improvedLBP   = 0;
options.rmextremebins = 1;
options.norm          = 4;
options.clamp         = 0.2;


[dmblbp , infodmblbp] =  denseMBLBP(I , [options] );

figure(1)
imagesc(dmblbp)


figure(2)
imagesc(I)
colormap(gray)
hold on
plot(infodmblbp(2 , :) , infodmblbp(1 , :) , 'r+')
hold off


Example 2
---------

I                     = imread('image_0174.jpg');

options.scale         = [1,2,3];
options.deltax        = 10;
options.deltay        = 10;
options.color         = 0;
options.patchsize     = 22;
options.maptable      = 0;
options.useFF         = 0;
options.improvedLBP   = 0;
options.rmextremebins = 1;
options.norm          = 4;
options.clamp         = 0.2;


[dmblbp , infodmblbp] =  denseMBLBP(I , options );

half                  = options.patchsize/2;

xr                    = [infodmblbp(2, :)-half ; infodmblbp(2, :)-half ; infodmblbp(2, :)+ half ; infodmblbp(2, :)+ half ; infodmblbp(2, :)-half] + 1.5;
yr                    = [infodmblbp(1, :)-half ; infodmblbp(1, :)+half ; infodmblbp(1, :)+ half ; infodmblbp(1, :)- half ; infodmblbp(1, :)-half] + 1.5;


figure(1)
imagesc(dmblbp)


figure(2)
imagesc(I)
colormap(gray)
hold on
plot(infodmblbp(2 , :)+1.5 , infodmblbp(1 , :)+1.5 , 'r+')
plot(xr , yr , 'b')
hold off





Author : Sébastien PARIS : sebastien.paris@lsis.org
-------  Date : 09/06/2010

References [1] Timo Ahonen, Jiri Matas, Chu He, and Matti Pietikäinen ,
---------      "Rotation Invariant Image Description with Local Binary Pattern Histogram Fourier Features"
           


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
	double *kernelx;
	int    kyx;
	int    kxx;
	double *kernely;
	int    kyy;
	int    kxy;
	int    deltax;
	int    deltay;

	int    color;
	int    patchsize;
	int    maptable;
    int    useFF;
	int    improvedLBP;
	int    rmextremebins;
	int    norm;
	double clamp;
};


/*-------------------------------------------------------------------------------------------------------------- */

/* Function prototypes */

void rgb2gray(unsigned char * , int , int , double *);
void rgb2nrgb(unsigned char * , int , int , double *);
void rgb2opponent(unsigned char * , int , int , double *);
void rgb2nopponent(unsigned char * , int , int , double *);
void rgb2hue(unsigned char * , int , int , double *);


void MakeIntegralImage(double *, double *, int , int , double *);
double Area(double * , int , int , int , int , int );
void conv2(double *, double *, int  , int , int  , int , int , int , double * , double * );
void compute_hmblbp(double * , int , int , int , int , int , int ,  int , double *, int , int *, struct opts);
void compute_hmblbpff(double *, int , int , int , int , int , int , int , double *, int , int *, fftw_complex *, fftw_plan , double * , struct opts);
void denseMBLBP(double * , int  , int , struct opts , double * , double *  , int , int);

/*-------------------------------------------------------------------------------------------------------------- */
#ifdef MATLAB_MEX_FILE
void mexFunction( int nlhs, mxArray *plhs[] , int nrhs, const mxArray *prhs[] )
{  

	unsigned char *im;
	const int *dimim;
	double *I;
	double *feat , *des;

	double	kernelx_default[1]  = {1};
	double	kernely_default[1]  = {1};
	int i , ny , nx , nynx , dimcolor = 1  , npts ,  npt_ker = 1 , Nbins;
	struct opts options;
	mxArray *mxtemp;
	double *tmp , temp , scalemin = 1.0;
	int tempint , dummy;


	options.nscale        = 1;
	options.kyx           = 1;
	options.kxx           = 1;
	options.kxy           = 1;
	options.kyy           = 1;
	options.deltax        = 10;
	options.deltay        = 10;
	options.color         = 0;
	options.patchsize     = 16;
	options.maptable      = 0;
    options.useFF         = 0;
	options.improvedLBP   = 0;
	options.rmextremebins = 1;
	options.norm          = 0;
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

		mxtemp                            = mxGetField(prhs[1] , 0 , "deltax");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			tempint                       = (int) tmp[0];
			dummy                         = (int) floor(nx*scalemin);

			if( (tempint < 1) || (tempint > dummy) )
			{
				mexPrintf("deltax must be > 0 and floor(nx*min(scale)), force to 1");	
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
				mexPrintf("deltay must be > 0 and floor(ny*min(scale)), force to 1");	
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

		mxtemp                            = mxGetField(prhs[1] , 0 , "patchsize");
		if(mxtemp != NULL)
		{
			tmp                           = mxGetPr(mxtemp);
			tempint                       = (int) tmp[0];
			if( (tempint < 1))
			{
				mexPrintf("patchsize > 0, force to 9");	
				options.patchsize         = 9;
			}
			else
			{
				options.patchsize         = tempint;
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
			tmp                           = mxGetPr(mxtemp);
			tempint                       = (int) tmp[0];
			if( (tempint < 0) || (tempint > 4) )
			{
				mexPrintf("norm = {0,1,2,3,4}, force to 0\n");	
				options.norm             = 0;
			}
			else
			{
				options.norm             = tempint;
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

	if(options.improvedLBP)
	{
		Nbins         *= 2;
	}

	/*----------------------- Outputs -------------------------------*/

	npts               =  options.deltay*options.deltax*options.nscale*dimcolor;

	plhs[0]            =  mxCreateDoubleMatrix(Nbins , npts , mxREAL);
	feat               =  mxGetPr(plhs[0]);

	plhs[1]            =  mxCreateDoubleMatrix(6 , npts , mxREAL);
	des                =  mxGetPr(plhs[1]);

	/*------------------------ Main Call ----------------------------*/

	denseMBLBP(I ,  ny , nx  , options , feat , des , npts , dimcolor );

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
	}
	else
	{
		mxFree(options.scale);
		mxFree(options.kernelx);
		mxFree(options.kernely);
	}

	mxFree(I);
}

#else


#endif

/*----------------------------------------------------------------------------------------------------------------------------------------- */
void denseMBLBP(double *I , int ny , int nx , struct opts options , double *feat , double *des  , int npts , int dimcolor)
{
	double *kernelx = options.kernelx , *kernely = options.kernely;
	double *scale = options.scale;
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

	int i , l , m , v , s;
	int nynx = ny*nx , vnynx , coscale = 0 , nscale = options.nscale , improvedLBP = options.improvedLBP;
	int kyy = options.kyy , kxy = options.kxy , kyx = options.kyx , kxx = options.kxx  , maptable = options.maptable;
	int useFF = options.useFF , patchsize = options.patchsize , half_patch = (patchsize - 1)/2 , half_patch1 = -half_patch + 1, scalecurrent;
	int deltay=options.deltay, deltax=options.deltax;
	double tempx , tempy , temp;
	int origy, origx;
	int nygrady , nxgrady , nygradx , nxgradx , noconv ;
	int NBINS , Nbins;
	int nynew , nxnew  , lynew , lxnew , offsety , offsetx , co  , indd;

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
		Nbins          = 36;
		NBINS          = 36;
	}
	else if(maptable == 3)
	{
		Nbins          = 10;
		NBINS          = 10;
	}

	if(improvedLBP)
	{
		Nbins          *= 2;
		NBINS          *= 2;
	}
	if(useFF)
	{
		work    = fftw_malloc(8*sizeof(fftw_complex)); 
		plan    = fftw_plan_dft_1d(8 , work, work , FFTW_FORWARD, FFTW_ESTIMATE); 
		fftw_execute(plan); 
		Htemp   = (double *) malloc(NBINS*sizeof(double));
	}

	table       = (int *) malloc((256*(improvedLBP+1))*sizeof(int));

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
			i = l*10;

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

	co          = 0;
	if(useFF)
	{
		for (v = 0 ; v < dimcolor ; v++)
		{
			vnynx    = v*nynx;
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
			for (s = 0 ; s < nscale ; s++)
			{
				scalecurrent = scale[s];

				nynew        = max(1 , ny - patchsize);
				nxnew        = max(1 , nx - patchsize);
				lynew        = (int) floor(nynew/(deltay - 1) );
				lxnew        = (int) floor(nxnew/(deltax - 1) );
				offsety      = max(0 , (int) ( floor((nynew - (deltay - 1)*lynew)/2))) + half_patch - 1;
				offsetx      = max(0 , (int) ( floor((nxnew - (deltax - 1)*lxnew)/2))) + half_patch - 1;

				for(l = 0 ; l < deltax ; l++) /* Loop shift on x-axis  */
				{
					origx  = offsetx + l*lxnew;
					for(m = 0 ; m < deltay ; m++)   /* Loop shift on y-axis  */
					{
						origy         = offsety + m*lynew;
						indd          = co*6;	

						compute_hmblbpff(II , ny , nx , patchsize , patchsize , origy + half_patch1 , origx +  half_patch1 , scalecurrent , feat + coscale , NBINS , table , work , plan , Htemp , options);

						des[0 + indd] = (double)(origy + 1);
						des[1 + indd] = (double)(origx + 1);
						des[2 + indd] = scalecurrent;
						des[3 + indd] = (double) (v+1);
					    des[4 + indd] = (double) (ny);
					    des[5 + indd] = (double) (nx);
						coscale       += Nbins;
						co++;
					}
				}
			}
		}
	}
	else
	{
		for (v = 0 ; v < dimcolor ; v++)
		{
			vnynx    = v*nynx;

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

			for (s = 0 ; s < nscale ; s++)
			{
				scalecurrent = scale[s];
				nynew        = max(1 , ny - patchsize);
				nxnew        = max(1 , nx - patchsize);
				lynew        = (int) floor(nynew/(deltay - 1) );
				lxnew        = (int) floor(nxnew/(deltax - 1) );
				offsety      = max(0 , (int) ( floor((nynew - (deltay - 1)*lynew)/2))) + half_patch - 1;
				offsetx      = max(0 , (int) ( floor((nxnew - (deltax - 1)*lxnew)/2))) + half_patch - 1;

				for(l = 0 ; l < deltax ; l++) /* Loop shift on x-axis  */
				{
					origx  = offsetx + l*lxnew;

					for(m = 0 ; m < deltay ; m++)   /* Loop shift on y-axis  */
					{
						origy         = offsety + m*lynew;
						indd          = co*6;	

						compute_hmblbp(II , ny , nx , patchsize , patchsize , origy +  half_patch1 , origx +  half_patch1 , scalecurrent , feat + coscale , Nbins , table , options);

						des[0 + indd] = (double)(origy + 1);
						des[1 + indd] = (double)(origx + 1);
						des[2 + indd] = (double) scalecurrent;
						des[3 + indd] = (double) (v+1);
						des[4 + indd] = (double) (ny);
						des[5 + indd] = (double) (nx);
						co++;
						coscale      += Nbins ;

					}
				}
			}
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

/*----------------------------------------------------------------------------------------------------------------------------------------- */
void compute_hmblbpff(double *II , int ny , int nx , int sy , int sx , int origy , int origx, int currentscale  , double *H , int NBINS , int *table , fftw_complex *work , fftw_plan plan , double *Htemp , struct opts options )
{
	int i , j , l , xc , yc , xnw , ynw , xse , yse , co1 , co2;
	int norm = options.norm;
	int improvedLBP = options.improvedLBP , rmextremebins = options.rmextremebins;
	int Nbins = 38;

	double clamp = options.clamp;
	double sum, temp , re , im;
	double Ac , tmpA , sumA;
	unsigned short int valF;

	if(improvedLBP)
	{
		Nbins      *= 2;
	}

	for (i = 0 ; i < NBINS ; i++)
	{
		Htemp[i] = 0.0;
	}

	for (xc = currentscale + origx ; xc < sx - 2*currentscale + origx ; xc++)
	{
		for (yc = currentscale + origy ; yc < sy - 2*currentscale + origy ; yc++)
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
	co2                = 0;

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

/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
void compute_hmblbp(double *II , int ny , int nx , int sy , int sx , int origy , int origx ,  int currentscale  , double *H , int NBINS , int *table , struct opts options )
{
	int i , xc , yc , xnw , ynw , xse , yse;
	int norm = options.norm;
	int improvedLBP = options.improvedLBP , rmextremebins = options.rmextremebins ;
	int Nbins = NBINS;
	double clamp = options.clamp;
	double sum, temp;
	double Ac , tmpA , sumA;
	unsigned short int valF;

	if(improvedLBP)
	{
		Nbins      = NBINS/2;
	}

	for (xc = currentscale + origx ; xc < sx - 2*currentscale + origx ; xc++)
	{
		for (yc = currentscale + origy ; yc < sy - 2*currentscale + origy ; yc++)
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
			H[table[valF]]++;
		}
	}

	if(norm == 1)
	{

		sum       = 0.0;
		for(i = 0 ; i < NBINS  ; i++)
		{
			sum   += H[i];
		}

		sum = 1.0/(sum + tiny);
		for(i = 0 ; i < NBINS  ; i++)
		{
			H[i]   *= sum;
		}
	}

	if(norm == 2)
	{
		sum       = 0.0;
		for(i = 0 ; i < NBINS  ; i++)
		{
			temp   = H[i];
			sum   += temp*temp;
		}

		sum = 1.0/sqrt(sum + verytiny);
		for(i = 0 ; i < NBINS  ; i++)
		{
			H[i]   *= sum;
		}
	}

	if(norm == 3)
	{
		sum       = 0.0;
		for(i = 0 ; i < NBINS  ; i++)
		{
			sum   += H[i];
		}

		sum = 1.0/(sum + tiny);

		for(i = 0 ; i < NBINS  ; i++)
		{
			H[i]   = sqrt(H[i]*sum);
		}
	}

	if(norm == 4)
	{

		sum       = 0.0;
		for(i = 0 ; i < NBINS  ; i++)
		{
			temp   = H[i];
			sum   += temp*temp;
		}

		sum = 1.0/sqrt(sum + verytiny);
		for(i = 0 ; i < NBINS  ; i++)
		{
			H[i]   *= sum;

			if(H[i] > clamp)
			{
				H[i] = clamp;
			}
		}

		sum       = 0.0;
		for(i = 0 ; i < NBINS  ; i++)
		{
			temp   = H[i];
			sum   += temp*temp;
		}

		sum = 1.0/sqrt(sum + verytiny);

	for(i = 0 ; i < NBINS  ; i++)
		{
			H[i]   *= sum;
		}
	}

	if(rmextremebins == 1)
	{
		if(improvedLBP)
		{
			H[0] = H[Nbins-1] = H[Nbins] = H[NBINS-1] = 0.0;
		}
		else
		{
			H[0] = H[Nbins-1] = 0.0;
		}
	}
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

