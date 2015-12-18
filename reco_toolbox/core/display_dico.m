function display_dico(D)

%  Display dictionary D on a single figure 
%
%  Usage
%  ------
% 
%  display_database(X)
%
%  Input
%  ------
%   
%  D                 Image's database (d x N x ncolor)
% 
%
%  Example
%  -------
%
%  load dltrain_densePATCH.mat
%  display_dico(D(:,:,1))
%
%
%  Author : Sébastien PARIS : sebastien.paris@lsis.org
%  -------  Date : 01/27/2009
%

[d , N , ncolor]  = size(D);

Ny                = sqrt(d);

Nx                = Ny;

c                 = class(D);

P                 = ceil(sqrt(N));

Y                 = permute(cat(3 , reshape(D , [Ny , Nx , N]) , zeros(Ny , Nx , P*P-N , c)) , [1 , 3 , 2]);

Y                 = reshape(Y , [Ny , P , P , Nx]);

Y                 = reshape(permute(Y , [1 , 2 , 4 , 3]) , Ny*P , Nx*P);

imagesc(Y)
colormap(gray)
axis off



