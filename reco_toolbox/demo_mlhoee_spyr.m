% Multi-Level of Color Histogram of Oriented Edge Energy with Spatial Pyramid
% 
% Usage
% ------
% 
% H = mlhoee_spyr(I , [options] );
% 
% 
% Inputs
% -------
% 
% I                                     Input image (ny x nx x [3]) in UINT8 format. 
% 
% options
%        spyr                           Spatial Pyramid (nspyr x 4) (default [1 , 1 , 1 , 1] with nspyr = 1)
%                                       where spyr(i,1) is the ratio of ny in y axis of the blocks at level i (by(i) = spyr(i,1)*ny)
%                                       where spyr(i,2) is the ratio of ny in y axis of the shifting at level i (deltay(i) = spyr(i,2)*ny)
%                                       where spyr(i,3) is the ratio of nx in x axis of the blocks at level i (bx(i) = spyr(i,3)*nx)
%                                       where spyr(i,3) is the ratio of nx in x axis of the shifting at level i (deltax(i) = spyr(i,4)*nx)
% 
%        color                          0 : force gray-scale (dimcolor = 1, default), 1 : RGB (dimcolor = 3), 2 : nRGB (dimcolor = 3), 3 : Opponent color (dimcolor = 3), 
%                                       4 : nOpponent color (dimcolor = 2), 5 : Hue  (dimcolor = 1)
% 
%        norma                          Gradient normalization block size (default norma = [1 , 1] of the orginal image size)
% 	   kernelx                        Kernel in x-direction for computing the gradient (default kernelx = [-0.5 , 0 , 0.5])
% 	   kernely                        Kernel in y-direction for computing the gradient (default kernely = [-0.5 ; 0 ; 0.5])
%        bndori                         bndori = 0 <=> angle in [-pi/2 , pi/2[), bndori = 1 angle in [-pi , pi[
% 	   nori                           Number of orientation (default nori = 9)
% 	   norm                           Normalization : norm = 0 <=> no normalization, norm = 1 <=> v=v/(sum(v)+epsi), norm = 2 <=> v=v/sqrt(sum(v²)+epsi²), 
% 	                                  norm = 3 <=> v=sqrt(v/(sum(v)+epsi)) , norm = 4 <=> L2-clamped (default norm = 1)
%        interpolate                    Interpolate lineary values of energy : 1 if interpolate, 0 else (default = 1)
% 	   clamp                          Clamping value (default clamp = 0.2)
% 
% 
% Output
% ------
% 
% H                                     mlhoee_spyr features (nH*nori*dimcolor x 1) in double format where nH is the total number of subwindows defined
%                                       by the spatial pyramid spyr, i.e. nH = sum(floor(((1 - spyr(:,1))./(spyr(:,3)) + 1)).*floor((1 - spyr(:,2))./(spyr(:,4)) + 1)).
%
% Reference :   [1] Subhransu Maji and Alexander C. Berg and Jitendra Malik, "Classification Using Intersection Kernel Support Vector Machines is efficient"
% ---------         In Proceedings, CVPR 2008, Anchorage, Alaska

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
title('Gray Image example' ,'fontname' , 'times' , 'fontsize' , 13, 'fontweight','bold')
co                   = co + 1;

%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  Example 1 %%%%%%%%%%%%%%%%%%%%%%%%%
%
% Simple Histogram of LSD (without normalization)
%
%

H                     = mlhoee_spyr(I);

figure(co)
plot(H , 'linewidth',2)
axis([0.5 , length(H)+0.5 , 0 , 1.2*max(H)]);
title('Histogram of Oriented Edge Energy' ,'fontname' , 'times' , 'fontsize' , 13, 'fontweight','bold')
co                   = co + 1;

%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  Example 2 %%%%%%%%%%%%%%%%%%%%%%%%%
%
% A 4 level spatial pyramid (overlapping (50%) subwindows for the 2th,Eth,4th levels)
% 9 bins for encoding orientation.
% Histograms are normalized with a L2-clamped norm
%

options.spyr          = [1 , 1 , 1 , 1 ; 1/2 , 1/2 , 1/4 , 1/4 ; 1/4 , 1/4 , 1/8 , 1/8 ; 1/8 , 1/8 , 1/16 , 1/16];
options.norma         = [1/16 , 1/16];
options.kernelx       = [-0.5 , 0 , 0.5];
options.kernely       = [-0.5 ; 0 ; 0.5];
options.nori          = 9;
options.bndori        = 1;
options.norm          = 4;
options.clamp         = 0.4;


nS                    = sum(floor(((1 - options.spyr(:,1))./(options.spyr(:,3)) + 1)).*floor((1 - options.spyr(:,2))./(options.spyr(:,4)) + 1));



H                     = mlhoee_spyr(I , options);

figure(co)
plot(1:length(H) , H)
axis([0 , length(H)+1 , min(H) , max(H)*1.2])
title(sprintf('Histograms of OEE with 4 levels SP, nS = %d', nS) ,'fontname' , 'times' , 'fontsize' , 13, 'fontweight','bold')

co                   = co + 1;

%%

I                     = imread(fullfile(core_dir , '02769_Right_StudentOffice.jpeg'));

figure(co)
imagesc(I)
title('Color Image example' ,'fontname' , 'times' , 'fontsize' , 13, 'fontweight','bold')
co                   = co + 1;

%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  Example 3 %%%%%%%%%%%%%%%%%%%%%%%%%
%
% A 4 level spatial pyramid (overlapping (50%) subwindows for the 2th,Eth,4th levels)
% 9 bins for encoding orientation. Opponent color projection is used
% Histograms are normalized with a L1 norm
%

options.spyr          = [1 , 1 , 1 , 1 ; 1/2 , 1/2 , 1/4 , 1/4 ; 1/4 , 1/4 , 1/8 , 1/8 ; 1/8 , 1/8 , 1/16 , 1/16];
options.norma         = [1/32 , 1/32];
options.kernelx       = [-0.5 , 0 , 0.5];
options.kernely       = [-0.5 ; 0 ; 0.5];
options.color         = 3;
options.nori          = 9;
options.bndori        = 1;
options.norm          = 1;
options.interpolate   = 1;


nS                    = sum(floor(((1 - options.spyr(:,1))./(options.spyr(:,3)) + 1)).*floor((1 - options.spyr(:,2))./(options.spyr(:,4)) + 1));



H                     = mlhoee_spyr(I , options);

figure(co)
plot(1:length(H) , H)
axis([0 , length(H)+1 , min(H) , max(H)*1.2])
title(sprintf('Histograms of OEE with 4 levels SP, nS = %d', nS) ,'fontname' , 'times' , 'fontsize' , 13, 'fontweight','bold')

co                   = co + 1;

%%
