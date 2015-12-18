param_classif             = cell(length(descriptors_name) , length(classifier));

%% Sift patches%%

param_classif{1,1}        = struct('s' , 2 , 'B' , 1 , 'c' , 0.07 , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 0);
param_classif{1,2}        = struct('lambda' , 1/(N*0.1) , 'biasmult' , 1 , 'nbite' , 15*N , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 2 , 'numsubdiv' , 8 ,  'minexponent' , -20 , 'maxexponent' , 8);
param_classif{1,3}        = struct('s' , 0 , 't' , 0 , 'c' , 0.07 , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 0);
param_classif{2,1}        = struct('s' , 2 , 'B' , 1 , 'c' , 0.35 , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 0); %0.81=(s=2,c=0.1)
param_classif{2,2}        = struct('lambda' , 1/(N*0.1) , 'biasmult' , 1 , 'nbite' , 15*N , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 2 , 'numsubdiv' , 8 ,  'minexponent' , -20 , 'maxexponent' , 8);
param_classif{2,3}        = struct('s' , 0 , 't' , 0 , 'c' , 0.07 , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 0);
param_classif{3,1}        = struct('s' , 2 , 'B' , 1 , 'c' , 0.1 , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 0); %0.81=(s=2,c=0.1)
param_classif{3,2}        = struct('lambda' , 1/(N*0.1) , 'biasmult' , 1 , 'nbite' , 15*N , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 2 , 'numsubdiv' , 8 ,  'minexponent' , -20 , 'maxexponent' , 8);
param_classif{3,3}        = struct('s' , 0 , 't' , 0 , 'c' , 0.07 , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 0);

%% Color patches%%

param_classif{4,1}        = struct('s' , 2 , 'B' , 1 , 'c' , 0.1 , 'n' , 0 , 'L' , 1.5 , 'kerneltype' , 2); %0.1=0.832,
param_classif{4,2}        = struct('lambda' , 1/(N*0.1) , 'biasmult' , 1 , 'nbite' , 15*N , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 2 , 'numsubdiv' , 8 ,  'minexponent' , -20 , 'maxexponent' , 8);
param_classif{4,3}        = struct('s' , 0 , 't' , 0 , 'c' , 0.07 , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 0);
param_classif{5,1}        = struct('s' , 2 , 'B' , 1 , 'c' , 0.35, 'n' , 0 , 'L' , 1.5 , 'kerneltype' , 2); %0.1=0.832,
param_classif{5,2}        = struct('lambda' , 1/(N*0.1) , 'biasmult' , 1 , 'nbite' , 15*N , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 2 , 'numsubdiv' , 8 ,  'minexponent' , -20 , 'maxexponent' , 8);
param_classif{5,3}        = struct('s' , 0 , 't' , 0 , 'c' , 0.07 , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 0);
param_classif{6,1}        = struct('s' , 2 , 'B' , 1 , 'c' , 0.1, 'n' , 0 , 'L' , 1.5 , 'kerneltype' , 2);
param_classif{6,2}        = struct('lambda' , 1/(N*0.1) , 'biasmult' , 1 , 'nbite' , 15*N , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 2 , 'numsubdiv' , 8 ,  'minexponent' , -20 , 'maxexponent' , 8);
param_classif{6,3}        = struct('s' , 0 , 't' , 0 , 'c' , 0.07 , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 0);

%% Pixels patches%%

param_classif{7,1}        = struct('s' , 2 , 'B' , 1 , 'c' , 0.07 , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 2 ); %0.8456=(s=2,c=0.1) %
param_classif{7,2}        = struct('lambda' , 1/(N*0.1) , 'biasmult' , 1 , 'nbite' , 15*N , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 2 , 'numsubdiv' , 8 ,  'minexponent' , -20 , 'maxexponent' , 8);
param_classif{7,3}        = struct('s' , 0 , 't' , 0 , 'c' , 0.07 , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 0);
param_classif{8,1}        = struct('s' , 2 , 'B' , 1 , 'c' , 0.35 , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 2 ); %0.8456=(s=2,c=0.1) %
param_classif{8,2}        = struct('lambda' , 1/(N*0.1) , 'biasmult' , 1 , 'nbite' , 15*N , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 2 , 'numsubdiv' , 8 ,  'minexponent' , -20 , 'maxexponent' , 8);
param_classif{8,3}        = struct('s' , 0 , 't' , 0 , 'c' , 0.07 , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 0);
param_classif{9,1}        = struct('s' , 2 , 'B' , 1 , 'c' , 0.1 , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 2); %0.8456=(s=2,c=0.1) %
param_classif{9,2}        = struct('lambda' , 1/(N*0.1) , 'biasmult' , 1 , 'nbite' , 15*N , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 2 , 'numsubdiv' , 8 ,  'minexponent' , -20 , 'maxexponent' , 8);
param_classif{9,3}        = struct('s' , 0 , 't' , 0 , 'c' , 0.07 , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 0);

%% MBLBP patches%%

