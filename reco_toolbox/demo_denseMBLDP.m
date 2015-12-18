% denseMBLDP extract Histogram of Multi-Block LDP on a regular dense grid and computed on image I after color projection
% 
% 
% Usage
% ------
% 
% [dmbldp , infodmbldp] =  densembldp(I , [options] );
% 
% 
% Inputs
% -------
% 
% I                              Input image (ny x nx x [3]) in UINT8 format. 
% options 
% 
%        scale                   Multi-Scale vector (1 x nscale) (default scale = 1) where scale(i) = s is the size's factor to apply to each 9 blocks
%                                in the LDP computation, i = 1,...,nscale
% 
% 	   deltax                  Division step in the x-axis for the grid (default deltax = floor(nx*min(scale))) 
% 	   deltay                  Division step in the y-axis for the grid (default deltay = floor(ny*min(scale)))
% 
%        n                       Derivative order of ldp (n > 1, default n = 2)
% 
%        ldporient               Orientation for ldp computation (1 x norient), where norien <= 4 and ldporient(i) = {0,1,2,3}
% 	                           ldporient(i) = 0 <=> (I(x_1+1,y_1)-I(x_1,y_1))*(I(x_0+1,y_0)-I(x_0,y_0))< 0
% 	                           ldporient(i) = 1 <=> (I(x_1+1,y_1+1)-I(x_1,y_1))*(I(x_0+1,y_0+1)-I(x_0,y_0))< 0
% 	                           ldporient(i) = 2 <=> (I(x_1,y_1+1)-I(x_1,y_1))*(I(x_0,y_0+1)-I(x_0,y_0))< 0
% 	                           ldporient(i) = 3 <=> (I(x_1-1,y_1+1)-I(x_1,y_1))*(I(x_0-1,y_0+1)-I(x_0,y_0))< 0
% 	                           (default ldporient = [0 , 1 , 2 , 3])
% 
%        color                   0 : force gray-scale (dimcolor = 1, default), 1 : RGB-LDP (dimcolor = 3), 2 : nRGB-LDP (dimcolor = 3), 3 : Opponent-LDP (dimcolor = 3), 
%                                4 : nOpponent-LDP (dimcolor = 2), 5 : Hue-LDP (dimcolor = 1)
% 
% 	   patchsize               Size of the patch where the descriptor is computed (default patchsize = 16 )	  
% 
% 
%        improvedLDP             0 for default 8 bits encoding, 1 for 9 bits encoding (8 + central area)
% 
%        kernelx                 Kernel in x-direction for computing the gradient (default kernelx = [1])
%        kernely                 Kernel in y-direction for computing the gradient (default kernely = [1])
%                                If numel(kernelx) ~1 || numel(kernely) ~1, compute the magnitude, i.e. sqrt(dx*dx + dy*dy)
%                                For example if kernelx = [1 2 1 ; 0 0 0 ; -1 -2 -1] and kernely = kernelx', this is equivalent to compute filter
%                                the image with a sobel filter.
% 
% 
%        rmextremebins           Force to zero bin = {0 , {255,58,9}} if  rmextremebins = 1 (default rmextremebins = 1)
% 
%        norm                    Normalization : norm = 0 <=> no normalization, norm = 1 <=> v=v/(sum(v)+epsi), norm = 2 <=> v=v/sqrt(sum(v²)+epsi²), 
%                                norm = 3 <=> v=sqrt(v/(sum(v)+epsi)) , norm = 3 <=> L2-clamped (default norm = 1)
% 
%        clamp                   Clamping value (default clamp = 0.2)
% 
% 
% Outputs
% -------
% 
% dmbldp                         MBLDP patches descriptors (nbin x nb_pts) nb_pts = deltax*deltay*nscale*dimcolor 
%                                in double format where nbin = ([256,59,10]*(improvedLDP+1)) (depending on improvedLDP) 
% 							   
% 
% infodmbldp                     MBLDP patches informations(4 x nb_pts) where nb_pts = deltax*deltay*nscale*dimcolor
%                                infodmbldp(1,i) = y
% 						       infodmbldp(2,i) = x
% 							   infodmbldp(3,i) = scale
% 							   infodmbldp(4,i) = color    
% 							   infodmbldp(5,i) = nyscale                 
% 							   infodmbldp(6,i) = nxscale                

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
title('Gray Image example' ,'fontname' , 'times' , 'fontsize' , 13 , 'fontweight','bold')
co                   = co + 1;

%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%  Example 1 %%%%%%%%%%%%%%%%%%%%%%
%
% Extract patches of gray (20 x 20) pixels for computing histograms
% of MB-LDP with 3 block-size and derivative order = 2
% Histograms are L2-clamp normalized
%
%

options.scale         = [1,2,3];
options.deltax        = 12;
options.deltay        = 12;
options.n             = 2;
options.ldporient     = [0 , 1 , 2 , 3];
options.color         = 0;
options.patchsize     = 20;
options.rmextremebins = 1;
options.norm          = 4;
options.clamp         = 0.2;


[dmbldp , infodmbldp] =  denseMBLDP(I , options );

half                  = options.patchsize/2;

xr                    = [infodmbldp(2, :)-half ; infodmbldp(2, :)-half ; infodmbldp(2, :)+ half ; infodmbldp(2, :)+ half ; infodmbldp(2, :)-half] + 1.5;
yr                    = [infodmbldp(1, :)-half ; infodmbldp(1, :)+half ; infodmbldp(1, :)+ half ; infodmbldp(1, :)- half ; infodmbldp(1, :)-half] + 1.5;


figure(co)
imagesc(I)
colormap(gray)
hold on
plot(infodmbldp(2 , :)+1.5 , infodmbldp(1 , :)+1.5 , 'r+')
plot(xr , yr , 'b')
hold off
title(sprintf('Location of %dx%d=%d patches of size = %dx%d' , options.deltay,options.deltax,options.deltay*options.deltax,options.patchsize,options.patchsize) ,'fontname' , 'times' , 'fontsize' , 13, 'fontweight','bold')
co                   = co + 1;


figure(co)
imagesc(dmbldp)
title(sprintf('Histograms of MBLDP for 3 different block-size') ,'fontname' , 'times' , 'fontsize' , 13 ,  'fontweight','bold')
h=ylabel('bins');
set(h,'fontsize',12,'fontweight','bold')
co                   = co + 1;

%%
