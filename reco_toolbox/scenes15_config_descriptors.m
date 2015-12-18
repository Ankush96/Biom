%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Local Patches DESCRIPTORS %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

sift.scale                         = 1;
sift.sigma_scale                   = 0.6;
sift.deltax                        = 30;
sift.deltay                        = 30;
sift.color                         = 0;
sift.nori                          = 8;
sift.alpha                         = 9;
sift.nbins                         = 4;
sift.patchsize                     = 16;
sift.norm                          = 4;
sift.clamp                         = 0.2;
sift.sigma_edge                    = 1;
sift.standardize                   = 0;
sift.whithning                     = 0;
[sift.kernely , sift.kernelx]      = gen_dgauss(sift.sigma_edge);
sift.weightx                       = gen_weight(sift.patchsize , sift.nbins);
sift.weighty                       = sift.weightx';
if((sift.color == 0) || (sift.color == 5))
    sift.dimcolor                  = 1;
elseif((sift.color == 1) || (sift.color == 2) ||  (sift.color == 3))
    sift.dimcolor                  = 3;
else
    sift.dimcolor                  = 2;
end
sift.nbscale                       = length(sift.scale);
sift.nbpatches                     = sift.deltax*sift.deltay*sift.nbscale*sift.dimcolor;
sift.patchdim                      = sift.nbins^2*sift.nori;
sift.size                          = sift.nbpatches*sift.patchdim;
sift                               = {sift};




color.scale                         = [0.5 , 0.75 , 1];
color.sigma_scale                   = 0.6;
color.deltax                        = 12;
color.deltay                        = 12;
color.color                         = 0;
color.nbins                         = 12;
color.patchsize                     = 16;
color.norm                          = 4;
color.clamp                         = 0.2;
color.standardize                   = 0;
color.whithning                     = 0;

if((color.color == 0) || (color.color == 5))
    color.dimcolor                  = 1;
elseif((color.color == 1) || (color.color == 2) ||  (color.color == 3))
    color.dimcolor                  = 3;
else
    color.dimcolor                  = 2;
end
color.nbscale                       = length(color.scale);
color.nbpatches                     = color.deltax*color.deltay*color.nbscale*color.dimcolor;
color.patchdim                      = color.nbins;
color.size                          = color.nbpatches*color.patchdim;
color                               = {color};




patch.scale                         = [0.5 , 1];
patch.sigma_scale                   = 0.6;
patch.deltax                        = 10;
patch.deltay                        = 10;
patch.color                         = 0;
patch.patchsize                     = 10;
patch.norm                          = 4;
patch.clamp                         = 0.2;
patch.standardize                   = 1;
patch.whithning                     = 1;
if((patch.color == 0) || (patch.color == 5))
    patch.dimcolor                  = 1;
elseif((patch.color == 1) || (patch.color == 2) ||  (patch.color == 3))
    patch.dimcolor                  = 3;
else
    patch.dimcolor                  = 2;
end
patch.nbscale                       = length(patch.scale);
patch.nbpatches                     = patch.deltax*patch.deltay*patch.nbscale*patch.dimcolor;
patch.patchdim                      = patch.patchsize*patch.patchsize;
patch.size                          = patch.nbpatches*patch.patchdim;
patch                               = {patch};




mblbp.scale                         = [1];
mblbp.deltax                        = 26;
mblbp.deltay                        = 26;
mblbp.color                         = 0;
mblbp.patchsize                     = 26;
mblbp.maptable                      = 0;
mblbp.useFF                         = 0;
mblbp.improvedLBP                   = 0;
mblbp.rmextremebins                 = 1;
mblbp.norm                          = 4;
mblbp.clamp                         = 0.2;
mblbp.standardize                   = 0;
mblbp.whithning                     = 0;

if((mblbp.color == 0) || (mblbp.color == 5))
    mblbp.dimcolor                  = 1;
elseif((mblbp.color == 1) || (mblbp.color == 2) ||  (mblbp.color == 3))
    mblbp.dimcolor                  = 3;
else
    mblbp.dimcolor                  = 2;
end

if(mblbp.maptable==0)
    mblbp.nbbins = 256;
elseif (mblbp.maptable==1)
    mblbp.nbbins = 59;
elseif (mblbp.maptable==2)
   mblbp.nbbins = 36;
