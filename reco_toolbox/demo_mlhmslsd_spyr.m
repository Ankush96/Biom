% Multi-Level Histogram of Color Multi-Scale Line Segment Detector with Spatial Pyramid.
%
% Usage
% -----
%
%
% [H , segments] = mlhmslsd_spyr(I , [options] );
%
%
% Inputs
% ------
%
%
% I                              Input image (ny x nx x [3]) in UINT8 format.
%
%
% options
%        q                       Bound to quantization error on the gradient norm (default q = 2.0)
% 	   d                       Gradient angle tolerance, tau = 180 degree / d (default d = 8.0)
% 	   eps                     Detection threshold, -log10(max. NFA) (default eps = 0.0)
%        n_bins                  # bins in 'ordering' of gradient modulus (default n_bins = 16256)
% 	   max_grad                Gradient modulus in the highest bin (default max_grad = 260100)
%        spyr                    Spatial Pyramid (nspyr x 4) (default [1 , 1 , 1 , 1] with nspyr = 1)
%                                where spyr(i,1) is the ratio of ny in y axis of the blocks at level i (by(i) = spyr(i,1)*ny)
%                                where spyr(i,2) is the ratio of ny in y axis of the shifting at level i (deltay(i) = spyr(i,2)*ny)
%                                where spyr(i,3) is the ratio of nx in x axis of the blocks at level i (bx(i) = spyr(i,3)*nx)
%                                where spyr(i,3) is the ratio of nx in x axis of the shifting at level i (deltax(i) = spyr(i,4)*nx)
%        color                   0 : force gray-scale (dimcolor = 1, default), 1 : RGB (dimcolor = 3), 2 : nRGB (dimcolor = 3), 3 : Opponent color (dimcolor = 3),
%                                4 : nOpponent color (dimcolor = 2), 5 : Hue  (dimcolor = 1)
% 	   scale                   Gaussian filter scaling vector for image before processing (1 x nscale)
% 	   sigma_scale             Scaling factor to obtain the standard deviation of the Gaussian filter (sigma = sigma_scale/scale)
% 	   nori                    Number of bin's orientation (default nori = 9)
% 	   nlength                 Number of bin's length  (default nlength = 9)
% 	   norm                    Normalization : norm = 0 <=> no normalization, norm = 1 <=> v=v/(sum(v)+epsi), norm = 2 <=> v=v/sqrt(sum(v²)+epsi²),
% 	                           norm = 3 <=> v=sqrt(v/(sum(v)+epsi)) , norm = 4 <=> L2-clamped (default norm = 1)
% 	   clamp                   Clamping value (default clamp = 0.2)
%
%
%
% Outputs
% --------
%
% H                                     Histogram of features (nH*nori*nlength x 1) where nH = sum(floor(((1 - spyr(:,1))./(spyr(:,3)) + 1)).*floor((1 - spyr(:,2))./(spyr(:,4)) + 1))
% segments                              Matrix of detected segments (7 x nb_segments) where each column vector is [x1 , y1 , x2 , y2 , witdh , scale , dimcolor]
%
% Reference : [1] R. Grompone von Gioi, J. Jakubowicz, J.-M. Morel, G. Randall,
% ---------       "LSD: A Fast Line Segment Detector with a False Detection Control".
%                 IEEE Trans. on PAMI, 19 Dec. 2008

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
% Simple Histogram of LSD (without normalization)
%
%

H                     = mlhmslsd_spyr(I);

figure(co)
plot(H)
axis([0 , length(H) , 0 , 1.2*max(H)]);
title('Histogram of LSD' ,'fontname' , 'times' , 'fontsize' , 13)
co                   = co + 1;

%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  Example 2 %%%%%%%%%%%%%%%%%%%%%%%%%
%
% A 3 level spatial pyramid (overlapping (50%) subwindows for the second and third levels)
% 12 bins for encoding orientation, 12 bins for encoding line length
% Histograms are normalized with a L2-clamped norm
%

options.q           = 2;
options.d           = 8;
options.eps         = -15.0;
options.n_bins      = 16256;
options.max_grad    = 260100;
options.spyr        = [1 , 1 , 1 , 1 ; 1/2 , 1/2 , 1/4 , 1/4 ; 1/4 , 1/4 , 1/8 , 1/8];
options.scale      = 1;
options.color       = 0;
options.nori        = 12;
options.nlength     = 12;
options.norm        = 4;
options.clamp       = 0.2;


