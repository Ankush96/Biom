%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Script to compute features with direct approach 
%
% i)   edit "xxxx_config_descriptors.m" to modify dense descriptor parameters
%
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
clc,close all, clear all,drawnow


database_name        = {'scenes15' , 'skinan' , 'agemen'};
database_ext         = {'jpg' , 'jpg' , 'png'};
descriptors_name     = {'mlhoee_spyr' , 'mlhmslsd_spyr' , 'mlhmslbp_spyr' , 'mlhmsldp_spyr'};

%%

choice_database      = [1]; %scenes15=1/skinan=2/agemen=3
choice_descriptors   = [3]; %mlhoee_spyr=1/mlhmslsd_spyr=2/mlhmslbp_spyr=3/mlhmsldp_spyr=4

do_extract_feature   = 1;   %no=0/yes=1

%%

data_name            = database_name{choice_database(1)};
im_ext               = database_ext{choice_database(1)};

rootbase_dir         = pwd;
images_dir           = fullfile(pwd , 'images' , data_name);
core_dir             = fullfile(pwd , 'core');
feat_dir             = fullfile(pwd , 'features');
des_dir              = fullfile(pwd , 'descriptors');
addpath(core_dir)

eval([data_name , '_config_descriptors']);


dirim                = dir(images_dir);
nb_topic             = length(dirim) - 2;
classe_name          = cellstr(char(dirim(3:nb_topic+2).name))';
nbimagespertopic     = zeros(1 , nb_topic); 
for i = 1:nb_topic
    dir_name            = dir(fullfile(pwd  , 'images', data_name , dirim(i+2).name , ['*.' , im_ext]));
    nbimagespertopic(i) = length(dir_name);
end
N                    = sum(nbimagespertopic);


descriptors_param    = cell(1 , length(descriptors_name));
descriptors_param{1} = mlhoee;
descriptors_param{2} = mlhmslsd;
descriptors_param{3} = mlhmslbp;
descriptors_param{4} = mlhmsldp;


descriptors_size     = cell(1 , length(descriptors_name));
descriptors_size{1}  = descriptors_param{1}{1}.size;
descriptors_size{2}  = descriptors_param{2}{1}.size;
descriptors_size{3}  = descriptors_param{3}{1}.size;
descriptors_size{4}  = descriptors_param{4}{1}.size;


nb_descriptors       = length(choice_descriptors);

if(do_extract_feature)    
    for j  = 1 : nb_descriptors
        
        current_descriptor = choice_descriptors(j);
        current_size       = descriptors_size{current_descriptor};
        base_descriptor    = descriptors_name{current_descriptor};
           
        eval(['descriptors = @' base_descriptor ';']);
        
        X                  = zeros(descriptors_size{current_descriptor} , N);
        y                  = zeros(1 , N );
        co                 = 1;
                
        for t = 1 : nb_topic        
            current_path  = fullfile(pwd , 'images' , data_name , dirim(t+2).name);
            current_dir   = dir(fullfile(current_path , ['*.' , im_ext]));
            current_topic = char(classe_name(t));
            for i = 1 : length(current_dir)
                
                I                  = imread(fullfile(current_path , current_dir(i).name));
                X(: , co)          = descriptors(I , descriptors_param{current_descriptor}{:});            
                y(co)              = t;
                
                fprintf('descriptor = %s, topic = %s (%d/%d), image = %s (%d/%d)\n' , base_descriptor , current_topic, t , nb_topic , current_dir(i).name , i ,  nbimagespertopic(t))
                drawnow
                co                 = co + 1;
            end
        end
                
        save(fullfile(feat_dir , [data_name , '_' , base_descriptor]) , 'X'  , 'y' , 'classe_name')
        clear X y;
        
    end
end




