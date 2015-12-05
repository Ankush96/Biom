addpath('frangi/');
img = imread('M_1 (1).jpg');


subplot(2,2,1);
imshow(adapthisteq(img(:,:,2))); 
title('G');

subplot(2,2,2);
imshow(adapthisteq(img(:,:,3), 'ClipLimit',0.03)); 
title('B');

subplot(2,2,3);
i = FrangiFilter2D(double(adapthisteq(img(:,:,2),'Cliplimit',0.03)));
imshow(i);
title('G_Frangi');

subplot(2,2,4);
j = FrangiFilter2D(double(adapthisteq(img(:,:,3),'Cliplimit',0.03)));
imshow(j)
title('B_Frangi');