nS                  = sum(floor(((1 - options.spyr(:,1))./(options.spyr(:,3)) + 1)).*floor((1 - options.spyr(:,2))./(options.spyr(:,4)) + 1));



[H , out]           = mlhmslsd_spyr(I , options);
nb_segment          = histc(out(6 , :) , options.scale);

figure(co)
plot(1:length(H) , H)
axis([0 , length(H)+1 , min(H) , max(H)*1.2])
title(sprintf('Histograms of LSD with 3 levels SP, nS = %d', nS) ,'fontname' , 'times' , 'fontsize' , 13)

co                   = co + 1;

figure(co)
imagesc(I)
colormap(gray)
hold on
h = line(out([2,4] , :) , out([1,3] , :));
hold off
set(h , 'Color' , [0 , 0 , 1]);
title('Gray Image and detected line segments' ,'fontname' , 'times' , 'fontsize' , 13)

co                   = co + 1;

%%


I                     = imread(fullfile(core_dir , '02769_Right_StudentOffice.jpeg'));

figure(co)
imagesc(I)
title('Color Image example' ,'fontname' , 'times' , 'fontsize' , 13)
co                   = co + 1;


%%

% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  Example 3 %%%%%%%%%%%%%%%%%%%%%%%%%
% Opponent colors are used with
% a 3 level spatial pyramid (overlapping (50%) subwindows for the second & third levels)
% 12 bins for encoding orientation, 12 bins for encoding line length
% Histograms are normalized with a L2
%
options.q           = 2;
options.d           = 8;
options.eps         = -15.0;
options.n_bins      = 16256;
options.max_grad    = 260100;
options.spyr        = [1 , 1 , 1 , 1 ; 1/2 , 1/2 , 1/4 , 1/4 ; 1/4 , 1/4 , 1/8 , 1/8];
option.scale        = 1;
options.sigma_scale = 0.6;
options.color       = 3;
options.nori        = 12;
options.nlength     = 12;
options.norm        = 2;


nS                  = sum(floor(((1 - options.spyr(:,1))./(options.spyr(:,3)) + 1)).*floor((1 - options.spyr(:,2))./(options.spyr(:,4)) + 1));



[H , out]           = mlhmslsd_spyr(I , options);
nb_segment          = histc(out(6 , :) , options.scale);

figure(co)
plot(1:length(H) , H)
axis([0 , length(H)+1 , min(H) , max(H)*1.2])
title(sprintf('Histograms of LSD with 3 levels SP, nS = %d'  , nS) ,'fontname' , 'times' , 'fontsize' , 13)
co                   = co + 1;

figure(co)
imagesc(I)
colormap(gray)
hold on
h = line(out([2,4] , :) , out([1,3] , :));
hold off
set(h , 'Color' , [0 , 0 , 1]);
title('Color Image and detected line segments' ,'fontname' , 'times' , 'fontsize' , 13)

co                   = co + 1;

%%

% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  Example 4 %%%%%%%%%%%%%%%%%%%%%%%%%
% Opponent colors are used with
% a 3 level spatial pyramid (overlapping (50%) subwindows for the second & third levels)
% 12 bins for encoding orientation, 12 bins for encoding line length.
% 3 differents scale are used.
% Histograms are normalized with a L2-clamped norm
%
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
options.norm        = 4;
options.clamp       = 0.2;


nS                  = sum(floor(((1 - options.spyr(:,1))./(options.spyr(:,3)) + 1)).*floor((1 - options.spyr(:,2))./(options.spyr(:,4)) + 1));



[H , out]           = mlhmslsd_spyr(I , options);
nb_segment          = histc(out(6 , :) , options.scale);

figure(co)
plot(1:length(H) , H)
axis([0 , length(H)+1 , min(H) , max(H)*1.2])
title(sprintf('Histograms of LSD with 3 levels SP, nS = %d'  , nS) ,'fontname' , 'times' , 'fontsize' , 13)
co                   = co + 1;

figure(co)
imagesc(I)
colormap(gray)
hold on
h = line(out([2,4] , :) , out([1,3] , :));
hold off
set(h , 'Color' , [0 , 0 , 1]);
title('Color Image and detected line segments' ,'fontname' , 'times' , 'fontsize' , 13)

co                   = co + 1;

%%
