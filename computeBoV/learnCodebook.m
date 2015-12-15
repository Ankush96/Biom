function C=learnCodebook(words,K,niter)
%% This function learns a codebook of required size
%  using K-means clustering
%  
% i/p  -->  words : words are the features (SIFT like) in the image
%                   one per column
%               K : number of codewords user wants
%           niter : 
%
% o/p  -->  C     : the codewords one per column
%
%
%  Author : Mopuri K Reddy, VAL, SERC, IISC, Banglore
%  sercmkreddy@ssl.serc.iisc.in
%  Date   : 31/10/14

if(nargin<3)
    niter=50;
else if(niter<2)
        error('pls. chk the inputs\n');        
    end
end
C=yael_kmeans(single(words),K,'niter',niter);