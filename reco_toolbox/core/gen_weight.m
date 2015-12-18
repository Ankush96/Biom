function weight    = gen_weight(patch_size , nbin)

if(nargin < 1)
    patch_size = 9;
end
if(nargin < 2)
    nbin       = 4;
end

r             = patch_size/2;
cx            = r - 0.5;
sample_res    = patch_size/ nbin;
weight        = abs((1:patch_size) - cx)/sample_res;
weight        = (1 - weight) .* (weight <= 1);
