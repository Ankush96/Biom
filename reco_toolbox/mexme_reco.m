function mexme_reco(options)

% Compile each mex-files of the core functions of the reco_toolbox.
%
%  Usage
%  ------
%
%  mexme_reco([options])
%
%  Inputs
%  -------
%
%  options            options strucure
%                     config_file   If user need a particular compiler config file (default config_file = []).
%                     ext           Extention to the compiled files (default ext = [])
%                     makespams     Compile SPAMS function for windows system
%
%
%
%  Example1 (with MSVC compiler or LCC compiler)
%  -------
%
%  mexme_reco;
%
%  Example2 (with Intel Compiler )
%  -------
%
%  options.config_file = 'mexopts_intel10.bat';
%  options.ext         = 'dll';
%  options.makespams   = 1;
%  mexme_reco(options);
%
%
%  Example3 (with Intel Compiler )
%  -------
%
%  options.config_file = 'mexopts_intel11_64.bat';
%  options.ext         = 'mexw64';
%  options.makespams   = 1;
%  mexme_reco(options);
%
%
%  Author : Sébastien PARIS : sebastien.paris@lsis.org
%  -------  Date : 12/07/2010
%

% doc mex.getCompilerConfigurations


if( (nargin < 1) || isempty(options) )
    options.config_file = [];
    options.ext         = [];
    options.makespams   = 1;    
end

if(~any(strcmp(fieldnames(options) , 'config_file')))
    options.config_file = [];
end
if(~any(strcmp(fieldnames(options) , 'ext')))    
    options.ext = [];
end
if(~any(strcmp(fieldnames(options) , 'makespams')))
    options.makespams = 0;
end
if(~exist(options.config_file , 'file'))  
    options.config_file = [];
end


mexOpts   = [];
C         = computer;

switch C
  case {'PCWIN64', 'GLNXA64' , 'MACI64'}
    %mexOpts = [mexOpts , ' -largeArrayDims '] ;
end

switch C
  case {'GLNXA64' , 'MACI64'}
    mexOpts = [mexOpts , ' CFLAGS="\$CFLAGS -fopenmp -Wall" LDFLAGS="\$LDFLAGS -fopenmp" '] ;
end


if(ispc)
    
    if (~exist(options.config_file , 'file'))
        optpath             = prefdir;
        mexoptfile          = [optpath filesep 'mexopts.bat'];
    else
        mexoptfile          = options.config_file;
    end
    
    res                     = getmexopts(mexoptfile);
    
    if(strcmp(res , 'lcc'))
        tempdir             =  fullfile(matlabroot , 'extern\lib\win32\lcc');
    elseif(strcmp(res , 'cl') || strcmp(res , 'icl'))
        tempdir             =  fullfile(matlabroot , sprintf('extern\\lib\\%s\\microsoft', computer('arch')));
    end
    
    if(exist(fullfile(tempdir , 'libmwblas.lib') , 'file') == 2)
        
        libblas   = ['"' , fullfile(tempdir , 'libmwblas.lib') , '"'];
        liblapack = ['"' , fullfile(tempdir , 'libmwlapack.lib') , '"'];
        
    else % prior version of matlab % 
        libblas = ['"' , fullfile(tempdir , 'libmwlapack.lib') , '"'];
        liblapack = ['"' , fullfile(tempdir , 'libmwlapack.lib') , '"'];
    end
else
    libblas        = '-lmwblas';
    liblapack      = '-lmwlapack';
end


core_dir           = fullfile(pwd , 'core');
addpath(core_dir)

cd(core_dir)


warning off

