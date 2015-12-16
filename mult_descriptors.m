function [features, index] = mult_descriptors(train_images)

l = floor(length(train_images)/30);
features =[];
index =1;

%% Progress bar
fprintf('Calculating descriptors...\n');
fprintf('Progress [ ');
        for j = 1:30
            fprintf(' ');
        end
fprintf(']\n');

%% Reading each image and finding descriptors

for i = 1:length(train_images)
    %fprintf('Reading image %d\n', i);
    img = imread(char(train_images(i)));
    [f,v] = descriptors(img);
    %fprintf('Calculated descriptors for image of size %d %d\n', size(f));
    features = [ features; f];
    index= [index; index(i) + size(f,1)];  
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
    
end
fprintf('Final descriptors of size %d %d\n', size(features));