%% Generate Histograms form Training images with given number of CLusters
%  
%  SURF Descriptors of each image in the training set is calculated.
%  These 128 Dimensional vectors are clustered into 'numClusters' 
%  clusters. Each descriptor in an image is assigned a centroid
%  and a histogram is calculated for each image storing the num of 
%  descriptors in each bin. 
%--------------------------------------------------------------------

[train_images,labels] = dir_read('../Data/');
[features, index] = mult_descriptors(train_images);
gen_hist(train_images, 64, features, index);