function BoWvec=computeBoV(Vwords,feats,Nind)
%% Computes the Bag of Visual word ( BoV/BoW/BoF ) representation of an image or video
%               via hard voting (i.e, assignment to the nearest(K=1) codeword)
%
% Inputs->
%
% Vwords(C) : visual words or centroids or cluster centers (each column is a word)
% feats     : features extracted from an entity (image/video)(each column is a feature)
% Nind      : Normalizatin index with the number of features(words) in the bag
%
%             0-no normalization
%             1-with noramlization (making it independent of number of features in the image)
%
% ***************************  Needs INRIA's yael library ******************
% Output->
%
% BoWvec : output BoW vector (column Vector)
% 
% Author: Mopuri K Reddy, SERC, IISC Banglore, 24/7/13.
%  sercmkreddy@ssl.serc.iisc.in

if(Nind~=0 || Nind~=1)
    disp('please specify correct normalization from 0, 1, 2');
    return;
end
if(size(Vwords,1)~=size(feats,1))
    disp('please check the i/p arguments: sizes are not matching');
    return;
end

% addpath(genpath('path to yael library'));
% adds the 'yael' library here. ( If not added already, uncomment the above statement; 
% If it is added already, no need to uncomment )

dis_type=2;
% you can specify the distance type here.

[idx,~]=yael_nn(single(Vwords),single(feats),1,dis_type);
BoWvec = hist( double(idx), 1:size(Vwords,2) );

if(Nind==1)
    BoWvec=BoWvec/size(feats,2);
    % normalization wrt the numnber of features(words) in the image
end
BoWvec=BoWvec';