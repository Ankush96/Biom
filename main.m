%% Splitting the data into training and testing sets
[images,labels, numDir] = dir_read('../Data/');
train =[];
test = [];
for i = 1:numDir
    %fprintf('Directory label %d\n', i);
    label_ind = find(labels == i);
    [train_ind, ~, test_ind] = dividerand(length(label_ind), 0.7, 0.0, 0.3);
    train = [train; label_ind(train_ind)];
    test = [test; label_ind(test_ind)];
%     for j = 1:length(label_ind)
%        if sum(j == train_ind)
%            fprintf('%d -> Train\n', label_ind(j));
%        else 
%            fprintf('%d -> Test\n', label_ind(j));
%        end    
%     end    
%    pause
end    

train_images = images(train);
train_labels = labels(train);

test_images = images(test);
test_labels = labels(test);

numClusters = 64;
%% Training
%[features, index] = mult_descriptors(train_images);
%[histograms, Centers] = gen_hist(train_images, numClusters, features, index);

%% Testing
predicted_label = ones(length(test_labels),1);
for i = 1:length(test_images)
    fprintf('Image number %d / %d \n', i, length(test_images));
    img = imread(char(test_images(i)));
    [f,~] = descriptors(img);    
    idx = knnsearch(Centers, f);
    hist_image = hist(idx, numClusters);
    hist_image = hist_image/sum(hist_image);
    % Now we find the closest training image histogram
    x = knnsearch(histograms, hist_image); 
    predicted_label(i) = train_labels(x);
end    