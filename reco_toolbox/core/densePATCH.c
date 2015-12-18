
/*

densePATCH extract patches of pixels after color projection on a regular dense grid

Usage
------

[dpatch , infodpatch] = densePATCH(I , [options] );


Inputs
-------

I                                     Input image (ny x nx x [3]) in UINT8 format. 

options
	   scale                          Scaling vector (1 x nscale). Extract descriptors at different scaling of the image (default scale = [1]).
	   sigma_scale                    Scaling factor to obtain the standard deviation of the Gaussian filter (sigma = sigma_scale/scale)(default sigma_scale = 0.6)
	   deltax                         Division step in the x-axis for the grid (default deltax = floor(nx*min(scale))) 
	   deltay                         Division step in the y-axis for the grid (default deltay = floor(ny*min(scale)))
       color                          0 : force gray-scale (dimcolor = 1, default), 1 : RGB (dimcolor = 3), 2 : nRGB (dimcolor = 3), 3 : Opponent (dimcolor = 3), 
                                      4 : nOpponent (dimcolor = 2), 5 : Hue (dimcolor = 1)
	   patchsize                      Size of the patch where the descriptor is computed (default patchsize = 9 )	  
	   norm                           Normalization : norm = 0 <=> no normalization, norm = 1 <=> v=v/(sum(v)+epsi), norm = 2 <=> v=v/sqrt(sum(v²)+epsi²), 
	                                  norm = 3 <=> v=sqrt(v/(sum(v)+epsi)) , norm = 3 <=> L2-clamped (default norm = 1)
	   clamp                          Clamping value (default clamp = 0.2)


Outputs
-------

dpatch                                PATCH descriptors (nbins x nb_pts) where nb_pts = deltax*deltay*nscale*dimcolor
infodpatch                            PATCH descriptors informations(6 x nb_pts)   where nb_pts = deltax*deltay*nscale*dimcolor
                                      infodpatch(1,i) = y
									  infodpatch(2,i) = x
									  infodpatch(3,i) = scale
									  infodpatch(4,i) = color
									  infodpatch(5,i) = nyscale
									  infodpatch(6,i) = nxscale



Example 1
---------


I                                    = imread('02769_Right_StudentOffice.jpeg');


options.scale                        = [0.5 , 0.75 , 1];
options.sigma_scale                  = 0.6;
options.deltax                       = 10;
options.deltay                       = 10;
options.color                        = 3;
options.patchsize                    = 24;
options.norm                         = 4;
options.clamp                        = 0.2;


[dpatch , infodpatch]                 = densePATCH(I , options ); 

figure(1)
imagesc(dpatch)


figure(2)
imagesc(I)
colormap(gray)
hold on
plot(infodpatch(2 , :) , infodpatch(1 , :) , 'r+')
hold off




Example 2
---------


I                                    = imread('image_0174.jpg');


options.scale                        = 1;
options.sigma_scale                  = 0.6;
options.deltax                       = 10;
options.deltay                       = 10;
options.color                        = 0;
options.patchsize                    = 16;
options.norm                         = 4;
options.clamp                        = 0.2;


[dpatch , infodpatch]                = densePATCH(I , options ); 

half                                 = options.patchsize/2;
xr                                   = [infodpatch(2, :)-half ; infodpatch(2, :)-half ; infodpatch(2, :)+ half ; infodpatch(2, :)+ half ; infodpatch(2, :)-half] + 1.5;
yr                                   = [infodpatch(1, :)-half ; infodpatch(1, :)+half ; infodpatch(1, :)+ half ; infodpatch(1, :)- half ; infodpatch(1, :)-half] + 1.5;

figure(1)
imagesc(I)
colormap(gray)
hold on
plot(infodpatch(2 , :)+1.5 , infodpatch(1 , :)+1.5 , 'r+')
plot(xr , yr , 'b')
hold off

figure(2)
display_dico(dpatch)
colormap(gray)



To compile
----------

mex  -output densePATCH.dll densePATCH.c

mex  -g -output densePATCH.dll densePATCH.c

mex  -f mexopts_intel10.bat -output densePATCH.dll densePATCH.c


Author : Sébastien PARIS : sebastien.paris@lsis.org
-------  Date : 02/04/2010


References :  
---------        


*/


#include <time.h>
#include <math.h>
#include <mex.h>


