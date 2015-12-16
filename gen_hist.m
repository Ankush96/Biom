function [histograms, Centers] = gen_hist(train_images, numClusters, features, index)

histograms = zeros(length(train_images), numClusters);

%% Running K-means clustering on descriptors
%fprintf('Calculating cluster centroids...\n');
%[idx, Centers] = kmeans(features, numCLusters, 'EmptyAction', 'singleton', 'replicates',5);
%fprintf('Calculating Histograms of each image...\n');

%% Running LBG for quantisation
Centers = vqlbg(features', numClusters)';
if sum(sum(isnan(Centers))) >0
    fprintf('Nan in Centres\n');
end
idx = knnsearch(Centers, features);
%% Generating histograms

fprintf('Centroids found. Building histograms . . . \n');
for i=1:length(train_images)
    image_clusters = idx(index(i):index(i+1)-1);
    hist_image = hist(image_clusters, numClusters);
    histograms(i,:) = hist_image/sum(hist_image);
end    
fprintf('Completed !!\n');

