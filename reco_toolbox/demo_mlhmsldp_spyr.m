% Multi-Level Histogram of Color Multi-Scale Local Derivative Pattern with Spatial Pyramid.
%
% Usage
% -----
%
% H = mlhmsldp_spyr(I , [options] );
%
%
% Inputs
% -------
%
% I                              Input image (ny x nx x [3]) in UINT8 format.
% options
%        spyr                    Spatial Pyramid (nspyr x 4) (default [1 , 1 , 1 , 1] with nspyr = 1)
%                                where spyr(i,1) is the ratio of ny in y axis of the blocks at level i (by(i) = spyr(i,1)*ny)
%                                where spyr(i,2) is the ratio of ny in y axis of the shifting at level i (deltay(i) = spyr(i,2)*ny)
%                                where spyr(i,3) is the ratio of nx in x axis of the blocks at level i (bx(i) = spyr(i,3)*nx)
%                                where spyr(i,3) is the ratio of nx in x axis of the shifting at level i (deltax(i) = spyr(i,4)*nx)
%
%       scale                    Multi-Scale vector (1 x nscale) (default scale = 1) where scale(i) = s is the size's factor to apply to each 9 blocks
%                                in the LBP computation, i = 1,...,nscale
%
%       n                        Derivative order of ldp (n > 1, default n = 2)
%
%       ldporient                Orientation for ldp computation (1 x norient), where norien <= 4 and ldporient(i) = {0,1,2,3}
% 	                           ldporient(i) = 0 <=> (I(x_1+1,y_1)-I(x_1,y_1))*(I(x_0+1,y_0)-I(x_0,y_0))< 0
% 	                           ldporient(i) = 1 <=> (I(x_1+1,y_1+1)-I(x_1,y_1))*(I(x_0+1,y_0+1)-I(x_0,y_0))< 0
% 	                           ldporient(i) = 2 <=> (I(x_1,y_1+1)-I(x_1,y_1))*(I(x_0,y_0+1)-I(x_0,y_0))< 0
% 	                           ldporient(i) = 3 <=> (I(x_1-1,y_1+1)-I(x_1,y_1))*(I(x_0-1,y_0+1)-I(x_0,y_0))< 0
% 	                           (default ldporient = [0 , 1 , 2 , 3])
%
%       color                    0 : force gray-scale (dimcolor = 1, default), 1 : RGB-LDP (dimcolor = 3), 2 : nRGB-LDP (dimcolor = 3), 3 : Opponent-LDP (dimcolor = 3),
%                                4 : nOpponent-LDP (dimcolor = 2), 5 : Hue-LDP (dimcolor = 1)
%
%
%       improvedLDP              0 for default 8 bits encoding, 1 for 9 bits encoding (8 + central area)
%
%       kernelx                  Kernel in x-direction for computing the gradient (default kernelx = [1])
%       kernely                  Kernel in y-direction for computing the gradient (default kernely = [1])
%                                If numel(kernelx) ~1 || numel(kernely) ~1, compute the magnitude, i.e. sqrt(dx*dx + dy*dy)
%                                For example if kernelx = [1 2 1 ; 0 0 0 ; -1 -2 -1] and kernely = kernelx', this is equivalent to compute filter
%                                the image with a sobel filter.
%
%       rmextremebins            Force to zero bin = {0 , 255} if  rmextremebins = 1 (default rmextremebins = 1)
%
%       norm                     Normalization : norm = 0 <=> no normalization, norm = 1 <=> v=v/(sum(v)+epsi), norm = 2 <=> v=v/sqrt(sum(v²)+epsi²),
%                                norm = 3 <=> v=sqrt(v/(sum(v)+epsi)) , norm = 3 <=> L2-clamped (default norm = 1)
%
%       clamp                    Clamping value (default clamp = 0.2)
%
%
% Output
% ------
%
% H                              mlhmsldp_spyr features (nbin*nH*nscale*dimcolor x 1) in double format where nbin = 256*(improvedLDP+1) and nH is the total number of subwindows defined
%                                by the spatial pyramid spyr, i.e. nH = sum(floor(((1 - spyr(:,1))./(spyr(:,3)) + 1)).*floor((1 - spyr(:,2))./(spyr(:,4)) + 1)).
%
%
% Reference : [1] Baochang Zhang, Yongsheng Gao, Sanqiang Zhao, and Jianzhuang Liu,
% ---------       "Local Derivative Pattern Versus Local BinaryPattern: Face Recognition With High-Order Local Pattern Descriptor"
%                 IEEE TRANSACTIONS ON IMAGE PROCESSING, VOL. 19, NO. 2, FEBRUARY 2010


%%
clc,close all, clear all,drawnow
rootbase_dir         = pwd;
core_dir             = fullfile(pwd , 'core');
addpath(core_dir)

co                   = 1;


I                     = imread(fullfile(core_dir , 'image_0174.jpg'));

figure(co)
imagesc(I)
colormap(gray)
title('Gray Image example' ,'fontname' , 'times' , 'fontsize' , 13)
co                   = co + 1;
%%


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  Example 1 %%%%%%%%%%%%%%%%%%%%%%%%%
%
% Simple Histogram of LDP (without normalization)
% derivative order n = 2 by default
%
%

H                     = mlhmsldp_spyr(I);

figure(co)
plot(H)
axis([0 , length(H) , 0 , 1.2*max(H)]);
title('Histogram of LDP' ,'fontname' , 'times' , 'fontsize' , 13)
co                   = co + 1;

