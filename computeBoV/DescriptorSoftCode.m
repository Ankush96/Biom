function BoW=DescriptorSoftCode(sift,C,param)
%% BoW=DescriptorSoftCode(sift,C,param)
%
%  This function codes the image descriptors over a codebook C 
%  Each image is a set of local(SIFT) features, meaning
%  SIFT features are the visual words and A user given codebook is the
%  dictionary.
%
%  input  : sift - a cell array containg features from all the images of the
%           database. N X 1 array of cells, N is the database size. 
%           Each array entry should contain features as its columns
%           
%           C  - centroids/codebook/cluster centers. Each column is
%           a codeword
%
%  param   : set of parameters like
%
%            dis_type
%            K in K-NN soft assignment
%            sigma in the exp{-1/2*(distance)^2} while soft assignment
%            pooltype 'sum' or 'max'
%            verbose
%
%  BoW    : Bag of Visual words representation for all the database images
%  
%
%  Requirement : yael library from INRIA "https://gforge.inria.fr/projects/yael/"
%
%  Author : Mopuri K Reddy, VAL, SERC, IISc, Banglore.
%  sercmkreddy@ssl.serc.iisc.in


if(nargin < 2 || nargin > 3)
        error('incorrect number of i/p arguments');
else if(nargin<3)
        
        param.k=floor(0.08*size(C,2));
        % k-NN pooling for soft assignment
        % default is assign to 8% of the number of the total codebook
        
        param.pooltype='s';
        % type of pooling       's' -> sum      'm' -> max
        
        param.verbose=0;
        % display the status or not
        
        param.sigma=100;
        % used while soft assignment
        
        param.dis_type=2;
        % distance type for nearest 
    end
     
end

N=length(sift);
% number of images in the database

% d=size(C,1);
% dimension of the local feature

M=size(C,2);
% size of the codebook

BoW=single(zeros([M N]));
% Bag of visual words Representation

tic
for i=1:N
    
    [ids, dis]=yael_nn(single(C),single(sift{i}),param.k,param.dis_type);
    % Soft descriptor coding (assignment)
    
    try
    BoW(:,i)=poolResponses(ids,dis,M,param);
    % pools the descriptors' responses over the codebook
    catch
    end
    if(param.verbose)
        fprintf('inside the function DescriptorSoftCode, %d / %d\n',i,N);
        % prints the status
    end
    
end
toc
