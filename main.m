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
%            fprintf('%d ->fi Test\n', label_ind(j));
%        end    
%     end    
%    pause
end    

train_images = images(train);
train_labels = labels(train);

test_images = images(test);
test_labels = labels(test);

numClusters = 4;
%% Training
[train_features, train_feat_index, class_index] = mult_descriptors(train_images, train_labels);
[train_histograms, Centers] = gen_hist(train_images, numClusters, train_features, train_feat_index, class_index, numDir);

%% Testing
test_histograms = zeros(length(test_images), numClusters);
predicted_label = ones(length(test_labels),1);
for i = 1:length(test_images)
    fprintf('Image number %d / %d \n', i, length(test_images));
    test_images(i)
    img = imread(char(test_images(i)));
    [f,~] = descriptors(img); 
    idx = knnsearch(Centers, f);
    hist_image = hist(idx, numClusters);
    hist_image = hist_image/sum(hist_image);
    test_histograms(i,:) = hist_image;
    % Now we find the closest training image histogram
    x = knnsearch(train_histograms, hist_image, 'K', 5); 
    y = train_labels(x)
    un = unique(y);
    if length(y) == length(un)
        x = y(1);
    else
        count = un;
        for j=1:length(count)
            count(j) = length(find(y == un(j)));
        end
        max_voted = un(count == max(count));            
        if length(max_voted) == 1
            x = max_voted;
        else
            for j=1:length(y)
                if ~isempty(find(y(j) == max_voted))
                    x = y(j);
                    break
                end    
            end
        end    
    end    
    
    predicted_label(i) = x;
    fprintf('Predicted %d. Actual %d \n', x, test_labels(i));
end
mean(predicted_label == test_labels)