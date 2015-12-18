// pwl_sgd.cc alex berg march 2009

// mex -f mexopts_intel10.bat -output pwlsgd_predict.dll pwlsgd_predict.c

// mex -g -output pwlsgd_predict.dll pwlsgd_predict.c

/*

clear, close all
mu1                    = [0  0];
sigma1                 = [1 0; 0 1];
sigma2                 = [2 0; 0 2];
npts                   = 5000;


options.ndivs          = 20;
options.use_sqrt       = 1;


options.k              = 10;
options.niters         = 10000;
options.lambda         = 0.0001; 
options.use_pwl        = 1;


Xtrain                  = single(mvnrnd(mu1,sigma1,npts)');

p1                      = mvnpdf(Xtrain',mu1,sigma1);
p2                      = mvnpdf(Xtrain',mu1,sigma2);
ytrain                  = zeros(1,npts);
ytrain(p1 >= p2)        = 1;
ytrain(p1 < p2)         = -1;


[Xetrain , seg_data]    = encoder(Xtrain , options);


wtrain                  = pwlsgd_train(Xetrain , ytrain , options);




Xtest                   = single(mvnrnd(mu1,sigma1,npts)');
Xetest                  = encoder(Xtest , options , seg_data);

p1                      = mvnpdf(Xtest',mu1,sigma1);
p2                      = mvnpdf(Xtest',mu1,sigma2);
ytest                   = zeros(1,npts);
ytest(p1 >= p2)         = 1;
ytest(p1 < p2)          = -1;

ptest                   = pwlsgd_predict(Xetest , wtrain);


figure; 
hold on; axis equal;box on;
title('Training Data')
plot(Xtrain(1,ytrain > 0), Xtrain(2,ytrain > 0),'+g');
plot(Xtrain(1,ytrain < 0), Xtrain(2,ytrain < 0),'+r');

figure;
hold on; axis equal;box on;
plot(Xtest(1,ptest > 0), Xtest(2,ptest > 0),'+g');
plot(Xtest(1,ptest < 0), Xtest(2,ptest < 0),'+r');

%find misclassified ones
ii         = find(sign(ptest).*sign(ytest) < 0);
plot(Xtest(1,ii), Xtest(2,ii),'ok');
error_rate = length(ii)*100/length(ptest);
title(sprintf('Classification on Test Data using PWLSGD (%.2f%% error)',error_rate));


*/

#include <string.h>
#include <math.h>
#include <stdio.h>
#include "mex.h"


void mexFunction (int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{

    double *X , *w;
	int *ir, *jc;
	double *p;
	int d , N , i , j;
	double sum;
	

	/*-------------------- Input 1 : X  ---------------------------*/
	
	if ((!mxIsSparse(prhs[0]))  || (mxGetNumberOfDimensions(prhs[0]) != 2))
	{
		mexErrMsgTxt("X must be a sparse ((dxndivs) x N) matrix\n");
	}

	X                 = mxGetPr(prhs[0]);
    ir                = mxGetIr(prhs[0]);
    jc                = mxGetJc(prhs[0]);
	d                 = mxGetM(prhs[0]);
	N                 = mxGetN(prhs[0]);


	/*-------------------- Input 2 : w  ---------------------------*/

	if ((!mxIsDouble(prhs[1])) || (mxIsComplex(prhs[1])) || (mxGetM(prhs[1]) != 1) || (mxGetN(prhs[1]) != d))
	{
		mexErrMsgTxt("w must be (1 x d) of type (real) double\n");
	}

	w                 = mxGetPr(prhs[1]);


	/*--------------------  Output 1 : p  ---------------------------*/

    plhs[0]           = mxCreateNumericMatrix(1 , N , mxDOUBLE_CLASS,  mxREAL);
	p                 = mxGetPr(plhs[0]);


	for (j = 0 ; j < N ; j++)
	{
		sum        = 0.0;
		for (i = jc[j] ; i < jc[j + 1] ; i++)
		{
			sum  += w[ir[i]]*X[i]; 
		}
		p[j]       = sum;
	}
}