files1   = {'imresize.c ' , 'perf_classif.c ' , 'rgb2gray.c ' , 'sigmoid_train.c ' , ...
    'denseSIFT.c ' , 'denseCOLOR.c ' , 'densePATCH.c ' , 'denseMBLDP.c ' , ...
    'mlhmslsd_spyr.c '  , 'mlhoee_spyr.c ','mlhmsldp_spyr.c ' , ...
    'mlhbow_spyr.c ' , ...
    'pegasos_train.c ' , 'homker_predict.c ' , 'homkermap.c ' , 'homker_pegasos_train.c ' , ...
    'train_dense.c linear_model_matlab.c linear.cpp tron.cpp daxpy.c ddot.c dnrm2.c dscal.c -D_DENSE_REP ' , ...
    'predict_dense.c linear_model_matlab.c linear.cpp tron.cpp daxpy.c ddot.c dnrm2.c dscal.c -D_DENSE_REP ' , ...
    'svmtrain.c svm.cpp svm_model_matlab.c -DopenMP -largeArrayDims ' , ...
    'svmpredict.c svm.cpp svm_model_matlab.c -largeArrayDims ' , ...
    'precomp_model.cpp  fiksvm.cpp svm.cpp svm_model_matlab.c ' , ...
    'fiksvm_predict.cpp  fiksvm.cpp svm.cpp svm_model_matlab.c '};

%% LBP codes must be linked with FFTW %%

files2   = {'denseMBLBP.c ' , 'mlhmslbp_spyr.c '};

%% yeal (kmeans) must be linked eventually with pthreads %%

files3   = {'yael_kmeans.c kmeans.c binheap.c machinedeps.c nn.c sorting.c vector.c vlad.c'};

%% mlhlcc must be linked with LAPACK %%

files4   = {'mlhlcc_spyr.c '};

%% spams2 must be linked with MKL %%

files5   = {'mexLasso.cpp -largeArrayDims ' , 'mexTrainDL.cpp -largeArrayDims ' , 'mexTrainDL_Memory.cpp -largeArrayDims '};
mkl32    = {'mkl_core.lib' , 'mkl_intel_c.lib' , 'mkl_intel_thread.lib' , 'libiomp5md.lib'};
mkl64    = {'mkl_core.lib' , 'mkl_intel_lp64.lib' , 'mkl_intel_thread.lib'};
    


if(~exist('mexLasso.cpp' , 'file'))
    options.makespams = 0;
end


%% Main Files %%
for i = 1 : length(files1)
    
    str  = [];
    if(~isempty(options.config_file))
        str = ['-f ' , options.config_file , ' '];
    end
    
    temp               = files1{i};
    ind                = find(isspace(temp));
    [dummy , name]     = fileparts(temp(1:ind-1));
    
    if(~isempty(options.ext))
        str = [str , '-output ' , name , '.' , options.ext , ' '];
    end
    
    str                = [str , temp];
    
    disp(['compiling ' name])
    eval(['mex ' mexOpts ' ' str ])
end

%% LBP files linked with FFTW %%

for i = 1 : length(files2)
    
    str  = [];
    if(~isempty(options.config_file))
        str = ['-f ' , options.config_file , ' '];
    end
    
    temp               = files2{i};
    ind                = find(isspace(temp));
    [dummy , name]     = fileparts(temp(1:ind-1));
    
    if(~isempty(options.ext))
        str = [str , '-output ' , name , '.' , options.ext , ' '];
    end
    
    str                    = [str , temp];
    
    if(strcmp(C , 'PCWIN'))
        str                = [str , 'libfftw_x32.lib'];
        
    elseif(strcmp(C , 'PCWIN64'))  
        str                = [str , 'libfftw_x64.lib'];
    elseif(strcmp(C , 'GLNX86'))
        str                = [str , 'libfftw_glnx32.a'];
    elseif(strcmp(C , 'GLNXA64'))
        str                = [str , 'libfftw_glnx64.a'];
    end
    disp(['compiling ' name])
    eval(['mex ' mexOpts ' ' str ])
end

%% Yael files linked with BLAS and eventually phreads %%

for i = 1 : length(files3)
    
    str  = '-DPTW32_STATIC_LIB ';
    if(~isempty(options.config_file))
        str = [str, ' -f ' , options.config_file , ' '];
    end
    if(ispc)
        %      str = [str , ' COMPFLAGS="$COMPFLAGS /MD" '];
        str = [str , ' OPTIMFLAGS="$OPTIMFLAGS /MD" '];
        
    end
    
    temp               = files3{i};
    ind                = find(isspace(temp));
    [dummy , name]     = fileparts(temp(1:ind-1));
    
    if(~isempty(options.ext))
        str = [str , '-output ' , name , '.' , options.ext , ' '];
    end
    
    str                = [str , temp];
    
    if(strcmp(C , 'PCWIN'))
        str            = [str , ' pthreadVC2_x32.lib '];
    elseif(strcmp(C , 'PCWIN64'))
        str            = [str , ' pthreadVC2_x64.lib '];
    end
    
    str            = [str , libblas];
    disp(['compiling ' name])
    eval(['mex ' mexOpts ' ' str ])
