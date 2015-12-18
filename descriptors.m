function [features, valid_points] = descriptors(I, channel, erode_threshold, numOctaves, numScaleLevels)

if nargin<5
    numScaleLevels = 30;  % Increse this for finer increment of scales
    if nargin < 4
        numOctaves = 1;  % Decrease this for detecting smaller blobs
        if nargin < 3
            erode_threshold = 80;   % Increase this to remove more boundary area
            if nargin < 2
                channel = 3;    % 3 for blue, 2 for green, 1 for red
            end
        end
    end
end
% Apply CLAHE
b = adapthisteq(I(:,:,channel)); 
% Search for the dark parts in the image
b_mask =1. - double(b<10);
% Remove the area near the boundaries so that we can remove features in
% that area
b_mask = imerode(b_mask, ones(erode_threshold, erode_threshold));
% Uncomment the following line to see the mask after erosion
%imagesc(b_mask);

%% Extract SURF features
%points = detectSURFFeatures(b, 'NumOctaves', numOctaves, 'NumScaleLevels', numScaleLevels);
%[features, valid_points] = extractFeatures(b, points,'SURFSize',128);

%% Extract features based on 'Block' method from a dense grid
% figure; imshow(b); hold on;
grid_size = 16;
i = grid_size;
j = grid_size;
points_grid = [];
while i < size(b,1)
    while j < size(b,2)
        if b_mask(i,j) >0.1
            p = [j i];
            points_grid = [points_grid ; p];
%             plot(j, i, 'go');
%             drawnow;
        else
%             plot(j, i, 'rx');
%             drawnow;
        end    
        
        j = j+grid_size;
    end
    i = i+grid_size;
    j = grid_size;
end

%plot(points_grid, 'bo');
[features_grid, points_grid ] = extractFeatures(b, points_grid, 'BlockSize', 11);
size(features_grid);
features = double(features_grid)./255;
valid_points = points_grid;

% for i =1:length(valid_points)
%     x = valid_points(i,1);
%     y = valid_points(i,2);
%     plot(x, y, 'go');
%     drawnow;
% end    

%% Uncomment following line and all corresponding 'plot' commands to visualise
% figure; imshow(b); hold on;

%% Removing SURF points not within the mask
%fprintf('Initial size of features is %d %d \n', size(features));
%fprintf('Initial size of valid points is %d %d \n', size(valid_points));
% For every point we check if it lies near the boundary. If yes then that
% point is considered invalid
%k = 0;

% i = 1;
% while(i <=length(valid_points) )   
%     p = valid_points(i).Location; 
%     if b_mask(round(p(2)),round(p(1))) < 0.1
%         % The invalid points are plotted with red crosses
%         %plot(p(1), p(2), 'rx');
%         valid_points(i)=[];
%         %k = k+1;
%         features(i,:) = [];
%     else
%         % The valid points with green
%         %plot(p(1),p(2),'go');
%         %drawnow;
%         i = i+1;
%     end
% end 
%plot(valid_points, 'go');
%fprintf('Final size of features is %d %d \n', size(features));
%fprintf('Rejected features num is %d \n', k);
%fprintf('Final size of valid points is %d %d\n', size(valid_points));
%pause;
%close;
