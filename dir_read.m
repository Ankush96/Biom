function [ images, labels ] = dir_read( root )
% A function that reads all the images in all subdirectories of the
% folder location specified by root and returns the complete vector
% of images along with their labels


folders = dir(root);  % Read the contents of root
folders = folders(3:end, :); % Remove '.' and '..' from the array 
images = cellstr(['a']);
labels = [];
for i = 1:size(folders,1)
    filepattern = sprintf('%s%s/*.jpg',root,folders(i).name);
    jpgfiles = dir(filepattern);
    for k = 1:length(jpgfiles)
       basefileName = jpgfiles(k).name;
       fullfileName = sprintf('%s%s/%s',root,folders(i).name,basefileName);
       %display(fullfileName);
       %img = imread(fullfileName);
       %imshow(img);  
       %drawnow; % Force display to update immediately.
       images = [images ; fullfileName];
       labels = [labels ; str2num(folders(i).name)];
    end    
end
images = images(2:end);
end