elseif (mblbp.maptable==3)
    mblbp.nbbins = 10; 
end

mblbp.nbscale                       = length(mblbp.scale);
mblbp.nbpatches                     = mblbp.deltax*mblbp.deltay*mblbp.nbscale*mblbp.dimcolor;
mblbp.patchdim                      = mblbp.nbbins*(1 + mblbp.improvedLBP);
mblbp.size                          = mblbp.nbpatches*mblbp.patchdim;
mblbp                               = {mblbp};






mbldp.scale                         = [1];
mbldp.deltax                        = 24;
mbldp.deltay                        = 24;
mbldp.n                             = 2;
mbldp.ldporient                     = [0 , 1 , 2];
mbldp.color                         = 0;
mbldp.patchsize                     = 24;
mbldp.improvedLDP                   = 0;
mbldp.rmextremebins                 = 1;
mbldp.norm                          = 4;
mbldp.clamp                         = 0.2;
mbldp.standardize                   = 0;
mbldp.whithning                     = 0;

if((mbldp.color == 0) || (mbldp.color == 5))
    mbldp.dimcolor                  = 1;
elseif((mbldp.color == 1) || (mbldp.color == 2) ||  (mbldp.color == 3))
    mbldp.dimcolor                  = 3;
else
    mbldp.dimcolor                  = 2;
end
mbldp.nbscale                       = length(mbldp.scale);
mbldp.nbpatches                     = mbldp.deltax*mbldp.deltay*mbldp.nbscale*mbldp.dimcolor;
mbldp.patchdim                      = 4*256*(1 + mbldp.improvedLDP);
mbldp.size                          = mbldp.nbpatches*mbldp.patchdim;
mbldp                               = {mbldp};


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% DIRECT DESCRIPTORS %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



mlhoee.spyr                        = [1 , 1 , 1 , 1 ; 1/2 , 1/2 , 1/4 , 1/4 ; 1/4 , 1/4 , 1/8 , 1/8 ; 1/8 , 1/8 , 1/16 , 1/16 ; 1/16 , 1/16 , 1/32 , 1/32];
mlhoee.norma                       = (1)*[1 , 1] ;
mlhoee.kernelx                     = [0.0069 0.0101 0.0115 0.0101 0.0069;0.0684 0.1012 0.1152 0.1012 0.0684;-0 -0 -0 -0 -0;-0.0684 -0.1012 -0.1152 -0.1012 -0.0684;-0.0069 -0.0101 -0.0115 -0.0101 -0.0069];
mlhoee.kernely                     = [0.0069 0.0101 0.0115 0.0101 0.0069;0.0684 0.1012 0.1152 0.1012 0.0684;-0 -0 -0 -0 -0;-0.0684 -0.1012 -0.1152 -0.1012 -0.0684;-0.0069 -0.0101 -0.0115 -0.0101 -0.0069]';
mlhoee.color                       = 0;
mlhoee.nori                        = 12;
mlhoee.bndori                      = 1;
mlhoee.interpolate                 = 1;
mlhoee.norm                        = 0;
mlhoee.clamp                       = 0.2;
mlhoee.nH                          = sum(floor(((1 - mlhoee.spyr(:,1))./(mlhoee.spyr(:,3)) + 1)).*floor((1 - mlhoee.spyr(:,2))./(mlhoee.spyr(:,4)) + 1));

if((mlhoee.color == 0) || (mlhoee.color == 5))
    mlhoee.dimcolor                = 1;
elseif((mlhoee.color == 1) || (mlhoee.color == 2) ||  (mlhoee.color == 3))
    mlhoee.dimcolor                = 3;
else
    mlhoee.dimcolor                = 2;
end
mlhoee.size                        = mlhoee.nH*mlhoee.nori;
mlhoee                             = {mlhoee};



