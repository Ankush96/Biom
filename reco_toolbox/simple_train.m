%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Script to learn/test models dataset
% with 1-vs-all approach for 19 high dimensionnal descriptors.
% Models are learnt with Fast-Large-Scale Linear SVM such
% a dense version of liblinear or pegasos.
% Classic SMO-SVM solver Libsvm is also included
%
% i)   edit "xxxx_config_classifier.m" to modify classifier'parameters
%
% ii)  select features to train simultaneously with the "xxxx_choice_descriptors"
%      vector. Actually 19 different types of descriptor can be trained
%
% iii) select classifier types with the "choice_classifier" vector
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%%


clc,close all, clear ,drawnow

database_name               = {'scenes15' , 'skinan' , 'agemen'};
database_ext                = {'jpg' , 'jpg' , 'png'};

descriptors_name            = {'denseSIFT_mlhbow_spyr' , 'denseSIFT_dl_spyr' , 'denseSIFT_mlhlcc_spyr' ,....
                               'denseCOLOR_mlhbow_spyr' , 'denseCOLOR_dl_spyr' , 'denseCOLOR_mlhlcc_spyr' , ...
                               'densePATCH_mlhbow_spyr' , 'densePATCH_dl_spyr' , 'densePATCH_mlhlcc_spyr' , ...
                               'denseMBLBP_mlhbow_spyr' , 'denseMBLBP_dl_spyr' , 'denseMBLBP_mlhlcc_spyr' , ...
                               'denseMBLDP_mlhbow_spyr' , 'denseMBLDP_dl_spyr' , 'denseMBLDP_mlhlcc_spyr' , ...
                               'mlhoee_spyr' , 'mlhmslsd_spyr' , 'mlhmslbp_spyr' , 'mlhmsldp_spyr'};

classifier                  = {'liblinear' , 'pegasos' , 'libsvm'};

choice_database             = [1]; %scenes15=1/skinan=2/agemen=3
choice_descriptors          = [8]; %11,18
choice_classifier           = [1]; %Libnear=1/Pegasos=2/Libsvm=3


data_name                   = database_name{choice_database(1)};
im_ext                      = database_ext{choice_database(1)};

rootbase_dir                = pwd;
images_dir                  = fullfile(pwd , 'images' , data_name);
core_dir                    = fullfile(pwd , 'core');
feat_dir                    = fullfile(pwd , 'features');
models_dir                  = fullfile(pwd , 'models');
addpath(core_dir)
dir_image                   = dir(images_dir);
nb_topic                    = length(dir_image) - 2;
classe_name                 = cellstr(char(dir_image(3:nb_topic+2).name))';


%% Training Parameters %%

K                          = 1;   % nb of K-cross-validation
seed_value                 = 5489;
post_norm                  = 0;

do_weightinglearning       = [1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1];
uselogic                   = 1;
fusion_method              = 1;   %max=0/mean=1
nbin                       = 100; % for ROC curves

%%

nb_images_per_topic        = zeros(1 , nb_topic);
y                          = [];
for i = 1:nb_topic
    nb_images_per_topic(i) = length(dir(fullfile(pwd , 'images' , data_name , dir_image(i+2).name , ['*.' , im_ext])));
    y                      = [y , i*ones(1 , nb_images_per_topic(i))];
end
N                          = sum(nb_images_per_topic);

%%

config_databases;
eval([data_name , '_config_classifier']);

%%

s                         = RandStream.create('mt19937ar','seed',seed_value);
RandStream.setDefaultStream(s);



Itrain                    = zeros(K , sum(base{choice_database}.maxperclasstrain));
Itest                     = zeros(K , sum(base{choice_database}.maxperclasstest));

for j = 1:K
    cotrain = 1;
    cotest  = 1;
    for i = 1:nb_topic
        indi                                                                     = find(y==i);
        tempind                                                                  = randperm(nb_images_per_topic(i));
        indtrain                                                                 = tempind(1:base{choice_database}.maxperclasstrain(i));
        indtest                                                                  = tempind(base{choice_database}.maxperclasstrain(i)+1:base{choice_database}.maxperclasstrain(i)+base{choice_database}.maxperclasstest(i));
        Itrain(j , cotrain:cotrain+base{choice_database}.maxperclasstrain(i)-1)  = indi(indtrain);
        Itest(j  , cotest:cotest+base{choice_database}.maxperclasstest(i)-1)     = indi(indtest);
        cotrain                                                                  = cotrain + base{choice_database}.maxperclasstrain(i);
        cotest                                                                   = cotest  + base{choice_database}.maxperclasstest(i);
    end