%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  Example 2 %%%%%%%%%%%%%%%%%%%%%%%%%
%
% A 2 level spatial pyramid (overlapping (50%) subwindows for the second level)
% where MB-LBP are computed for 4 block size and LBP code = {0,255} are
% removed (white or black uniform patches)
% Histogram are normalized with a L2-clamped norm
%

options.spyr          = [1 , 1 , 1 , 1 ; 1/2 , 1/2 , 1/4 , 1/4];
options.scale         = [1 , 2 , 3 , 4 ];
options.n             = 3;
options.rmextremebins = 1;
options.norm          = 4;
options.clamp         = 0.2;


nS                    = length(options.scale)*sum(floor(((1 - options.spyr(:,1))./(options.spyr(:,3)) + 1)).*floor((1 - options.spyr(:,2))./(options.spyr(:,4)) + 1));

H                     = mlhmslbp_spyr(I , options );

figure(co)
plot(H)
axis([0 , length(H) , 0 , 1.2*max(H)]);
title(sprintf('Histograms of MB-LDP with 2 levels SP, n = %d, nS = %d' , options.n , nS) ,'fontname' , 'times' , 'fontsize' , 13)
co                   = co + 1;

%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  Example 3 %%%%%%%%%%%%%%%%%%%%%%%%%
%
% A 3 level spatial pyramid (non-overlapping subwindows)
% where Uniform MB-LDP are computed for 2 block size
% Histogram are normalized with a L1 norm
%

options.spyr          = [1 , 1 , 1 , 1 ; 1/2 , 1/2 , 1/2 , 1/2 ; 1/4 , 1/4 , 1/4 , 1/4];
options.scale         = [1 , 3 ];
options.n             = 3;
options.maptable      = 1;
options.norm          = 1;


nS                    = length(options.scale)*sum(floor(((1 - options.spyr(:,1))./(options.spyr(:,3)) + 1)).*floor((1 - options.spyr(:,2))./(options.spyr(:,4)) + 1));

H                     = mlhmsldp_spyr(I , options );

figure(co)
plot(H)
axis([0 , length(H) , 0 , 1.2*max(H)]);
title(sprintf('Histograms of Uniform MB-LDP with 3 levels SP, n = %d, nS = %d' , options.n , nS) ,'fontname' , 'times' , 'fontsize' , 13)
co                   = co + 1;

%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  Example 4 %%%%%%%%%%%%%%%%%%%%%%%%%
% Image I is first filtered with a sobel mask,
% If = conv2(kernelx , kernely , I , 'same');
% A 2 level spatial pyramid (overlapping (50%) subwindows for the second level)
% where MB-LDP are computed for 3 block size and LBP code = {0,255} are
% removed (white or black uniform patches)
% Histogram are normalized with a L2-clamped norm
%

options.spyr          = [1 , 1 , 1 , 1 ; 1/2 , 1/2 , 1/4 , 1/4];
options.scale         = [1 , 3 , 5 ];
options.kernelx       = [1 2 1 ; 0 0 0 ; -1 -2 -1];  %sobel mask
options.kernely       = [1 0 -1 ; 2 0 -2 ; 1 0 -1];  %sobel mask
options.n             = 2;
options.rmextremebins = 1;
options.norm          = 4;
options.clamp         = 0.2;


nS                    = length(options.scale)*sum(floor(((1 - options.spyr(:,1))./(options.spyr(:,3)) + 1)).*floor((1 - options.spyr(:,2))./(options.spyr(:,4)) + 1));

H                     = mlhmsldp_spyr(I , options );

figure(co)
plot(H)
axis([0 , length(H) , 0 , 1.2*max(H)]);
title(sprintf('Histograms of MB-LDP with 2 levels SP, n = %d, nS = %d' , options.n , nS) ,'fontname' , 'times' , 'fontsize' , 13)
co                   = co + 1;

%%



I                     = imread(fullfile(core_dir , '02769_Right_StudentOffice.jpeg'));

figure(co)
imagesc(I)
title('Color Image example' ,'fontname' , 'times' , 'fontsize' , 13)
co                   = co + 1;

%%


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  Example 5 %%%%%%%%%%%%%%%%%%%%%%%%%
% Opponent colors are used with
% a 2 level spatial pyramid (overlapping (50%) subwindows for the second level)
% where MB-LBP are computed for 4 block size and only 2 orientations.
% LBP code = {0,255} are removed (white or black uniform patches)
% Histogram are normalized with a L2-clamped norm
%

options.spyr          = [1 , 1 , 1 , 1 ; 1/2 , 1/2 , 1/4 , 1/4];
options.scale         = [1  , 3 , 5 ];
options.ldporient     = [1 , 3];
options.n             = 3;
options.color         = 3;
options.rmextremebins = 1;
options.norm          = 4;
options.clamp         = 0.2;

nS                    = length(options.scale)*sum(floor(((1 - options.spyr(:,1))./(options.spyr(:,3)) + 1)).*floor((1 - options.spyr(:,2))./(options.spyr(:,4)) + 1));

H                     = mlhmsldp_spyr(I , options );

figure(co)
plot(H)
axis([0 , length(H) , 0 , 1.2*max(H)]);
title(sprintf('Histograms of Color MB-LDP (Opponnent) with 2 levels SP, n = %d, nS = %d' , options.n , nS) ,'fontname' , 'times' , 'fontsize' , 13)
co                   = co + 1;

%%
