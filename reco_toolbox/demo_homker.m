%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Compare Linear SVM, Homogeous mapping + Linear SVM and Intersection
% Kernel+Liblinear
% 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


clc,close all, clear ,drawnow

rootbase_dir         = pwd;

core_dir             = fullfile(pwd , 'core');
feat_dir             = fullfile(pwd , 'features');

%features             = 'mlhmslsd_spyr.mat';
%features             = 'denseMBLBP_dl_spyr.mat';
features             = 'mlhmslbp_spyr.mat';



addpath(core_dir)

if(exist(fullfile(feat_dir , features ) , 'file'))

load(fullfile(feat_dir , features ));

nb_topic             = length(unique(y));
nb_images_per_topic  = histc(y , 1:nb_topic);


maxperclasstrain     = 100*ones(1,nb_topic);
maxperclasstest      = nb_images_per_topic - maxperclasstrain;

Itrain               = zeros(1 , sum(maxperclasstrain));
Itest                = zeros(1 , sum(maxperclasstest));

cotrain              = 1;
cotest               = 1;

for i = 1:nb_topic
    indi                                                                     = find(y==i);
    tempind                                                                  = randperm(nb_images_per_topic(i));
    indtrain                                                                 = tempind(1:maxperclasstrain(i));
    indtest                                                                  = tempind(maxperclasstrain(i)+1:maxperclasstrain(i)+maxperclasstest(i));
    Itrain(cotrain:cotrain+maxperclasstrain(i)-1)                            = indi(indtrain);
    Itest(cotest:cotest+maxperclasstest(i)-1)                                = indi(indtest);
    cotrain                                                                  = cotrain + maxperclasstrain(i);
    cotest                                                                   = cotest  + maxperclasstest(i);
    
end

%% Train with linear kernel and liblinear 

Xtrain               = X(: , Itrain);
ytrain               = y(Itrain);                  
%model_lin            = train_dense(ytrain' , Xtrain , '-q -s 2 -B 1 -c 0.002' , 'col');
model_lin            = train_dense(ytrain' , Xtrain , '-q -s 2 -B 1 -c 0.07' , 'col');


Xtest               = X(: , Itest);
ytest               = y(Itest);

clear X y


[ytest_est ,  lin_accuracy_test  , fest]     = predict_dense(ytest' , Xtest , model_lin , '-b 0' , 'col');

fprintf('accuracy = %4.2f\n' , lin_accuracy_test)
drawnow

%% Train with intersection kernel + livsvm

%model_inter                                    = svmtrain(ytrain' , Xtrain' , '-s 0 -t 4 -c 0.1');
model_inter                                    = svmtrain(ytrain' , Xtrain' , '-s 0 -t 4 -c 0.07');

[ytest_est ,  inter_accuracy_test  , fest]     = svmpredict(ytest' , Xtest' , model_inter , '-b 0' );

fprintf('accuracy = %4.2f\n' , inter_accuracy_test(1))
drawnow


%% Train with intersection kernel approximation + liblinear

options.n            = 1;
options.L            = 1.5;
options.kerneltype   = 0;


Xtrain               = homkermap(Xtrain , options );

                   
%model_homker         = train_dense(ytrain' , Xtrain , '-q -s 2 -B 1 -c 0.002' , 'col');
model_homker         = train_dense(ytrain' , Xtrain , '-q -s 2 -B 1 -c 0.07' , 'col');

clear Xtrain ytrain

%%
Xtest               = homkermap(Xtest , options );

[homker_ytest_est ,  homker_accuracy_test  , homker_fest]     = predict_dense(ytest' , Xtest , model_homker , '-b 0' , 'col');

fprintf('accuracy with feature map = %4.2f\n' , homker_accuracy_test)
drawnow

clear Xtest ytest

else
    
    fprintf('%s is absent !!!\n\nPlease run extract_direct_features.m and extract %s descriptor first (3th descriptor in config_classifier.m)\n' , features , features)
    
end