param_classif{10,1}       = struct('s' , 2 , 'B' , 1 , 'c' , 0.2 , 'n' , 0 , 'L' , 1 , 'kerneltype' , 0); %0.8456=(s=2,c=0.1) %
param_classif{10,2}       = struct('lambda' , 1/(N*0.1) , 'biasmult' , 1 , 'nbite' , 15*N , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 2 , 'numsubdiv' , 8 ,  'minexponent' , -20 , 'maxexponent' , 8);
param_classif{10,3}       = struct('s' , 0 , 't' , 0 , 'c' , 0.2 , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 0);
param_classif{11,1}       = struct('s' , 2 , 'B' , 1 , 'c' , 0.07 , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 2); %0.8456=(s=2,c=0.1) %
param_classif{11,2}       = struct('lambda' , 1/(N*0.1) , 'biasmult' , 1 , 'nbite' , 15*N , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 2 , 'numsubdiv' , 8 ,  'minexponent' , -20 , 'maxexponent' , 8);
param_classif{11,3}       = struct('s' , 0 , 't' , 0 , 'c' , 0.07 , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 0);
param_classif{12,1}       = struct('s' , 2 , 'B' , 1 , 'c' , 0.1 , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 2); %0.8456=(s=2,c=0.1) %
param_classif{12,2}       = struct('lambda' , 1/(N*0.1) , 'biasmult' , 1 , 'nbite' , 15*N , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 2 , 'numsubdiv' , 8 ,  'minexponent' , -20 , 'maxexponent' , 8);
param_classif{12,3}       = struct('s' , 0 , 't' , 0 , 'c' , 0.07 , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 0);

%% MBLDP patches%%


param_classif{13,1}       = struct('s' , 2 , 'B' , 1 , 'c' , 0.1 , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 2); %0.8456=(s=2,c=0.1) %
param_classif{13,2}       = struct('lambda' , 1/(N*0.1) , 'biasmult' , 1 , 'nbite' , 15*N , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 2 , 'numsubdiv' , 8 ,  'minexponent' , -20 , 'maxexponent' , 8);
param_classif{13,3}       = struct('s' , 0 , 't' , 0 , 'c' , 0.07 , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 0);
param_classif{14,1}       = struct('s' , 2 , 'B' , 1 , 'c' , 0.1 , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 2); %0.8456=(s=2,c=0.1) %
param_classif{14,2}       = struct('lambda' , 1/(N*0.1) , 'biasmult' , 1 , 'nbite' , 15*N , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 2 , 'numsubdiv' , 8 ,  'minexponent' , -20 , 'maxexponent' , 8);
param_classif{14,3}       = struct('s' , 0 , 't' , 0 , 'c' , 0.07 , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 0);
param_classif{15,1}       = struct('s' , 2 , 'B' , 1 , 'c' , 0.1 , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 2); %0.8456=(s=2,c=0.1) %
param_classif{15,2}       = struct('lambda' , 1/(N*0.1) , 'biasmult' , 1 , 'nbite' , 15*N , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 2 , 'numsubdiv' , 8 ,  'minexponent' , -20 , 'maxexponent' , 8);
param_classif{15,3}       = struct('s' , 0 , 't' , 0 , 'c' , 0.07 , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 0);


%% mlhoee_spyr %%

param_classif{16,1}       = struct('s' , 2 , 'B' , 1 , 'c' , 0.0001 , 'n' , 0 , 'L' , 1.5 , 'kerneltype' , 2);
param_classif{16,2}       = struct('lambda' , 1/(N*0.01) , 'biasmult' , 1 , 'nbite' , 15*N , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 2 , 'numsubdiv' , 8 ,  'minexponent' , -20 , 'maxexponent' , 8);
param_classif{16,3}       = struct('s' , 0 , 't' , 0 , 'c' , 0.07 , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 0);

%% mlhmslsd_spy %%

param_classif{17,1}       = struct('s' , 2 , 'B' , 1 , 'c' , 0.002 , 'n' , 0 , 'L' , 1.5 , 'kerneltype' , 2);
param_classif{17,2}       = struct('lambda' , 1/(N*0.002) , 'biasmult' , 1 , 'nbite' , 15*N , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 2 , 'numsubdiv' , 8 ,  'minexponent' , -20 , 'maxexponent' , 8);
param_classif{17,3}       = struct('s' , 0 , 't' , 0 , 'c' , 0.1 , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 0);

%% mlhmslbp_spyr %% 1000 , 0.07

param_classif{18,1}       = struct('s' , 2 , 'B' , 1 , 'c' , 0.1 , 'n' , 1 , 'L' , 1.5 , 'kerneltype' , 2); %0.8456=(s=2,c=0.1) %
param_classif{18,2}       = struct('lambda' , 1/(N*0.1) , 'biasmult' , 1 , 'nbite' , 15*N , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 2 , 'numsubdiv' , 8 ,  'minexponent' , -20 , 'maxexponent' , 8);
param_classif{18,3}       = struct('s' , 0 , 't' , 0 , 'c' , 0.07 , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 0);

%% mlhmsldp_spyr %% 1000 , 0.07

param_classif{19,1}       = struct('s' , 2 , 'B' , 1 , 'c' , 0.1 , 'n' , 1 , 'L' , 1.5 , 'kerneltype' , 2 ); %0.8456=(s=2,c=0.1) %
param_classif{19,2}       = struct('lambda' , 1/(N*0.1) , 'biasmult' , 1 , 'nbite' , 15*N , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 2 , 'numsubdiv' , 8 ,  'minexponent' , -20 , 'maxexponent' , 8);
param_classif{19,3}       = struct('s' , 0 , 't' , 0 , 'c' , 0.07 , 'n' , 0 , 'L' , 0.5 , 'kerneltype' , 0);


