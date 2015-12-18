% denseSIFT compute SIFT (Scale-invariant feature transform) descriptors on a regular dense grid
% 
% Usage
% ------
% 
% [dsift , infodsift] = denseSIFT(I , [options] );
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
% 	   kernelx                        Kernel in x-direction for computing the gradient (default kernelx = [0.028 , 0.127 , 0.209 , 0.127 , 0.028 ; 0.026 , 0.116 , 0.192 , 0.116 , 0.026 ; 0 , 0 , 0 , 0 , 0 ; -0.026 , -0.116 ,  -0.192 , -0.116 -0.026 ; -0.028 -0.127 -0.209 -0.127 -0.028])
% 	   kernely                        Kernel in y-direction for computing the gradient (default kernely = [0.028 , 0.127 , 0.209 , 0.127 , 0.028 ; 0.026 , 0.116 , 0.192 , 0.116 , 0.026 ; 0 , 0 , 0 , 0 , 0 ; -0.026 , -0.116 ,  -0.192 , -0.116 -0.026 ; -0.028 -0.127 -0.209 -0.127 -0.028]')
%        weightx                        Weights in x-direction for computing orientation responses (default weightx = [0 , 0.11 , 0.55 , 1 , 0.55 , 0.11 , 0 , 0 , 0])
% 	   weighty                        Weights in y-direction for computing orientation responses (default weightx = [0 , 0.11 , 0.55 , 1 , 0.55 , 0.11 , 0 , 0 , 0]')
% 	   deltax                         Division step in the x-axis for the grid (default deltax = floor(nx*min(scale))) 
% 	   deltay                         Division step in the y-axis for the grid (default deltay = floor(ny*min(scale)))
%        color                          0 : force gray-scale (dimcolor = 1, default), 1 : RGB (dimcolor = 3), 2 : nRGB (dimcolor = 3), 3 : Opponent (dimcolor = 3), 
%                                       4 : nOpponent (dimcolor = 2), 5 : Hue (dimcolor = 1)
% 	   nori                           Number of orientation (default nori = 8)
%        alpha                          Parameter for attenuation of angles, must be odd (default alpha = 9)
% 	   nbins                          Number of bins for histograms (default nbins = 4)
% 	   patchsize                      Size of the patch where the descriptor is computed (default patchsize = 2*nbins+1 )	  
% 	   norm                           Normalization : norm = 0 <=> no normalization, norm = 1 <=> v=v/(sum(v)+epsi), norm = 2 <=> v=v/sqrt(sum(v�)+epsi�), 
% 	                                  norm = 3 <=> v=sqrt(v/(sum(v)+epsi)) , norm = 3 <=> L2-clamped (default norm = 1)
% 	   clamp                          Clamping value (default clamp = 0.2)
% 	   rmmean                         Remove mean value of the current Image at scale s_i: 0 don't remove, 1 remove (default = rmmean = 0)
% 
% 
% Outputs
% -------
% 
% dsift                                 SIFT descriptors (nbins*nbins*nori x nb_pts) where nb_pts = deltax*deltay*nscale*dimcolor
% infodsift                             SIFT descriptors informations(7 x nb_pts)   where nb_pts = deltax*deltay*nscale*dimcolor
%                                       infodsift(1,i) = y
% 									  infodsift(2,i) = x
% 									  infodsift(3,i) = scale
% 									  infodsift(4,i) = color
% 									  infodsift(5,i) = nyscale;
% 									  infodsift(6,i) = nxscale;
% 									  infodsift(7,i) = ||f||_{options.norm}
%
%
% References : [1] Lowe, David G. (1999). "Object recognition from local scale-invariant features". 
% ----------       Proceedings of the International Conference on Computer Vision. 2. pp. 1150�1157
% 
%              [2] Koen E. A. van de Sande, Theo Gevers and Cees G. M. Snoek, 
% 			     "Evaluating Color Descriptors for Object and Scene Recognition", 
% 				 IEEE Transactions on Pattern Analysis and Machine Intelligence, volume 32 (9), pages 1582-1596, 2010
%%


clc,close all, clear all,drawnow
rootbase_dir         = pwd;
core_dir             = fullfile(pwd , 'core');
addpath(core_dir)

co                   = 1;


I                     = imread(fullfile(core_dir , 'M_1 (1).jpg'));

figure(co)
imagesc(I)
colormap(gray)
title('Gray Image example' ,'fontname' , 'times' , 'fontsize' , 13 , 'fontweight','bold')
co                   = co + 1;

%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%  Example 1 %%%%%%%%%%%%%%%%%%%%%%
%
% Extract SIFT over (4x4x8 bins
% from patches (9 x 9) regulary sampled from I with scale = 1.
% sift are L2 normalized
%
%

options.deltax                       = 22;
options.deltay                       = 22;
options.nori                         = 8;
options.alpha                        = 9;
options.nbins                        = 4;
options.patchsize                    = 9;
options.norm                         = 2;

[dsift , infodsift]                  = denseSIFT(I , options ); 

half                                 = options.patchsize/2;
xr                                   = [infodsift(2, :)-half ; infodsift(2, :)-half ; infodsift(2, :)+ half ; infodsift(2, :)+ half ; infodsift(2, :)-half] + 1.5;
yr                                   = [infodsift(1, :)-half ; infodsift(1, :)+half ; infodsift(1, :)+ half ; infodsift(1, :)- half ; infodsift(1, :)-half] + 1.5;


figure(co)
imagesc(I)
colormap(gray)
hold on
plot(infodsift(2 , :)+1.5 , infodsift(1 , :)+1.5 , 'r+')
plot(xr , yr , 'b')
hold off
title(sprintf('Location of %dx%d=%d SIFT patches of size = %dx%d' , options.deltay,options.deltax,options.deltay*options.deltax,options.patchsize,options.patchsize) ,'fontname' , 'times' , 'fontsize' , 13, 'fontweight','bold')
co                   = co + 1;


figure(co)
imagesc(dsift)
title(sprintf('SIFT descriptors with nbins = %d, nori = %d ',options.nbins , options.nori) ,'fontname' , 'times' , 'fontsize' , 13 ,  'fontweight','bold')
h=ylabel('bins');
set(h,'fontsize',12,'fontweight','bold')
co                   = co + 1;

%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%  Example 2 %%%%%%%%%%%%%%%%%%%%%%
%
% Comparaison between denseSIFT.c and sp_dense_sift.m
% sift are L2 normalized
%


options.deltax                       = 5;
options.deltay                       = 15;

[dsift , infodsift]                  = denseSIFT(I , options ); 
[sift_arr, grid_x, grid_y]           = sp_dense_sift(I, 25 , options.patchsize);

% Same sampling point (4,4) %%

disp([infodsift(1:2,1) , floor([grid_y(1,1) ; grid_x(1,1)])])

% Sort sift vectors since there are not ordonered in the same way %%

siftc                                = sort(dsift(: , 1));
siftmatlab                           = sort(squeeze(sift_arr(1,1,:)));

figure(co)

plot(1:options.nbins^2*options.nori,siftc,1:options.nbins^2*options.nori,siftmatlab , 'linewidth' , 2)
axis([0.5 , options.nbins^2*options.nori+0.5 , -0.1 , 1.2*max(siftc)])
legend('denseSIFT (mexfile)', 'denseSIFT (matlab)' , 'location'  , 'northwest')
title(sprintf('Comparaison between SIFT descriptors with nbins = %d, nori = %d ',options.nbins , options.nori) ,'fontname' , 'times' , 'fontsize' , 13 ,  'fontweight','bold')


co                   = co + 1;


%%

I                     = imread(fullfile(core_dir , '02769_Right_StudentOffice.jpeg'));

figure(co)
imagesc(I)
title('Color Image example' ,'fontname' , 'times' , 'fontsize' , 13 , 'fontweight','bold')
co                   = co + 1;



%%%%%%%%%%%%%%%%%%%%%%%%%%%  Example 2 %%%%%%%%%%%%%%%%%%%%%%
%
% Extract SIFT over (4x4x8 bins
% from patches (16 x 16) regulary sampled from I with scale = 1.2.
% Opponent color projection is used
% sift are L2-clamp normalized
%
%
%%

options.deltax                       = 22;
options.deltay                       = 22;
options.color                        = 3;
options.nori                         = 8;
options.alpha                        = 9;
options.nbins                        = 4;
options.patchsize                    = 16;
options.norm                         = 4;
options.clamp                        = 0.2;

options.sigma_edge                   = 1.2;
[options.kernely , options.kernelx]  = gen_dgauss(options.sigma_edge);
options.weightx                      = gen_weight(options.patchsize , options.nbins);
options.weighty                      = options.weightx';

[dsift , infodsift]                  = denseSIFT(I , options ); 

half                                 = options.patchsize/2;
xr                                   = [infodsift(2, :)-half ; infodsift(2, :)-half ; infodsift(2, :)+ half ; infodsift(2, :)+ half ; infodsift(2, :)-half] + 1.5;
yr                                   = [infodsift(1, :)-half ; infodsift(1, :)+half ; infodsift(1, :)+ half ; infodsift(1, :)- half ; infodsift(1, :)-half] + 1.5;


figure(co)
imagesc(I)
colormap(gray)
hold on
plot(infodsift(2 , :)+1.5 , infodsift(1 , :)+1.5 , 'r+')
plot(xr , yr , 'b')
hold off
title(sprintf('Location of %dx%d=%d SIFT patches of size = %dx%d' , options.deltay,options.deltax,options.deltay*options.deltax,options.patchsize,options.patchsize) ,'fontname' , 'times' , 'fontsize' , 13, 'fontweight','bold')
co                   = co + 1;


figure(co)
imagesc(dsift)
title(sprintf('Color SIFT (Opponnent) descriptors with nbins = %d, nori = %d ',options.nbins , options.nori) ,'fontname' , 'times' , 'fontsize' , 13 ,  'fontweight','bold')
h=ylabel('bins');
set(h,'fontsize',12,'fontweight','bold')
co                   = co + 1;
%%
