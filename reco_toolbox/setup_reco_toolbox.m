conf.path           = pwd;
conf.imdir          = fullfile('images','scenes15') ;
conf.coredir        = 'core';
conf.descriptorsdir = 'descriptors';
conf.dicodir        = 'dico';
conf.featdir        = 'features';
conf.modelsdir      = 'models';
cd(conf.path)

%%

% --------------------------------------------------------------------
%                Create empty dir if absent
% --------------------------------------------------------------------


if (~exist(fullfile(conf.path,conf.descriptorsdir), 'dir'))
    mkdir(fullfile(conf.path,conf.descriptorsdir)) ;
end

if (~exist(fullfile(conf.path,conf.dicodir), 'dir'))
    mkdir(fullfile(conf.path,conf.dicodir)) ;
end

if (~exist(fullfile(conf.path,conf.featdir), 'dir'))
    mkdir(fullfile(conf.path,conf.featdir)) ;
end

if (~exist(fullfile(conf.path,conf.modelsdir), 'dir'))
    mkdir(fullfile(conf.path,conf.modelsdir)) ;
end

if (~exist(fullfile(conf.path,conf.imdir), 'dir'))
    mkdir(fullfile(conf.path,conf.imdir)) ;
end

%%
% --------------------------------------------------------------------
%                Download 15 scenes data
% --------------------------------------------------------------------


if  ((exist(fullfile(conf.path,conf.imdir), 'dir')) && (~exist(fullfile(conf.path,conf.imdir, 'bedroom'),'dir')))
    databaseurl = 'http://www.cs.unc.edu/~lazebnik/research/scene_categories.zip' ;
    fprintf('Downloading 15-scene data to ''%s''.\nThis will take a while (81.2 Mo to download, no progress bar available).\n', conf.imdir) ;
    unzip(databaseurl, fullfile(conf.path,conf.imdir)) ;
end

%%

% --------------------------------------------------------------------
%                Unzip SPAMS & Static libs
% --------------------------------------------------------------------

C  = computer;

cd(fullfile(conf.path,conf.coredir));


if(ispc)  
    if(strcmp(C , 'PCWIN'))
        fprintf('PC win32 detected, unzip ''libw32.zip''\n') ;
        unzip('libw32.zip');
    elseif(strcmp(C , 'PCWIN64'))
        fprintf('PC win64 detected, unzip ''libw64.zip''\n') ;
        unzip('libw64.zip');
    end
else
    if(strcmp(C , 'GLNX86'))   
        fprintf('Linux32 detected, unzip ''libgln32.zip''\n') ;
        unzip('libgln32.zip');
    elseif(strcmp(C , 'GLNXA64'))
        fprintf('Linux64 detected, unzip ''libgln64.zip''\n') ;
        unzip('libgln64.zip');
    end
end

%%

% --------------------------------------------------------------------
%                Compile mex-files
% --------------------------------------------------------------------

cd(conf.path);


fprintf('Compile mex-files in core dir\n') ;

try
    
%    options.config_file = 'mexopts_intel11_64.bat';
%    options.ext         = 'mexw64';
%    options.makespams   = 1;
%    mexme_reco(options);

%    options.config_file = 'mexopts_intel10.bat';
%    options.ext         = 'mexw32';
%    options.makespams   = 0;
%    mexme_reco(options);


   mexme_reco;

    
catch ME
    if(ispc)
        if(strcmp(C , 'PCWIN'))
            fprintf('Failed to compile mex-files, unzip precompiled mex32 in core dir\n') ;
            unzip('mexw32.zip');
        elseif(strcmp(C , 'PCWIN64'))
            fprintf('Failed to compile mex-files, unzip precompiled mex64 in core dir\n') ;
            unzip('mexw64.zip');
        end
    else
        if(strcmp(C , 'GLNX86'))    
        elseif(strcmp(C , 'GLNXA64'))
        end
    end
end

fprintf('Setup done !!!!\n\nPlease run demos for basic illustrations.\n\n');
fprintf('Run/Edit extract_bag_of_features.m or extract_direct_features.m to compute features \n\n') ;
fprintf('Run/Edit simple_train.m to train/evaluate system \n\n') ;




