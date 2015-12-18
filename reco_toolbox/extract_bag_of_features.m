%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Script to compute features with dictionary learning+pooling approach
%
% i)   edit "xxxx_config_descriptors.m" to modify dense descriptor parameters
%
% ii)  edit "xxxx_config_encoding.m" to modify dictionary learning algorithms
%       parameters
%
% iii) edit "xxxx_config_features.m" to modify spatial pooling parameters
%      with a spatial pyramidal architecture
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


clc,close all, clear ,drawnow

database_name        = {'scenes15' , 'skinan' , 'agemen'};
database_ext         = {'jpg' , 'jpg' , 'png'};
descriptors_name     = {'denseSIFT' , 'denseCOLOR' , 'densePATCH' , 'denseMBLBP' , 'denseMBLDP'};
encoding_name        = {'yael_kmeans' , 'dltrain'};
features_name        = {'mlhbow_spyr' , 'dl_spyr' , 'mlhlcc_spyr'};


%% Parameters %%
choice_database      = [1]; %scenes15=1/skinan=2/agemen=3
choice_descriptors   = [1]; %denseSIFT=1/denseCOLOR=2/densePATCH=3/denseMBLBP=4/denseMBLDP=5
choice_encoding      = [2]; %Kmeans=1/Sparse Learning =2
choice_feature       = [2]; %SP-Histogram = 1/Sparse Pooling = 2/LCC = 3

do_extract_patches   = 1;   %no=0/yes=1
do_encoding          = 1;   %no=0/yes=1
do_compute_features  = 1;   %no=0/yes=1
dicoshared           = 0;   %no=0/yes=1

%%

data_name            = database_name{choice_database(1)};
im_ext               = database_ext{choice_database(1)};

rootbase_dir         = pwd;
images_dir           = fullfile(pwd , 'images' , data_name);
core_dir             = fullfile(pwd , 'core');
feat_dir             = fullfile(pwd , 'features');
dico_dir             = fullfile(pwd , 'dico');
des_dir              = fullfile(pwd , 'descriptors');
addpath(core_dir)
dirim                = dir(images_dir);
nb_topic             = length(dirim) - 2;
classe_name          = cellstr(char(dirim(3:nb_topic+2).name))';

eval([data_name , '_config_descriptors']);
eval([data_name , '_config_encoding']);
eval([data_name , '_config_features']);


%%

nbimagespertopic     = zeros(1 , nb_topic);
for i = 1:nb_topic
    dir_name            = dir(fullfile(pwd  , 'images' , data_name , dirim(i+2).name , ['*.' , im_ext]));
    nbimagespertopic(i) = length(dir_name);
end
N                    = sum(nbimagespertopic);

descriptors_param    = cell(1 , length(descriptors_name));
descriptors_param{1} = sift;
descriptors_param{2} = color;
descriptors_param{3} = patch;
descriptors_param{4} = mblbp;
descriptors_param{5} = mbldp;


descriptors_size     = cell(1 , length(descriptors_name));
descriptors_size{1}  = descriptors_param{1}{1}.size;
descriptors_size{2}  = descriptors_param{2}{1}.size;
descriptors_size{3}  = descriptors_param{3}{1}.size;
descriptors_size{4}  = descriptors_param{4}{1}.size;
descriptors_size{5}  = descriptors_param{5}{1}.size;

encoding_param       = cell(1 , length(encoding_name));
encoding_param{1}    = yael;
encoding_param{2}    = spams;

features_param       = cell(1 , length(features_name));
features_param{1}    = mlhbow_feat;
features_param{2}    = dl_feat;
features_param{3}    = mlhlcc_feat;

nb_descriptors       = length(choice_descriptors);
nb_encoding          = length(choice_encoding);
nb_features          = length(choice_encoding);

