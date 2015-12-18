% denseCOLOR compute histogram of color projection on a regular dense grid
% 
% Usage
% ------
% 
% [dcolor , infodcolor] = denseCOLOR(I , [options] );
% 
% 
% Inputs
% -------
% 
% I                                     Input image (ny x nx x [3]) in UINT8 format. 
% 
% options
% 	   scale                          Scaling vector (1 x nscale). Extract descriptors at different scaling of the image (default scale = [1]).
% 	   sigma_scale                    Scaling factor to obtain the standard deviation of the Gaussian filter (sigma = sigma_scale/scale)(default sigma_scale = 0.6)
% 	   deltax                         Division step in the x-axis for the grid (default deltax = floor(nx*min(scale))) 
% 	   deltay                         Division step in the y-axis for the grid (default deltay = floor(ny*min(scale)))
%        color                          0 : force gray-scale (dimcolor = 1, default), 1 : RGB (dimcolor = 3), 2 : nRGB (dimcolor = 3), 3 : Opponent (dimcolor = 3), 
%                                       4 : nOpponent (dimcolor = 2), 5 : Hue (dimcolor = 1)
% 	   nbins                          Number of bins for histograms (default nbins = 32)
% 	   patchsize                      Size of the patch where the descriptor is computed (default patchsize = nbins/2 )	  
% 	   norm                           Normalization : norm = 0 <=> no normalization, norm = 1 <=> v=v/(sum(v)+epsi), norm = 2 <=> v=v/sqrt(sum(v²)+epsi²), 
% 	                                  norm = 3 <=> v=sqrt(v/(sum(v)+epsi)) , norm = 3 <=> L2-clamped (default norm = 1)
% 	   clamp                          Clamping value (default clamp = 0.2)
% 
% 
% Outputs
% -------
% 
% dcolor                                COLOR descriptors (nbins x nb_pts) where nb_pts = deltax*deltay*nscale*dimcolor
% infodcolor                            COLOR descriptors informations(4 x nb_pts)   where nb_pts = deltax*deltay*nscale*dimcolor
%                                       infodcolor(1,i) = y
% 									  infodcolor(2,i) = x
% 									  infodcolor(3,i) = scale
% 									  infodcolor(4,i) = color
% 									  infodcolor(5,i) = nyscale
% 									  infodcolor(6,i) = nxscale

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
% Extract Histogram of gray pixels over 16 bins
% from patches (20 x 20) regulary sampled from I.
% Histograms are L1-normalized
%
%

options.deltax                       = 10;
options.deltay                       = 10;
options.nbins                        = 16;
options.patchsize                    = 20;
options.norm                         = 1;


[dcolor , infodcolor]                = denseCOLOR(I , options ); 

half                                 = infodcolor(7, :)/2;
xr                                   = [infodcolor(2, :)-half ; infodcolor(2, :)-half ; infodcolor(2, :)+ half ; infodcolor(2, :)+ half ; infodcolor(2, :)-half] + 1.5;
yr                                   = [infodcolor(1, :)-half ; infodcolor(1, :)+half ; infodcolor(1, :)+ half ; infodcolor(1, :)- half ; infodcolor(1, :)-half] + 1.5;


figure(co)
imagesc(I)
colormap(gray)
hold on
plot(infodcolor(2 , :)+1.5 , infodcolor(1 , :)+1.5 , 'r+')
plot(xr , yr , 'b')
hold off
title(sprintf('Location of %dx%d=%d patches of size = %dx%d' , options.deltay,options.deltax,options.deltay*options.deltax,options.patchsize,options.patchsize) ,'fontname' , 'times' , 'fontsize' , 13, 'fontweight','bold')
co                   = co + 1;


figure(co)
imagesc(dcolor)
title(sprintf('Histograms of gray pixels with nbins = %d',options.nbins) ,'fontname' , 'times' , 'fontsize' , 13 ,  'fontweight','bold')
h=ylabel('bins');
set(h,'fontsize',12,'fontweight','bold')
co                   = co + 1;

%%



I                     = imread(fullfile(core_dir , '02769_Right_StudentOffice.jpeg'));

figure(co)
imagesc(I)
title('Color Image example' ,'fontname' , 'times' , 'fontsize' , 13 , 'fontweight','bold')
co                   = co + 1;


%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%  Example 2 %%%%%%%%%%%%%%%%%%%%%%
%
% Extract Histogram of color pixels with Opponent projection
% over 32 bins from patches (24 x 24) regulary sampled from I.
% I is scaled with 3 differents factor. 
% Histograms are L2-clamp normalized
%
%

options.scale                        = [0.5 , 0.75 , 1];
options.sigma_scale                  = 0.6;
options.deltax                       = 10;
options.deltay                       = 10;
options.color                        = 3;
options.nbins                        = 32;
options.patchsize                    = 28;
options.norm                         = 4;
options.clamp                        = 0.2;


[dcolor , infodcolor]                 = denseCOLOR(I , options ); 

half                                 = infodcolor(7, :)/2;
xr                                   = [infodcolor(2, :)-half ; infodcolor(2, :)-half ; infodcolor(2, :)+ half ; infodcolor(2, :)+ half ; infodcolor(2, :)-half] + 1.5;
yr                                   = [infodcolor(1, :)-half ; infodcolor(1, :)+half ; infodcolor(1, :)+ half ; infodcolor(1, :)- half ; infodcolor(1, :)-half] + 1.5;


figure(co)
imagesc(I)
hold on
plot(infodcolor(2 , :)+1.5 , infodcolor(1 , :)+1.5 , 'r+')
plot(xr , yr , 'b')
hold off
title(sprintf('Location of %dx%dx%d=%d patches of size = \\{%d,%d,%d\\}^2' , options.deltay,options.deltax,length(options.scale),options.deltay*options.deltax*length(options.scale),options.patchsize*options.scale(1),options.patchsize*options.scale(2),options.patchsize*options.scale(3)) ,'fontname' , 'times' , 'fontsize' , 13 , 'fontweight','bold')
co                   = co + 1;


figure(co)
imagesc(dcolor)
h=ylabel('bins');
set(h,'fontsize',12,'fontweight','bold')
title(sprintf('Histograms of color pixels (Opponent projection) with nbins = %d',options.nbins) ,'fontname' , 'times' , 'fontsize' , 13 , 'fontweight','bold')

%%


