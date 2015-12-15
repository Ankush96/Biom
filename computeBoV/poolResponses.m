function B=poolResponses(ind,dis,CBsize,param)
%% Pools the responses of the descriptors(words/features) over a codebook
%
%   i/p     :
%               ind   = indices of the codewords that are getting assigned
%               dis   = corresponding distances
%           CBsize    = the codebook size
%           param     = parameters
%
%   o/p
%                  B  = the pooled representation of the responses
%
%     Author: Mopuri K Reddy, VAL, SERC, IISc, Banglore,
%     sercmkreddy@ssl.serc.iisc.in
%     Date: 31/10/14

tmp=single(zeros(CBsize,size(ind,2)));

beta=1/(param.sigma^2);
% 1/sig^2 in the exponent of the gaussian kernel
% better to be chosen via cross validation

for i=1:size(ind,2)
    tmp(ind(:,i),i)=exp(-1*beta*dis(:,i));
    % dis is already squared distance
end

switch (param.pooltype)
    case 'm'
        B=max(tmp,[],2);
    case 's'
        B=sum(tmp,2);
    otherwise
        error('invalid pooltype');
end