#define tiny 1e-8
#define verytiny 1e-15
#define PI 3.14159265358979323846
#define PIhalf2 1.5707963267948966
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

	int    patchsize;
	int    deltax;
	int    deltay;

	int    color;

	int    norm;
	double clamp;

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
void densePATCH(double * , int  , int , struct opts , double * , double *  , int , int);


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
	struct opts options;
	mxArray *mxtemp;
	double *tmp , temp , scalemin = 1.0;
	int tempint , dummy;

	options.nscale        = 1;
	options.sigma_scale   = 0.6;
	options.deltax        = 10;
	options.deltay        = 10;
    options.color         = 0;
	options.patchsize     = 16;
	options.norm          = 1;
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
	}
	else
	{
		options.scale                 = (double *)mxMalloc(options.nscale*sizeof(double));
		for(i = 0 ; i < options.nscale ; i++)
		{
			options.scale[i]          = scale_default[i];
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

	d                  =  options.patchsize*options.patchsize;
	npts               =  options.deltay*options.deltax*options.nscale*dimcolor;

	plhs[0]            =  mxCreateDoubleMatrix(d , npts , mxREAL);
	feat               =  mxGetPr(plhs[0]);

	plhs[1]            =  mxCreateDoubleMatrix(6 , npts , mxREAL);
	des                =  mxGetPr(plhs[1]);


	/*------------------------ Main Call ----------------------------*/

	densePATCH(I , ny , nx , options , feat , des , npts , dimcolor);

	/*--------------------------- Free memory -----------------------*/

	if ( (nrhs > 1) && !mxIsEmpty(prhs[1]) )
	{
		if ( (mxGetField( prhs[1] , 0 , "scale" )) == NULL )
		{
			mxFree(options.scale);
		}
	}
	else
	{
		mxFree(options.scale);
	}
	mxFree(I);

}

/*----------------------------------------------------------------------------------------------------------------------------------------- */
void densePATCH(double *I , int ny , int nx , struct opts options , double *feat , double *des  , int npts , int dimcolor)
{
	double *scale = options.scale ;
	int nscale = options.nscale;
	int patchsize = options.patchsize , deltax = options.deltax , deltay = options.deltay;
	int norm = options.norm , d = patchsize*patchsize;
	double clamp = options.clamp;
	int s , i , j , x , y , v , nycurrent , nxcurrent , nymax , nxmax , nynxmax , half_patch = (patchsize - 1)/2 , half_patch1 = -half_patch + 1, nynx = ny*nx , vnynx;
	int hmax, nmax  , indi , indj , indx  , indf , indd;
	int nynew , nxnew  , lynew , lxnew , offsety , offsetx , co , cof;
	double sigma_scale = options.sigma_scale  ;
	double scalecurrent , scalemax = 0.0  , scalemin = 1.0, sigmamax , tmp , sum;
	double *kernel , *I_filtered, *aux;


	for (s = 0 ; s < nscale ; s++)
	{
		scalemax                     = max(scalemax , scale[s]);
		scalemin                     = min(scalemin , scale[s]);

	}

	nymax                            = (int) floor(ny*scalemax);
	nxmax                            = (int) floor(nx*scalemax);
	nynxmax                          = nymax*nxmax;


	sigmamax                         = scalemin < 1.0 ? sigma_scale / scalemin : sigma_scale;
	hmax                             = (int) ceil( sigmamax * CTE );
	nmax                             = 1 + 2*hmax;

	kernel                           = (double *)malloc(nmax*sizeof(double));
	I_filtered                       = (double *)malloc(nynxmax*sizeof(double));
	aux                              = (double *)malloc(nymax*nx*sizeof(double));

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

			nynew        = max(1 , nycurrent - patchsize);
			nxnew        = max(1 , nxcurrent - patchsize);
			lynew        = (int) floor(nynew/(deltay - 1) );
			lxnew        = (int) floor(nxnew/(deltax - 1) );
			offsety      = max(0 , (int) ( floor((nynew - (deltay - 1)*lynew)/2))) + half_patch - 1;
			offsetx      = max(0 , (int) ( floor((nxnew - (deltax - 1)*lxnew)/2))) + half_patch - 1;

			for(i = 0 ; i < deltax ; i++)
			{
				indi = offsetx + i*lxnew ;
				for (j = 0 ; j < deltay ; j++)
				{
					indj  = offsety + j*lynew;
					indf  = co*d;
					indd  = co*6;	
					cof   = indf;

					for(x = 0 ; x < patchsize ; x++)
					{
						indx = (half_patch1 + x + indi)*nycurrent + indj;
						for(y = 0 ; y < patchsize ; y++)
						{
							feat[cof] = I_filtered[y + half_patch1 + indx];
							cof++;
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
					co++;
				}
			}
		}
	}

	free(I_filtered);
	free(kernel);
	free(aux);
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