mlhmslsd.q                         = 2;
mlhmslsd.d                         = 8;
mlhmslsd.eps                       = -15.0;
mlhmslsd.n_bins                    = 16256;
mlhmslsd.max_grad                  = 260100;
mlhmslsd.spyr                      = [1 , 1 , 1 , 1 ; 1/2 , 1/2 , 1/8 , 1/8 ; 1/4 , 1/4 , 1/10 , 1/10];
mlhmslsd.scale                     = [0.25 , 0.5 , 1];
mlhmslsd.sigma_scale               = 0.6;
mlhmslsd.color                     = 0;
mlhmslsd.nori                      = 12;
mlhmslsd.nlength                   = 12;
mlhmslsd.norm                      = [0 , 0 , 4];
mlhmslsd.nH                        = sum(floor(((1 - mlhmslsd.spyr(:,1))./(mlhmslsd.spyr(:,3)) + 1)).*floor((1 - mlhmslsd.spyr(:,2))./(mlhmslsd.spyr(:,4)) + 1));

if((mlhmslsd.color == 0) || (mlhmslsd.color == 5))
    mlhmslsd.dimcolor              = 1;
elseif((mlhmslsd.color == 1) || (mlhmslsd.color == 2) ||  (mlhmslsd.color == 3))
    mlhmslsd.dimcolor              = 3;
else
    mlhmslsd.dimcolor              = 2;
end

mlhmslsd.size                      = mlhmslsd.dimcolor*mlhmslsd.nlength*mlhmslsd.nori*mlhmslsd.nH;
mlhmslsd                           = {mlhmslsd};




mlhmslbp.spyr                      = [1 , 1 , 1 , 1 ; 1/2 , 1/2 , 1/4 , 1/4];
mlhmslbp.scale                     = [1 , 2 , 3 , 4];
mlhmslbp.color                     = 0;
mlhmslbp.maptable                  = 0;
mlhmslbp.improvedLBP               = 0;
mlhmslbp.rmextremebins             = 1;
mlhmslbp.norm                      = [0 , 0 , 4];
mlhmslbp.clamp                     = 0.2;
mlhmslbp.nH                        = sum(floor(((1 - mlhmslbp.spyr(:,1))./(mlhmslbp.spyr(:,3)) + 1)).*floor((1 - mlhmslbp.spyr(:,2))./(mlhmslbp.spyr(:,4)) + 1));

if((mlhmslbp.color == 0) || (mlhmslbp.color == 5))
    mlhmslbp.dimcolor              = 1;
elseif((mlhmslbp.color == 1) || (mlhmslbp.color == 2) ||  (mlhmslbp.color == 3))
    mlhmslbp.dimcolor              = 3;
else
    mlhmslbp.dimcolor              = 2;
end
if(mlhmslbp.maptable==0)
    mlhmslbp.nbbins = 256;
elseif (mlhmslbp.maptable==1)
    mlhmslbp.nbbins = 59;
elseif (mlhmslbp.maptable==2)
    mlhmslbp.nbbins = 36;
elseif (mlhmslbp.maptable==3)
    mlhmslbp.nbbins = 10; 
end

mlhmslbp.size                      = length(mlhmslbp.scale)*mlhmslbp.nH*mlhmslbp.dimcolor*mlhmslbp.nbbins*(mlhmslbp.improvedLBP+1);
mlhmslbp                           = {mlhmslbp};




mlhmsldp.spyr                      = [1 , 1 , 1 , 1 ; 1/2 , 1/2 , 1/4 , 1/4];
mlhmsldp.scale                     = [1 , 2 , 3];
mlhmsldp.n                         = 2;
mlhmsldp.color                     = 0;
mlhmsldp.improvedLDP               = 0;
mlhmsldp.rmextremebins             = 1;
mlhmsldp.norm                      = [0 , 0 , 4];
mlhmsldp.clamp                     = 0.2;
mlhmsldp.nH                        = sum(floor(((1 - mlhmsldp.spyr(:,1))./(mlhmsldp.spyr(:,3)) + 1)).*floor((1 - mlhmsldp.spyr(:,2))./(mlhmsldp.spyr(:,4)) + 1));

if((mlhmsldp.color == 0) || (mlhmsldp.color == 5))
    mlhmsldp.dimcolor              = 1;
elseif((mlhmsldp.color == 1) || (mlhmsldp.color == 2) ||  (mlhmsldp.color == 3))
    mlhmsldp.dimcolor              = 3;
else
    mlhmsldp.dimcolor              = 2;
end

mlhmsldp.size                      = length(mlhmsldp.scale)*mlhmsldp.nH*mlhmsldp.dimcolor*4*256*(1+mlhmsldp.improvedLDP);
mlhmsldp                           = {mlhmsldp};
