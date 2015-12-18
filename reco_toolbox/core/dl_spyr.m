function h =  dl_spyr(D , X , z  , options)

%
% Returns histogram from spatial pyramidal pooling of patches X
% versus trained dictionary D
%
% Usage
% ------
%
% h =  dl_spyr(D , X , z  , options);
%
%
% Inputs
% -------
%
% D                    Trained dictionary (d x K x nscale x ncolor)
% X                    Patches extracted from an image I
% z                    Patches information (location (x,y), scale, color) in image I
% options
%        spyr          Spatial Pyramid (nspyr x 4) (default [1 , 1 , 1 , 1] with nspyr = 1)
%                      where spyr(i,1) is the ratio of ny in y axis of the blocks at level i (by(i) = spyr(i,1)*ny)
%                      where spyr(i,2) is the ratio of ny in y axis of the shifting at level i (deltay(i) = spyr(i,2)*ny)
%                      where spyr(i,3) is the ratio of nx in x axis of the blocks at level i (bx(i) = spyr(i,3)*nx)
%                      where spyr(i,3) is the ratio of nx in x axis of the shifting at level i (deltax(i) = spyr(i,4)*nx)
%
%        scale         Scaling vector associated with patches extraction
%
%        assignement   0 : max-pooling, 1  : mean-pooling, 2 : square root of mean of energy pooling
%
% 	     norm          Normalization vector (1 x 3) : [for all subwindows, for each subwindows of a pyramid level, for each subwindows]
%                      norm = 0 <=> no normalization, norm = 1 <=> v=v/(sum(v)+epsi), norm = 2 <=> v=v/sqrt(sum(v²)+epsi²),
% 	                   norm = 3 <=> v=sqrt(v/(sum(v)+epsi)) , norm = 4 <=> L2-clamped (default norm = 1)
% 	     clamp         Clamping value (default clamp = 0.2)
%
%
% Output
% ------
%
% h                 Handle on figure associated with confusion matrix
%
%
%
%
% Author : Sébastien PARIS : sebastien.paris@lsis.org
% -------  Date : 12/13/2010


spyr                        = options.spyr;
norm                        = options.norm;
clamp                       = options.clamp;
assignment                  = options.assignment;
scale                       = options.scale;
[d , K , nscale , ncolor]   = size(D);
np                          = size(spyr , 1);
%maxfactor                   = max(spyr(: , 1).*spyr(: , 2));

wperlayer                   = floor(((1 - options.spyr(:,1))./(options.spyr(:,3)) + 1)).*floor((1 - options.spyr(:,2))./(options.spyr(:,4)) + 1);
nH                          = sum(wperlayer);
h                           = zeros(K , nH*nscale*ncolor);

tiny                        = 10e-8;
verytiny                    = 10e-16;

co                          = 1;


for c = 1 : ncolor
    for s = 1 : nscale
        
        currentscale           = scale(s);
        index                  = find((z(3 , :) == currentscale) & (z(4 , :) == c));
        y                      = z(1 , index);
        x                      = z(2 , index);
        nycurrent              = z(5 , index(1));
        nxcurrent              = z(6 , index(1));
        
        G                      = full(abs(mexLasso(X(: , index) , D(: , : , s , c) , options)))'; %(np x K)
        
        for p = 1 : np
            
            scaley      = spyr(p , 3);
            ly          = (1 - spyr(p,1))/scaley + 1;
            deltay      = nycurrent*scaley;
            sy          = nycurrent*spyr(p,1);
            offsety     = max(0 , ( floor(nycurrent - ( (ly-1)*deltay + sy + 1)) ));
            
            scalex      = spyr(p , 4);
            lx          = (1 - spyr(p,2))/scalex + 1;
            deltax      = nxcurrent*scalex;
            sx          = nxcurrent*spyr(p,2);
            offsetx     = max(0 , ( floor(nxcurrent - ( (lx-1)*deltax + sx + 1)) ));
            
            nHp         = wperlayer(p);
            hp          = zeros(K*nHp , 1);
            cop         = 0;
            %            ratio       = maxfactor/(spyr(p , 1)*spyr(p , 2));
            
            for l = 1 : lx
                origx = offsetx + (l-1)*deltax + 1;
                for m = 1 : ly
                    origy              = offsety + (m-1)*deltay + 1;
                    
                    ind                = (y >= origy) & (y <= origy+sy) & (x >= origx) & (x <= origx+sx);
                    tempG              = G(ind , :);
                    
                    if(assignment == 0)     %max pooling of the absolute value
                        tmp            = max(tempG , [] , 1);
                    elseif(assignment == 1) % mean pooling of the absolute value
                        tmp            = mean(tempG , 1);
                    elseif(assignment == 2) % square root of mean of energy pooling
                        tmp            = sqrt(mean(tempG.*tempG , 1));
                    end
                    
                    if(norm(3) == 1)
                        sumtmp         = sum(tmp);
                        tmp            = tmp/(sumtmp + tiny);
                    end
                    if(norm(3) == 2)
                        sumtmp         = sum(tmp.*tmp);
                        tmp            = tmp/sqrt(sumtmp + verytiny);
                    end
                    if(norm == 3)
                        sumtmp         = sum(tmp);
                        tmp            = sqrt(tmp/(sumtmp + tiny));
                    end
                    if(norm == 4)
                        sumtmp         = sum(tmp.*tmp);
                        tmp            = tmp/sqrt(sumtmp + verytiny);
                        tmp(tmp>clamp) = clamp;
                        sumtmp         = sum(tmp.*tmp);
                        tmp            = tmp/sqrt(sumtmp + verytiny);
                    end
                    
                    hp(cop*K+1:(cop+1)*K) = tmp;
                    cop                   = cop + 1;
                    
                end
            end
            
            if(norm(2) == 1)
                sumtmp              = sum(hp);
                hp                  = hp/(sumtmp + tiny);
                h(: , co:co+cop-1)  = reshape(hp , K , nHp);
            end
            if(norm(2) == 2)
                sumtmp              = sum(hp.*hp);
                hp                  = hp/sqrt(sumtmp + verytiny);
                h(: , co:co+cop-1)  = reshape(hp , K , nHp);
            end
            if(norm(2) == 3)
                sumtmp              = sum(hp);
                hp                  = sqrt(hp/(sumtmp + tiny));
                h(: , co:co+cop-1)  = reshape(hp , K , nHp);
            end
            if(norm(2) == 4)
                sumtmp              = sum(hp.*hp);
                hp                  = hp/sqrt(sumtmp + verytiny);
                hp(hp>clamp)        = clamp;
                sumtmp              = sum(hp.*hp);
                hp                  = hp/sqrt(sumtmp + verytiny);
                h(: , co:co+cop-1)  = reshape(hp , K , nHp);
            end
            co                      = co + cop;
        end
    end
end

h                       = reshape(h , K*nH*nscale*ncolor , 1);

if(norm(1) == 1)
    sumtmp              = sum(h);
    h                   = h/(sumtmp + tiny);
end
if(norm(1) == 2)
    sumtmp              = sum(h.*h);
    h                   = h/sqrt(sumtmp + verytiny);
end
if(norm(1) == 3)
    sumtmp              = sum(h);
    h                   = sqrt(h/(sumtmp + tiny));
end
if(norm(1) == 4)
    sumtmp              = sum(h.*h);
    h                   = h/sqrt(sumtmp + verytiny);
    h(h>clamp)          = clamp;
    sumtmp              = sum(h.*h);
    h                   = h/sqrt(sumtmp + verytiny);
end