end

prior              = nb_images_per_topic'/length(y);
nb_descriptors     = length(choice_descriptors);
nb_classifier      = length(choice_classifier);

%%

Ntest              = size(Itest , 2);

proba              = zeros(nb_topic  , Ntest ,  K , nb_classifier , nb_descriptors );
auc                = zeros(nb_topic , K , nb_classifier ,  nb_descriptors );
tp                 = zeros(nb_topic , nbin , K , nb_classifier ,  nb_descriptors );
fp                 = zeros(nb_topic , nbin , K , nb_classifier ,  nb_descriptors );
perf               = zeros(K , nb_classifier ,  nb_descriptors );
perf_topic         = zeros(nb_topic , K , nb_classifier ,  nb_descriptors );
reco_topic         = zeros(nb_topic , K , nb_classifier ,  nb_descriptors );
matconf            = zeros(nb_topic , nb_topic , K , nb_classifier ,  nb_descriptors );
probaR             = zeros(nb_topic , nb_topic , nb_classifier ,  nb_descriptors );

model              = cell(1 , nb_topic);
RR                 = zeros( K  , nb_classifier ,  nb_descriptors );

for d = 1 : nb_descriptors
    
    cdescriptors    = choice_descriptors(d);
    base_descriptor = descriptors_name{cdescriptors};
    
    for c = 1:nb_classifier
        
        ccurrent        = choice_classifier(c);
        base_classifier = classifier{ccurrent};
        base_name       = [data_name , '_' , base_descriptor];
        base_name_model = [data_name , '_' ,base_descriptor , '_' , base_classifier];
        
        fprintf('\nLoad descriptor %s for classifier = %s\n\n' , base_name , base_classifier );
        
        drawnow
        clear X y
        
        load(fullfile(feat_dir , base_name ));
 
        if(post_norm == 1)          
            sumX = sum(X , 1) + 10e-8;
            X    = X./sumX(ones(size(X , 1) , 1) , :);           
        end
        if(post_norm == 2)          
            sumX = sum(X , 1);
            temp = sqrt(sumX.*sumX + 10e-8);
            X    = X./temp(ones(size(X , 1) , 1) , :);           
        end
        
        if(param_classif{cdescriptors,ccurrent}.n > 0)
            fprintf('Homoegeous Feature Kernel Map with n = %d, L = %4.2f, kernel = %d\n\n', param_classif{cdescriptors,ccurrent}.n , param_classif{cdescriptors,ccurrent}.L , param_classif{cdescriptors,ccurrent}.kerneltype);
            drawnow
            X           = homkermap(X , param_classif{cdescriptors,ccurrent});
        end
        
        for k = 1:K
            
            Xtrain                                     = X(: , Itrain(k , :));
            ytrain                                     = y(Itrain(k , :));
            fprintf('\nLearn train data for classifier = %s and descriptor = %s\n\n' ,  base_classifier , base_name);
            drawnow        
            for t = 1:nb_topic     
                ind_topic                              = (ytrain==t);
                ytopic                                 = double(ind_topic);
                ytopic(ytopic==0)                      = -1;
                
                if((strcmp(base_classifier , 'liblinear'))  )
                    
                    fprintf('cv = %d/%d, learn topic = %s (%d/%d), h1 = %10.5f for classifier = %s and descriptor = %s \n' , k , K , classe_name{t} , t , nb_topic  , param_classif{cdescriptors,ccurrent}.c , base_classifier , base_descriptor)
                    drawnow
                    
                    if(do_weightinglearning(c))
                        npos                                    = sum(ytopic==1);
                        nneg                                    = length(ytopic) - npos;
                        wpos                                    = nneg/npos;
                        options                                 = ['-q -s ' num2str(param_classif{cdescriptors,ccurrent}.s) ' -B ' num2str(param_classif{cdescriptors,ccurrent}.B) ' -w1 ' num2str(wpos)  ' -c ' num2str(param_classif{cdescriptors,ccurrent}.c)];   
                    else
                        options                                 = ['-q -s ' num2str(param_classif{cdescriptors,ccurrent}.s) ' -B ' num2str(param_classif{cdescriptors,ccurrent}.B)   ' -c ' num2str(param_classif{cdescriptors,ccurrent}.c)];
                    end
                    
                    model{t}                                    = train_dense(ytopic' , Xtrain , options , 'col');
                    [ytopic_est ,  accuracy_test  , ftopic]     = predict_dense(ytopic' , Xtrain , model{t} , '-b 0' , 'col'); % test the training data
                    
                    if(uselogic)
                        options                                 = ['-q -s 0  -B ' num2str(param_classif{cdescriptors,ccurrent}.B)  ' -c ' num2str(param_classif{cdescriptors,ccurrent}.c)];
                        model{t}.logist                         = train_dense(ytopic' , ftopic' , options , 'col');
                    else
                        [A , B]                                 = sigmoid_train(ytopic , ftopic');
                        ptopic                                  = sigmoid_predict(ftopic' , A , B);
                        
                        model{t}.A                              = A;
                        model{t}.B                              = B;
                    end        
                end
                
                if ((strcmp(base_classifier , 'pegasos'))  )
                    
                    fprintf('cv = %d, learn topic = %s (%d/%d), h1 = %10.5f for classifier = %s and descriptor = %s \n' , k , classe_name{t} , t , nb_topic  , param_classif{cdescriptors,ccurrent}.lambda , base_classifier , base_descriptor)
                    drawnow
                    
                    model{t}.w                                    = homker_pegasos_train(Xtrain , ytopic , param_classif{cdescriptors,ccurrent});
                    ftopic                                        = homker_predict( Xtrain , model{t}.w , param_classif{cdescriptors,ccurrent}); % test the training data
                    
                    if(uselogic)
                        options                                       = ['-q -s 0  -B ' num2str(param_classif{cdescriptors,ccurrent}.biasmult)  ' -c ' num2str(1/(param_classif{cdescriptors,ccurrent}.lambda*N))];
                        model{t}.logist                               = train_dense(ytopic' , ftopic , options , 'col');
                    else        
                        [A , B]                                       = sigmoid_train(ytopic , ftopic);
                        ptopic                                        = sigmoid_predict(ftopic' , A , B);
                        model{t}.A                                    = A;
                        model{t}.B                                    = B;
                    end
                end
                
                if((strcmp(base_classifier , 'libsvm'))  )
                    
                    fprintf('cv = %d, learn topic = %s (%d/%d), h1 = %10.5f for classifier = %s and descriptor = %s \n' , k , classe_name{t} , t , nb_topic  , param_classif{cdescriptors,ccurrent}.c , base_classifier , base_descriptor)
                    drawnow
                    
                    if(do_weightinglearning(c))
                        
                        npos                                    = sum(ytopic==1);
                        nneg                                    = length(ytopic) - npos;
                        wpos                                    = nneg/npos;
                        options                                 = ['-s ' num2str(param_classif{cdescriptors,ccurrent}.s) ' -t ' num2str(param_classif{cdescriptors,ccurrent}.t) ' -w1 ' num2str(wpos)  ' -c ' num2str(param_classif{cdescriptors,ccurrent}.c)];
                    else          
                        options                                 = ['-s ' num2str(param_classif{cdescriptors,ccurrent}.s) ' -t ' num2str(param_classif{cdescriptors,ccurrent}.t)   ' -c ' num2str(param_classif{cdescriptors,ccurrent}.c)];
                    end
                    
                    model{t}                                    = svmtrain(ytopic' , Xtrain' , options );
                    [ytopic_est ,  accuracy_test  , ftopic]     = svmpredict(ytopic' , Xtrain' , model{t}); % test the training data
                    
                    if(uselogic)  
                        options                                 = ['-q -s 0   -c ' num2str(param_classif{cdescriptors,ccurrent}.c)];
                        model{t}.logist                         = train_dense(ytopic' , ftopic , options);
                    else
                        [A , B]                                 = sigmoid_train(ytopic , ftopic);
                        ptopic                                  = sigmoid_predict(ftopic , A , B);
                        
                        model{t}.A                              = A;
                        model{t}.B                              = B;
                    end 
                end       
            end
            
            clear Xtrain ytrain;
            Xtest                                      = X(: , Itest(k , :));
            ytest                                      = y(Itest(k , :));
            
            fprintf('\nPredict test data for classifier = %s and descriptor = %s\n\n' ,  base_classifier , base_name);
            drawnow
                   
            for t = 1:nb_topic
                ind_topic                              = (ytest==t);
                ytopic                                 = double(ind_topic);
                ytopic(ytopic==0)                      = -1;
                
                fprintf('cv = %d, predict topic = %s (%d/%d) for classifier = %s and descriptor = %s\n' , k , classe_name{t} , t , nb_topic ,  base_classifier , base_descriptor);
                drawnow
                
                if((strcmp(base_classifier , 'liblinear'))  )
                    [ytopic_est ,  accuracy_test , ftopic ]       = predict_dense(ytopic' , Xtest , model{t} , '-b 0' , 'col'); % test the training data
                    if(uselogic)     
                        [l2,a2,d2]                                = predict_dense(ytopic' , ftopic , model{t}.logist , '-b 1');
                        ptopic                                    = d2(:,find(model{t}.logist.Label==1))';
                    else
                        ptopic                                    = sigmoid_predict(ftopic' , model{t}.A , model{t}.B);
                    end    
                end
                         
                if ((strcmp(base_classifier , 'pegasos'))  )      
                    ftopic                                        = homker_predict( Xtest , model{t}.w , param_classif{cdescriptors,ccurrent}); % test the training data
                    if(uselogic)           
                        [l2,a2,d2]                                = predict_dense(ytopic' , ftopic' , model{t}.logist , '-b 1');
                        ptopic                                    = d2(:,find(model{t}.logist.Label==1))';
                        ytopic_est                                = double(ptopic > 0.5)';
                        ytopic_est(ytopic_est==0)                 = -1;
                        
                    else            
                        ptopic                                    = sigmoid_predict(ftopic , model{t}.A , model{t}.B);
                        ytopic_est                                = double(ptopic > 0.5)';
                        ytopic_est(ytopic_est==0)                 = -1;             
                    end
                end
                
                if((strcmp(base_classifier , 'libsvm'))  )          
                    [ytopic_est ,  accuracy_test , ftopic ]       = svmpredict(ytopic' , Xtest' , model{t}); % test the training data
                    
                    if(uselogic)
                        [l2,a2,d2]                                = predict_dense(ytopic' , ftopic , model{t}.logist , '-b 1');
                        ptopic                                    = d2(:,find(model{t}.logist.Label==1))';         
                    else
                        ptopic                                    = sigmoid_predict(ftopic' , model{t}.A , model{t}.B);
                    end
                end
                     
                [rec_topic , mat_conf ]                   = perf_classif(ytopic , ytopic_est');
                proba(t , : , k,c,d)                      = ptopic;
                reco_topic(t , k , c , d)                 = rec_topic;
                
                [tp(t,:,k,c,d) , fp(t,:,k,c,d)]           = basicroc(ytopic , ptopic );
                auc(t,k,c,d)                              = auroc(tp(t,:,k,c,d)', fp(t,:,k,c,d)');
                
            end
            
            [maxi , ytest_est]                         = max(proba(: , : , k , : , :) , [], 1);
            RR(k,c,d)                                  = sum(ytest == ytest_est(:,:,:,c,d))/Ntest;
            [val , matconf(: , : , k , c , d)]         = perf_classif(ytest , ytest_est(:,:,:,c,d));
            
            for t = 1:nb_topic
                ind                                    = find(ytest == t);
                perf_topic(t , k , c , d)              = sum(ytest_est(:,ind,:,c,d)==t)/length(ind);
            end
            
            clear Xtest ytest;
            
            fprintf('\nFor cv = %d, Test perf = %5.3f\n' ,  k , RR(k,c,d));
            drawnow
            
        end
    end
end

mean_reco_multi   = squeeze(mean(RR , 1));
std_reco_multi    = squeeze(std(RR , 0 , 1));

temp_1vsall       = mean(perf_topic, 1);
mean_reco_1vsall  = squeeze(mean(temp_1vsall , 2));
std_reco_1vsall   = squeeze(std(temp_1vsall  , 0 , 2));

%temp_1vsall       = sum(reco_topic.*prior(: , ones(1 , K) , ones(1,nb_classifier) , ones(1,nb_descriptors )) , 1);
% temp_1vsall       = mean(reco_topic, 1);
% mean_reco_1vsall  = squeeze(mean(temp_1vsall ,2));
% std_reco_1vsall   = squeeze(std(temp_1vsall  , 0 , 2));

fprintf((repmat('\nReco multi = %4.3f +/- %4.3f\n', 1 , length(mean_reco_multi) )) , [mean_reco_multi , std_reco_multi]')
fprintf((repmat('\nReco 1vsall = %4.3f +/- %4.3f\n', 1 , length(mean_reco_multi) )) , [mean_reco_1vsall , std_reco_1vsall]');

%disp([mean_reco_multi , std_reco_multi])


tpmean                    = reshape(mean(mean(tp , 1) , 3) , [nbin , nb_classifier*nb_descriptors]);
fpmean                    = reshape(mean(mean(fp , 1) , 3) , [nbin , nb_classifier*nb_descriptors]);
auc_mean                  = squeeze(mean(mean(auc , 1) , 2));

if(fusion_method == 0)
    fusion                 = prod(prod(proba , 5) , 4);
else
    fusion                 = mean(mean(proba , 5) , 4);
end

[maxifusion , yfusion_est] = max(fusion , [] , 1);
reco_fusion_multi          = mean(sum(permute(yfusion_est , [3 , 2 , 1]) == y(Itest) , 2)/Ntest);
fprintf('\nReco fusion multi = %4.3f\n' , reco_fusion_multi)

temp_fusion                = zeros(nb_topic , K);
temp_fusion2               = zeros(nb_topic , K);


for k = 1:K
    ytest                                  = y(Itest(k , :));
    for t = 1:nb_topic
        
        ind_topic                              = (ytest==t);
        ytopic                                 = double(ind_topic);
        ytopic(ytopic==0)                      = -1;
        
        ytopic_est                             = double(fusion(t , : , k) > 0.5);
        ytopic_est(ytopic_est==0)              = -1;
        
        temp_fusion(t , k)                     = sum(ytopic_est == ytopic)/length(ytopic);
        
        [tpfusion(t,:,k) , fpfusion(t,:,k)]    = basicroc(ytopic , fusion(t , : , k) );
        aucfusion(t,k)                         = auroc(tpfusion(t,:,k)', fpfusion(t,:,k)');
        
        ind                                    = find(ytest==t);
        temp_fusion2(t,k)                      = sum(yfusion_est(1,ind,k) == ytest(ind))/length(ytest(ind));
           
    end
end

tpfusion_mean                                  = squeeze(mean(mean(tpfusion , 3) , 1));
fpfusion_mean                                  = squeeze(mean(mean(fpfusion , 3) , 1));
aucfusion_mean                                 = mean(mean(aucfusion , 2) , 1);


perf_topic_fusion_mean                         = mean(temp_fusion2 , 2);
perf_topic_fusion_std                          = std(temp_fusion2 , 0 ,  2);


reco_fusion_1vsall                             = mean(mean(temp_fusion));

fprintf('\nReco fusion = %4.3f\n' , reco_fusion_1vsall)


%% Display %%

[indd ,indc] = meshgrid(choice_descriptors , choice_classifier );
toto         = cell(nb_classifier*nb_descriptors,1);
for i = 1:nb_classifier*nb_descriptors
    toto{i}  = [classifier{indc(i)} , '  ' , descriptors_name{indd(i)} , ', AUC= ' num2str(auc_mean(i))];
end


figure(1)

if(nb_descriptors*nb_classifier == 1)
    
    plot(fpmean , tpmean  , 'linewidth' , 2);
    
    grid on
    legend(toto , 'location' , 'SouthEast');
    axis([-0.02 , 1.02 , -.02 , 1.02])
    
else
    
    plot(fpmean , tpmean , fpfusion_mean , tpfusion_mean  , 'linewidth' , 2);
    
    toto{nb_classifier*nb_descriptors+1} = ['Fusion, AUC = ' , num2str(aucfusion_mean)];
    grid on
    legend(toto , 'location' , 'SouthEast');
    axis([-0.02 , 1.02 , -.02 , 1.02])
    
    
end

%axis([-0.02 , 0.22 , 0.7 , 1.02])

perf_topic_mean = reshape(mean(perf_topic , 2) , nb_topic , nb_classifier*nb_descriptors);
perf_topic_std  = reshape(std(perf_topic , 0 , 2) , nb_topic , nb_classifier*nb_descriptors);




figure(2)

errorbarbar([perf_topic_mean , perf_topic_fusion_mean] , [perf_topic_std , perf_topic_fusion_std] );
h = legend(toto{1:end-0} , 'location' , 'NorthEast');
axis([0.5 nb_topic+0.5 -0.02 , 1.2])
set(h , 'fontsize' , 9);

set(gca,'XTick' ,1:nb_topic);
set(gca,'XTickLabel',classe_name);
aa=get(gca,'XTickLabel');
bb=get(gca,'XTick');
cc=get(gca,'YTick');
th=text(bb,repmat(cc(1)-.3*(cc(2)-cc(1)),length(bb),1),aa,'HorizontalAlignment','left','rotation',310);
set(th , 'fontsize' , 7)
set(gca,'XTickLabel',{});


h = title('P_{d}' );
set(h,  'fontsize' , 13)




figure(3)
barh([prior , max(reshape(perf_topic_mean , nb_topic , nb_descriptors*nb_classifier) , [] , 2)])
h = legend('A priori' , 'max' , 'location' , 'NorthEast');
set(h , 'fontsize' , 9)
set(gca,'YTick' ,1:nb_topic);
set(gca , 'Yticklabel' , classe_name , 'fontsize' , 11)
h = title('P_{d}');
set(h , 'fontsize' , 13)
axis ij


efficiency = perf_topic_mean./ squeeze(prior(:  , ones(1,nb_classifier) , ones(1,nb_descriptors )));

figure(4)
plot(1:nb_topic , efficiency , 'linewidth' , 2)
set(gca,'XTick' ,1:nb_topic);
set(gca,'XTickLabel',classe_name);
aa=get(gca,'XTickLabel');
bb=get(gca,'XTick');
cc=get(gca,'YTick');
th=text(bb,repmat(0-.3*(cc(2)-cc(1)),length(bb),1),aa,'HorizontalAlignment','left','rotation',310);
set(th , 'fontsize' , 7)
set(gca,'XTickLabel',{});
h = title('Efficiency' );
set(h,  'fontsize' , 13)
axis([0.5 , nb_topic+0.5 , 0.95 , 1.1*max(efficiency(:))])
h = legend(toto{1:end-1} , 'location' , 'SouthEast');




off = 4;
co  = 1;

for d = 1:nb_descriptors
    for c = 1:nb_classifier  
        figure(co + off)
        sump                  = sum(matconf(: , : ,  : , c , d) , 1);
        sump(sump==0)         = 1;
        probaR(: , : , c , d) = mean(matconf(: , : , : , c , d)./sump(ones(nb_topic , 1) , : , :) , 3);
        
        confmatrix(probaR(: , : , c , d) , classe_name);
        hold on
        h = title(toto{co});
        set(h , 'fontsize' , 13)
        hold off
        co = co + 1;    
    end
end

probaR_mean = mean(mean(probaR,4),3);
%toto{co}    =   ['Fusion, AUC= ' num2str(auc_mean(i))];


figure(co + off)
confmatrix(probaR_mean , classe_name);
% hold on
% h = title(toto{co});
% set(h , 'fontsize' , 13)
% hold off

%%
