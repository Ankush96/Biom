% denseMBLBP extract Histograms of Multi-Block LBP on a regular dense grid and computed on image I after color projection
% 
% 
% Usage
% ------
% 
% [dmblbp , infodmblbp] =  denseMBLBP(I , [options] );
% 
% 
% Inputs
% -------
% 
% I                              Input image (ny x nx x [3]) in UINT8 format. 
% options 
% 
%        scale                   Multi-Scale vector (1 x nscale) (default scale = 1) where scale(i) = s is the size's factor to apply to each 9 blocks
%                                in the LBP computation, i = 1,...,nscale
% 
% 	   deltax                  Division step in the x-axis for the grid (default deltax = floor(nx*min(scale))) 
% 	   deltay                  Division step in the y-axis for the grid (default deltay = floor(ny*min(scale)))
% 
%        color                   0 : force gray-scale (dimcolor = 1, default), 1 : RGB-LBP (dimcolor = 3), 2 : nRGB-LBP (dimcolor = 3), 3 : Opponent-LBP (dimcolor = 3), 
%                                4 : nOpponent-LBP (dimcolor = 2), 5 : Hue-LBP (dimcolor = 1)
% 
% 	   patchsize               Size of the patch where the descriptor is computed (default patchsize = 16 )	  
% 
% 	   maptable                Mapping table for LBP codes. maptable = 0 <=> normal LBP = {0,...,255} (default), 
% 	                           maptable = 1 <=> uniform LBP = {0,...,58}, maptable = 2 <=> uniform rotation-invariant LBP = = {0,...,9}
% 
% 	   useFF                   Use LBP-Fourier Feature rotation invariant (force maptable = 1)
% 
%        improvedLBP             0 for default 8 bits encoding, 1 for 9 bits encoding (8 + central area)
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
% dmblbp                         MBLBP patches descriptors (nbin x nb_pts) nb_pts = deltax*deltay*nscale*dimcolor 
%                                in double format where nbin = ([256,59,10]*(improvedLBP+1)) (depending on improvedLBP) 
% 							   
% 
% infodmblbp                     MBLBP patches informations(6 x nb_pts) where nb_pts = deltax*deltay*nscale*dimcolor
%                                infodmblbp(1,i) = y
% 						       infodmblbp(2,i) = x
% 							   infodmblbp(3,i) = scale
% 							   infodmblbp(4,i) = color    
% 							   infodmblbp(5,i) = nyscale                 
% 							   infodmblbp(6,i) = nxscale                 
%
%
% References [1] Timo Ahonen, Jiri Matas, Chu He, and Matti Pietikäinen ,
% ---------      "Rotation Invariant Image Description with Local Binary Pattern Histogram Fourier Features"
%

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
options.color         = 0;
options.patchsize     = 20;
options.rmextremebins = 1;
options.norm          = 4;
options.clamp         = 0.2;


[dmblbp , infodmblbp] =  denseMBLBP(I , options );

half                  = options.patchsize/2;

xr                    = [infodmblbp(2, :)-half ; infodmblbp(2, :)-half ; infodmblbp(2, :)+ half ; infodmblbp(2, :)+ half ; infodmblbp(2, :)-half] + 1.5;
yr                    = [infodmblbp(1, :)-half ; infodmblbp(1, :)+half ; infodmblbp(1, :)+ half ; infodmblbp(1, :)- half ; infodmblbp(1, :)-half] + 1.5;


figure(co)
imagesc(I)
colormap(gray)
hold on
plot(infodmblbp(2 , :)+1.5 , infodmblbp(1 , :)+1.5 , 'r+')
plot(xr , yr , 'b')
hold off
title(sprintf('Location of %dx%d=%d patches of size = %dx%d' , options.deltay,options.deltax,options.deltay*options.deltax,options.patchsize,options.patchsize) ,'fontname' , 'times' , 'fontsize' , 13, 'fontweight','bold')
co                   = co + 1;


figure(co)
imagesc(dmblbp)
title(sprintf('Histograms of MBLBP for 3 different block-size') ,'fontname' , 'times' , 'fontsize' , 13 ,  'fontweight','bold')
h=ylabel('bins');
set(h,'fontsize',12,'fontweight','bold')
co                   = co + 1;

%%
