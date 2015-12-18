
/*

  RGB2GRAY of image I

  Usage
  ------

  gray                                  = rgb2gray(rgb);

  
  Inputs
  -------

  rgb                                  RGB image (Ny x Nx x 3) in UINT8 format 
   

  Outputs
  -------
  
  gray                                 Image in gray scale (Ny x Nx) in UINT8 format.                              

  To compile
  ----------


  mex  -g -output rgb2gray.dll rgb2gray.c

  mex  -f mexopts_intel10.bat -output rgb2gray.dll rgb2gray.c


  Example 1
  ---------

  
  clear, close all
  rgb   = imread('class57.jpg');
  I     = rgb2gray(rgb); 

  figure(1)
  imagesc(I)
  colormap(gray)





 Author : Sébastien PARIS : sebastien.paris@lsis.org
 -------  Date : 01/20/2009

 Reference ""


*/


#include <time.h>
#include <math.h>
#include <mex.h>


/* Function prototypes */
/*-------------------------------------------------------------------------------------------------------------- */

void rgb2gray(unsigned char * , int , int  , unsigned char *);

/*-------------------------------------------------------------------------------------------------------------- */



void mexFunction( int nlhs, mxArray *plhs[] , int nrhs, const mxArray *prhs[] )
{    
	unsigned char *rgb;
	const int *dimsrgb;
	int *dimsgray;
	int numdimsrgb  , Ny , Nx;
    unsigned char *gray;

    /* Input 1  */
	    
    if ((nrhs > 0) && !mxIsEmpty(prhs[0]) && mxIsUint8(prhs[0]))       
    {        	
		dimsrgb    = mxGetDimensions(prhs[0]);
        numdimsrgb = mxGetNumberOfDimensions(prhs[0]);
		
		rgb        = (unsigned char *)mxGetData(prhs[0]);
		Ny         = dimsrgb[0];
		Nx         = dimsrgb[1];
		if(numdimsrgb > 2)
		{
			if(dimsrgb[2] != 3)
			{		
				mexErrMsgTxt("rgb must be (Ny x Nx x 3) in UINT8 format");	
			}
		}
		else
		{
			mexErrMsgTxt("rgb must be (Ny x Nx x 3) in UINT8 format");	
		}		   
    }
	else
	{	
		mexErrMsgTxt("rgb must be (Ny x Nx x 3) in UINT8 format");	
	}
    
    /* Output 1  */

	dimsgray         = (int *)mxMalloc(2*sizeof(int));
	dimsgray[0]      = Ny;
	dimsgray[1]      = Nx;
	plhs[0]          = mxCreateNumericArray(2 , dimsgray , mxUINT8_CLASS , mxREAL);

	gray             = (unsigned char *)mxGetData(plhs[0]);
	    
    /*------------------------ Main Call ----------------------------*/
	
	rgb2gray(rgb , Ny , Nx , gray);

	/*----------------- Free Memory --------------------------------*/
		
	mxFree(dimsgray);

}

/*----------------------------------------------------------------------------------------------------------------------------------------- */
void rgb2gray(unsigned char *rgb , int Ny , int Nx , unsigned char *gray)
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
/*----------------------------------------------------------------------------------------------------------------------------------------- */

