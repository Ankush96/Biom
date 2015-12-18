function [features, image_index, class_index] = mult_descriptors(train_images, labels)

l = floor(length(train_images)/30);
features =[];
class_index =1;
image_index = 1;

%% Progress bar
fprintf('Calculating descriptors...\n');
fprintf('Progress [ ');
        for j = 1:30
            fprintf(' ');
        end
fprintf(']\n');

%% Reading each image and finding descriptors
class_features = [];
labels = [labels; labels(end)+1];
for i = 1:length(train_images)
    %fprintf('Reading image %d label %d\n', i, labels(i));
    img = imread(char(train_images(i)));
    [f,~] = descriptors(img);
    %fprintf('Initial Feature size of one image = %d\n', size(f,1));
    image_index = [image_index; image_index(end) + size(f,1)];
    
    % Now stack these features into an aggregate of features from same
    % class first. We reject them if they are complete uniqe(outlier)
    % For finding this we compare all our descriptors in a class
    % If the Euclidean distance of a particular descriptor from all other
    % descriptors is very high, we assume it is an outlier and reject it
    
    thresh = 3e-3; % Distance threshold for rejecting a descriptor
    %fprintf('Calculated descriptors for image of size %d %d\n', size(f));
    
    if labels(i) ==labels(i+1)
        class_features =[class_features; f];
    else
        % Now the featurs of the previous class has been calculated. Time
        % to reject outliers and empty the class features matrix.
        fprintf('Currently in class %d\n',labels(i));
               
        class_features =[class_features; f];
        class_index = [class_index;class_index(end)];
        fprintf('Initial size of features of one class %d \n',size(class_features,1));
        % Calculate offset so as to redadjust image_index when some
        % descriptors are removed
        offset = class_index(end) -1;  
        % Start removing descriptors
        id = image_index;
        for j = 1:size(class_features, 1)
            x = class_features(j,:);
            for k = 1:size( class_features,1)
                y = class_features(k,:);
                if (j~=k && (norm(x-y)/length(x)) < thresh)
                    %fprintf('Distance is %d\n',(norm(x-y)/length(x)));
                    features = [ features; x];
                    class_index(end) = class_index(end)+1;
                    break
                elseif k==size(class_features,1)
                    % A descriptor was removed. Now we need to adjust image
                    % index accordingly
                    rejected_descriptor_index = offset + j;
                    
                    % If image_index was currently [1;4;9] and descriptor
                    % number 2 is removed, we will start from the end of
                    % image_index and reduce all elements by 1 until we
                    % reach an element <= rejected_descriptor_index
                    image_index = image_index - double(id > rejected_descriptor_index);
                end 
            end
        end
        % Empty the matrix
        fprintf('Final features retained %d\n',class_index(end) - class_index(end-1));
               
        class_features = [];
            

    end
    % Uncomment following if you do not want to see progress
    if mod(i, l) == 0
        fprintf('Progress [');
        for j = 1:(i/l)
            fprintf('-');
        end    
        while (j<=30)
            fprintf(' ');
            j = j+1;
        end 
        fprintf(']\n');
    end
    % ------------------------------------------------------
    fprintf('Image_index- %d Class_index - %d\n',image_index(end),class_index(end));
        
end
fprintf('Final descriptors of size %d %d\n', size(features));