end

for i = 1 : length(files4)
    
    str                = [];
    
    if(~isempty(options.config_file))
        str = [str, ' -f ' , options.config_file , ' '];
    end
   
    
    temp               = files4{i};
    ind                = find(isspace(temp));
    [dummy , name]     = fileparts(temp(1:ind-1));
    
    if(~isempty(options.ext))
        str = [str , '-output ' , name , '.' , options.ext , ' '];
    end
    
    str                = [str , temp , liblapack];
        
    disp(['compiling ' name])
    eval(['mex ' mexOpts ' ' str ])
end


if(options.makespams)
    
    if(ispc)
        
        if(strcmp(C , 'PCWIN') )
            
            inteldir  = 'C:\Program Files\Intel\Compiler';
            localdir1 = fullfile('mkl','ia32','lib');
            localdir2 = fullfile('lib','ia32');
        elseif(strcmp(C , 'PCWIN64'))
            inteldir = 'C:\Program Files (x86)\Intel\Compiler';
            localdir1 = fullfile('mkl','em64t','lib');
            localdir2 = fullfile('mkl','em64t','lib');
        end
        if(exist(inteldir ,'dir'))
            
            tempdir  = dir(inteldir);
            release  = tempdir(3).name;
            
            tempdir  = dir(fullfile(inteldir , release));
            version  = tempdir(3).name;
            libdir1  = fullfile(inteldir , release , version , localdir1);
            libdir2  = fullfile(inteldir , release , version , localdir2);
            
        else         
            disp('Intel MKL not localized, can''t compile spams functions');
            return;
            
        end
        
        libstr = [];
        if(strcmp(C , 'PCWIN') )
            for i = 1:length(mkl32)-1
                libstr = [libstr , '"', fullfile(libdir1 , mkl32{i}) , '" '];
            end
            libstr = [libstr , '"', fullfile(libdir2 , mkl32{length(mkl32)}) , '" '];
            
        elseif(strcmp(C , 'PCWIN64') )
            
            for i = 1:length(mkl64)-1
                libstr = [libstr , '"', fullfile(libdir1 , mkl64{i}) , '" '];
            end
            libstr = [libstr , '"', fullfile(libdir2 , mkl64{length(mkl64)}) , '" '];
            
        end
        
        for i = 1 : length(files5)
            
            str  = [];
            if(~isempty(options.config_file))
                str = ['-f ' , options.config_file , ' '];
            end
            
            temp               = files5{i};
            ind                = find(isspace(temp));
            [dummy , name , ext]     = fileparts(temp(1:ind-1));
            
            if(~isempty(options.ext))
                str = [str , '-output ' , name , '.' , options.ext , ' '];
            end
            
            str                = [str , temp , ' ' libstr];
            
            if(exist([name , ext] , 'file'))
                disp(['compiling ' name])
                eval(['mex ' mexOpts ' ' str ])       
            end
        end 
    end
end

%%

cd ..


warning on



function res = getmexopts(mexoptfile)
% function res = getmexopts(Tag)
% Get the MCC or MEX configuration
% Author Bruno Luong <brunoluong@yahoo.com>
% Last update: 29-Jun-2009


Tag = 'COMPILER';

% Try to get MEX option first
fid=fopen(mexoptfile,'r');

if fid>0
    iscompilerline=@(S) (strcmp(S,['set ' Tag]));
    C=textscan(fid,'%s %s', 'delimiter', '=', 'whitespace', '');
    fclose(fid);
    cline=find(cellfun(iscompilerline,C{1}));
    if isempty(cline)
        error('getmexopt [Bruno]: cannot get Tag %s', Tag)
    end
    res=C{2}{cline};
    root=regexprep(matlabroot,'\\','\\\\');
    res = regexprep(res,'%MATLAB%',root);
else
    error('getmexopts [Bruno]: cannot open comopts.bat file')
end

