% densePATCH extract patches of pixels after color projection on a regular dense grid
% 
% Usage
% ------
% 
% [dpatch , infodpatch] = densePATCH(I , [options] );
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
% 	   patchsize                      Size of the patch where the descriptor is computed (default patchsize = 9 )	  
% 	   norm                           Normalization : norm = 0 <=> no normalization, norm = 1 <=> v=v/(sum(v)+epsi), norm = 2 <=> v=v/sqrt(sum(v²)+epsi²), 
% 	                                  norm = 3 <=> v=sqrt(v/(sum(v)+epsi)) , norm = 3 <=> L2-clamped (default norm = 1)
% 	   clamp                          Clamping value (default clamp = 0.2)
% 
% 
% Outputs
% -------
% 
% dpatch                                PATCH descriptors (nbins x nb_pts) where nb_pts = deltax*deltay*nscale*dimcolor
% infodpatch                            PATCH descriptors informations(6 x nb_pts)   where nb_pts = deltax*deltay*nscale*dimcolor
%                                       infodpatch(1,i) = y
% 									  infodpatch(2,i) = x
% 									  infodpatch(3,i) = scale
% 									  infodpatch(4,i) = color
% 									  infodpatch(5,i) = nyscale
% 									  infodpatch(6,i) = nxscale

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
% Extract patches of gray (20 x 20) pixels 
%
%

options.deltax                       = 16;
options.deltay                       = 16;
options.patchsize                    = 12;


[dcolor , infodcolor]                = densePATCH(I , options ); 

half                                 = options.patchsize/2;
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
display_dico(dcolor)
title(sprintf('Patches of gray (%dx%d) pixels',options.patchsize,options.patchsize) ,'fontname' , 'times' , 'fontsize' , 13 ,  'fontweight','bold')
h=ylabel('bins');
set(h,'fontsize',12,'fontweight','bold')
co                   = co + 1;

%%