if(do_extract_patches)
    for j  = 1 : nb_descriptors
        current_descriptor = choice_descriptors(j);
        current_size       = descriptors_size{current_descriptor};
        base_descriptor    = descriptors_name{current_descriptor};
        nbpatches          = descriptors_param{current_descriptor}{1}.nbpatches;
        nbpatchetotal      = nbpatches*N;
        standardize        = descriptors_param{current_descriptor}{1}.standardize;
        whithning          = descriptors_param{current_descriptor}{1}.whithning;
        patchdim           = descriptors_param{current_descriptor}{1}.patchdim;
        
        eval(['descriptors = @' base_descriptor ';']);
        fprintf('descriptor = %s \n\n' , base_descriptor)
        drawnow
        
        X                  = zeros(descriptors_size{current_descriptor} , N , 'single');
        Z                  = zeros(nbpatches*6 , N , 'single');
        S                  = zeros(2 , N , 'uint16');
        Y                  = zeros(nbpatches , N , 'single');
        y                  = zeros(1 , N );
        co                 = 1;
        
        for t = 1 : nb_topic
            
            current_path  = fullfile(pwd , 'images' , data_name , dirim(t+2).name);
            current_dir   = dir(fullfile(current_path , ['*.' , im_ext]));
            current_topic = char(classe_name(t));         
            for i = 1 : length(current_dir)
                
                I                   = imread(fullfile(current_path , current_dir(i).name));
                
                fprintf('descriptor = %s, topic = %s (%d/%d), image = %s (%d/%d)\n' , base_descriptor , current_topic, t , nb_topic , current_dir(i).name , i , nbimagespertopic(t))
                drawnow
                
                [des , fea]         = descriptors(I , descriptors_param{current_descriptor}{:});
                
                X(: , co)           = reshape(single(des) , current_size , 1);
                Z(: , co)           = reshape(single(fea(1:6,:)) , nbpatches*6 , 1);
                Y(: , co)           = t*ones(nbpatches , 1);
                y(co)               = t;
                co                  = co + 1;
            end
        end
        
        if(dicoshared)
            Z(3:4 , :)                                        = 1;
            descriptors_param{current_descriptor}{1}.scale    = 1;
            descriptors_param{current_descriptor}{1}.nbscale  = 1;
            descriptors_param{current_descriptor}{1}.dimcolor = 1;
            features_param{current_features}.scale            = 1;
        end
        
        X                           = reshape(X , patchdim , nbpatchetotal);
        Z                           = reshape(Z , [6 , nbpatchetotal]);
        Y                           = reshape(Y , 1 , nbpatchetotal);
        
        if(standardize)
            fprintf('Standardize patches\n' )
            drawnow
            mX            = mean(X , 2);
            stdX          = std(X , 0 , 2);
            stdX(stdX==0) = 1;
            X             = (X - mX(: , ones(1 , size(X , 2))))./stdX(: , ones(1 , size(X , 2)));            
            fprintf('End Standardize\n' )
            drawnow
        end
        if(whithning)
            
            fprintf('Whithning patches\n' )
            drawnow
            
            % SVD for
            %           [S,V,D] = svd(X',0);
            %           T       = (D*diag(sqrt(1 ./(diag(V) + 0.1))))*D';
            
            % EIG for
            covX    = (1/(size(X,2)-1))*(X*X');
            [V,D]   = eig(covX);
            T       = (V*diag(sqrt(1 ./(diag(D) + 0.1))))*V';
            X       = T * X;
            
            fprintf('End whithning\n' )
            drawnow
            
        end
        
        fprintf('Saving patches descriptor  %s ...\n' , [data_name , '_' , base_descriptor]);
        drawnow 
        save(fullfile(des_dir , [data_name , '_' , base_descriptor]) , 'X' , 'Z'  , 'Y' , 'y' , 'classe_name' , 'patchdim' , 'nbpatches' , 'N'  , '-v7.3')
        clear X Z Y y;
    end
end

if(do_encoding)
    for j  = 1 : nb_encoding 
        current_encoding = choice_encoding(j);
        base_encoding    = encoding_name{current_encoding};
        eval(['encoding = @' base_encoding ';']);
        
        for i  = 1 : nb_descriptors
            
            current_descriptor = choice_descriptors(i);
            base_descriptor    = descriptors_name{current_descriptor};
            
            nbpatches          = descriptors_param{current_descriptor}{1}.nbpatches;
            nbpatchetotal      = nbpatches*N;
            patchdim           = descriptors_param{current_descriptor}{1}.patchdim;
            nbscale            = descriptors_param{current_descriptor}{1}.nbscale;
            dimcolor           = descriptors_param{current_descriptor}{1}.dimcolor;
            K                  = encoding_param{current_encoding}{current_descriptor}.K;
            nbpatchesperclass  = encoding_param{current_encoding}{current_descriptor}.nbpatchesperclass;
            D                  = zeros(patchdim , K , nbscale , dimcolor , 'single');
            
            fprintf('Loading patches descriptor %s ...\n' , [data_name , '_' , base_descriptor]);
            drawnow
            
            load(fullfile(des_dir , [data_name , '_' , base_descriptor]) , 'X' , 'Z' , 'Y' , 'y' , 'classe_name')
            
            for c = 1 : dimcolor
                for s = 1 : nbscale
                    currentscale      = descriptors_param{current_descriptor}{1}.scale(s);
                    index             = find( (Z(3 , :) == currentscale ) & (Z(4 , :) == c) );
                    Yindex            = Y(index);
                    lindex            = length(index);
                    indexdico         = [];
                    for t = 1 : nb_topic
                        indt          = find(Yindex == t);
                        lindt         = length(indt);
                        idx           = randperm(lindt);
                        idxtemp       = idx(1:min(nbpatchesperclass ,lindt));
                        indexdico     = [indexdico , index(indt(idxtemp))];
                    end
                    
                    fprintf('Learning dictionary of K = %d words with encoder = %s, scale = %5.3f, dimcolor = %d from %d/%d patches of %s \n' , K , base_encoding , currentscale , c , length(indexdico) , lindex , base_descriptor)
                    drawnow
                    D(: , : , s , c)  = encoding(X(: , indexdico) , encoding_param{current_encoding}{current_descriptor});
                end
            end
            
            fprintf('Saving dictionary  %s ...\n' , [data_name , '_' , base_encoding , '_' , base_descriptor]);
            drawnow
            
            save(fullfile(dico_dir , [data_name , '_' , base_encoding , '_' , base_descriptor]) , 'D');
            clear X Z Y y classe_name;
            
        end
    end
end

if(do_compute_features)
    for j  = 1 : nb_encoding
        current_features   = choice_feature(j);
        base_features      = features_name{current_features};
        current_encoding   = choice_encoding(j);
        base_encoding      = encoding_name{current_encoding};
        eval(['features = @' base_features ';']);
        for i  = 1 : nb_descriptors
            current_descriptor                                          = choice_descriptors(i);
            base_descriptor                                             = descriptors_name{current_descriptor};
            nbpatches                                                   = descriptors_param{current_descriptor}{1}.nbpatches;
            nbpatchetotal                                               = nbpatches*N;
            patchdim                                                    = descriptors_param{current_descriptor}{1}.patchdim;
            nbscale                                                     = descriptors_param{current_descriptor}{1}.nbscale;
            dimcolor                                                    = descriptors_param{current_descriptor}{1}.dimcolor;
            features_param{current_features}{current_descriptor}.scale  = descriptors_param{current_descriptor}{1}.scale;
            features_param{current_features}{current_descriptor}.L      = patchdim;
            
            current_feature_param                                       = features_param{current_features}{current_descriptor};
            
            fprintf('Loading dictionnary %s ...\n' , [data_name , '_' , base_encoding , '_' , base_descriptor]);
            drawnow
            load(fullfile(dico_dir , [data_name , '_' , base_encoding , '_' , base_descriptor]) , 'D');
            
            fprintf('Loading patches descriptor %s ...\n' , [data_name , '_' , base_descriptor]);
            drawnow
            
            load(fullfile(des_dir , [data_name , '_' , base_descriptor]) , 'X' , 'Z'  , 'y' ,  'classe_name');
            
            K                                                           = size(D , 2);
            nH                                                          = current_feature_param.nH;
            X                                                           = reshape(X , descriptors_size{current_descriptor} , N);
            Z                                                           = reshape(Z , 6*nbpatches , N);
            F                                                           = zeros(K*nH*nbscale*dimcolor , N);
            
            co                                                          = 1;
            for t = 1 : nb_topic
                current_path  = fullfile(pwd , 'images' , data_name , dirim(t+2).name);
                current_dir   = dir(fullfile(current_path , ['*.' , im_ext]));
                current_topic = char(classe_name(t));
                for i = 1 : length(current_dir)
                    fprintf('encoder = %s, topic = %s (%d/%d), patches = %s , image = %s (%d/%d)\n' , base_features , current_topic, t , nb_topic , base_descriptor , current_dir(i).name , i , nbimagespertopic(t))
                    drawnow
                    
                    XX              = reshape(X(: , co) , patchdim , nbpatches);
                    ZZ              = reshape(Z(: , co) , 6 , nbpatches);
                    F(: , co)       = features(D , XX , ZZ  , current_feature_param);
                    co              = co + 1;
                end
            end
            
            X                  = F;
            clear F Z S;
            fprintf('Saving features  %s ...\n' , [data_name , '_' , base_descriptor , '_' , base_features]);
            drawnow
            save(fullfile(feat_dir , [data_name , '_' , base_descriptor , '_' , base_features]) , 'X' , 'y' , 'classe_name' , '-v7.3');
        end
    end
end
