addpath('frangi/');                 % Adding path to Frangi filter
addpath('coyefilter/');                 % Adding path to Coye filter
img = imread('M_1 (1).jpg');        % Trying on a sample sclara image

%---------------------------Frangi-------------------------
subplot(2,2,1);
imshow(adapthisteq(img(:,:,2)));    % CLAHE on Green channel 
title('G');

subplot(2,2,2);                     % CLAHE on Blue channel
imshow(adapthisteq(img(:,:,3), 'ClipLimit',0.03)); 
title('B');

subplot(2,2,3);                     % Frangi filter on CLAHE Green channel 
i = FrangiFilter2D(double(adapthisteq(img(:,:,2),'Cliplimit',0.03)));
imshow(i);
title('G_Frangi');

subplot(2,2,4);                     % Frangi filter on CLAHE Blue channel
j = FrangiFilter2D(double(adapthisteq(img(:,:,3),'Cliplimit',0.03)));
imshow(j)
title('B_Frangi');
%------------------------------------------------------------

%---------------------------Coye------------------------------
subplot(2,2,1);
imshow(adapthisteq(img(:,:,2)));    % CLAHE on Green channel 
title('G');

subplot(2,2,2);                     % CLAHE on Blue channel
imshow(adapthisteq(img(:,:,3), 'ClipLimit',0.03)); 
title('B');

subplot(2,2,3);                     %Coye filter on CLAHE Green channel 
i = FrangiFilter2D(double(adapthisteq(img(:,:,2),'Cliplimit',0.03)));
imshow(i);
title('G_Frangi');

subplot(2,2,4);                     % Coye filter on CLAHE Blue channel
j = FrangiFilter2D(double(adapthisteq(img(:,:,3),'Cliplimit',0.03)));
imshow(j)
title('B_Frangi');