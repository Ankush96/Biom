%%%%%%%% Classical Bag of Features with Spatial Pyramid. D learned with Kmeans %%%%

mlhbow_feat             = cell(1 , length(descriptors_name));
mlhbow_feat{1}          = struct('spyr' , [1 , 1 , 1 , 1 ; 1/3 , 1/3 , 1/6 , 1/6] , 'beta' , 1 , 'assignment' , 0 , 'norm' , [0 , 2 , 0] , 'clamp' , 0.2);
mlhbow_feat{1}.nH       = sum(floor(((1 - mlhbow_feat{1}.spyr(:,1))./(mlhbow_feat{1}.spyr(:,3)) + 1)).*floor((1 - mlhbow_feat{1}.spyr(:,2))./(mlhbow_feat{1}.spyr(:,4)) + 1));
mlhbow_feat{2}          = struct('spyr' , [1 , 1 , 1 , 1 ; 1/2 , 1/2 , 1/4 , 1/4] , 'beta' , 1 , 'assignment' , 0 , 'norm' , [0 , 2 , 0] , 'clamp' , 0.2);
mlhbow_feat{2}.nH       = sum(floor(((1 - mlhbow_feat{2}.spyr(:,1))./(mlhbow_feat{2}.spyr(:,3)) + 1)).*floor((1 - mlhbow_feat{2}.spyr(:,2))./(mlhbow_feat{2}.spyr(:,4)) + 1));
mlhbow_feat{3}          = struct('spyr' , [1 , 1 , 1 , 1 ; 1/2 , 1/2 , 1/4 , 1/4] , 'beta' , 1 , 'assignment' , 0 , 'norm' , [0 , 2 , 0] , 'clamp' , 0.2);
mlhbow_feat{3}.nH       = sum(floor(((1 - mlhbow_feat{3}.spyr(:,1))./(mlhbow_feat{3}.spyr(:,3)) + 1)).*floor((1 - mlhbow_feat{3}.spyr(:,2))./(mlhbow_feat{3}.spyr(:,4)) + 1));
mlhbow_feat{4}          = struct('spyr' , [1 , 1 , 1 , 1 ; 1/2 , 1/2 , 1/4 , 1/4] , 'beta' , 1 , 'assignment' , 0 , 'norm' , [0 , 2 , 0] , 'clamp' , 0.2);
mlhbow_feat{4}.nH       = sum(floor(((1 - mlhbow_feat{4}.spyr(:,1))./(mlhbow_feat{4}.spyr(:,3)) + 1)).*floor((1 - mlhbow_feat{4}.spyr(:,2))./(mlhbow_feat{4}.spyr(:,4)) + 1));
mlhbow_feat{5}          = struct('spyr' , [1 , 1 , 1 , 1 ; 1/2 , 1/2 , 1/4 , 1/4] , 'beta' , 1 , 'assignment' , 0 , 'norm' , [0 , 2 , 0] , 'clamp' , 0.2);
mlhbow_feat{5}.nH       = sum(floor(((1 - mlhbow_feat{5}.spyr(:,1))./(mlhbow_feat{5}.spyr(:,3)) + 1)).*floor((1 - mlhbow_feat{5}.spyr(:,2))./(mlhbow_feat{5}.spyr(:,4)) + 1));

%%%%%%%% Pooling + Spatial Pyramid. D learned with sparse learning  %%%%

dl_feat                 = cell(1 , length(descriptors_name));
dl_feat{1}              = struct('spyr' , [1 , 1 , 1 , 1 ; 1/3 , 1/3 , 1/6 , 1/6] , 'lambda' , 0.2 , 'mode' , 2 , 'pos' , 0 , 'assignment' , 0 , 'posAlpha' , 1  , 'numThreads' , '-1', 'norm' , [0 , 2 , 0] , 'clamp' , 0.2);
dl_feat{1}.nH           = sum(floor(((1 - dl_feat{1}.spyr(:,1))./(dl_feat{1}.spyr(:,3)) + 1)).*floor((1 - dl_feat{1}.spyr(:,2))./(dl_feat{1}.spyr(:,4)) + 1));
dl_feat{2}              = struct('spyr' , [1 , 1 , 1 , 1 ; 1/2 , 1/2 , 1/4 , 1/4] , 'lambda' , 0.2 , 'mode' , 2 , 'pos' , 0 , 'assignment' , 0 , 'posAlpha' , 1  , 'numThreads' , '-1', 'norm' , [0 , 2 , 0] , 'clamp' , 0.2);
dl_feat{2}.nH           = sum(floor(((1 - dl_feat{2}.spyr(:,1))./(dl_feat{2}.spyr(:,3)) + 1)).*floor((1 - dl_feat{2}.spyr(:,2))./(dl_feat{2}.spyr(:,4)) + 1));
dl_feat{3}              = struct('spyr' , [1 , 1 , 1 , 1 ; 1/2 , 1/2 , 1/4 , 1/4] , 'lambda' , 0.2 , 'mode' , 2 , 'pos' , 0 , 'assignment' , 0 , 'posAlpha' , 1  , 'numThreads' , '-1', 'norm' , [0 , 2 , 0] , 'clamp' , 0.2);
dl_feat{3}.nH           = sum(floor(((1 - dl_feat{3}.spyr(:,1))./(dl_feat{3}.spyr(:,3)) + 1)).*floor((1 - dl_feat{3}.spyr(:,2))./(dl_feat{3}.spyr(:,4)) + 1));
dl_feat{4}              = struct('spyr' , [1 , 1 , 1 , 1 ; 1/2 , 1/2 , 1/4 , 1/4] , 'lambda' , 0.2 , 'mode' , 2 , 'pos' , 0 , 'assignment' , 0 , 'posAlpha' , 1  , 'numThreads' , '-1', 'norm' , [0 , 2 , 0] , 'clamp' , 0.2);
dl_feat{4}.nH           = sum(floor(((1 - dl_feat{4}.spyr(:,1))./(dl_feat{4}.spyr(:,3)) + 1)).*floor((1 - dl_feat{4}.spyr(:,2))./(dl_feat{4}.spyr(:,4)) + 1));
dl_feat{5}              = struct('spyr' , [1 , 1 , 1 , 1 ; 1/2 , 1/2 , 1/4 , 1/4] , 'lambda' , 0.2 , 'mode' , 2 , 'pos' , 0 , 'assignment' , 0 , 'posAlpha' , 1  , 'numThreads' , '-1', 'norm' , [0 , 2 , 0] , 'clamp' , 0.2);
dl_feat{5}.nH           = sum(floor(((1 - dl_feat{5}.spyr(:,1))./(dl_feat{5}.spyr(:,3)) + 1)).*floor((1 - dl_feat{5}.spyr(:,2))./(dl_feat{5}.spyr(:,4)) + 1));


%%%%%%%% Locality-constraint Linear Coding + Spatial Pyramid. D learned with sparse learning  %%%%

mlhlcc_feat             = cell(1 , length(descriptors_name));
mlhlcc_feat{1}          = struct('spyr' , [1 , 1 , 1 , 1 ; 1/3 , 1/3 , 1/6 , 1/6] , 'lambda' , 10e-4 , 'nn' , 5  , 'assignment' , 0 , 'norm' , [0 , 0 , 4] , 'clamp' , 0.2);
mlhlcc_feat{1}.nH       = sum(floor(((1 - mlhlcc_feat{1}.spyr(:,1))./(mlhlcc_feat{1}.spyr(:,3)) + 1)).*floor((1 - mlhlcc_feat{1}.spyr(:,2))./(mlhlcc_feat{1}.spyr(:,4)) + 1));
mlhlcc_feat{2}          = struct('spyr' , [1 , 1 , 1 , 1 ; 1/2 , 1/2 , 1/4 , 1/4] , 'lambda' , 10e-4 , 'nn' , 5  , 'assignment' , 0 , 'norm' , [0 , 0 , 4] , 'clamp' , 0.2);
mlhlcc_feat{2}.nH       = sum(floor(((1 - mlhlcc_feat{2}.spyr(:,1))./(mlhlcc_feat{2}.spyr(:,3)) + 1)).*floor((1 - mlhlcc_feat{2}.spyr(:,2))./(mlhlcc_feat{2}.spyr(:,4)) + 1));
mlhlcc_feat{3}          = struct('spyr' , [1 , 1 , 1 , 1 ; 1/2 , 1/2 , 1/4 , 1/4] , 'lambda' , 10e-4 , 'nn' , 5  , 'assignment' , 0 , 'norm' , [0 , 0 , 4] , 'clamp' , 0.2);
mlhlcc_feat{3}.nH       = sum(floor(((1 - mlhlcc_feat{3}.spyr(:,1))./(mlhlcc_feat{3}.spyr(:,3)) + 1)).*floor((1 - mlhlcc_feat{3}.spyr(:,2))./(mlhlcc_feat{3}.spyr(:,4)) + 1));
mlhlcc_feat{4}          = struct('spyr' , [1 , 1 , 1 , 1 ; 1/2 , 1/2 , 1/4 , 1/4] , 'lambda' , 10e-4 , 'nn' , 5  , 'assignment' , 0 , 'norm' , [0 , 0 , 4] , 'clamp' , 0.2);
mlhlcc_feat{4}.nH       = sum(floor(((1 - mlhlcc_feat{4}.spyr(:,1))./(mlhlcc_feat{4}.spyr(:,3)) + 1)).*floor((1 - mlhlcc_feat{4}.spyr(:,2))./(mlhlcc_feat{4}.spyr(:,4)) + 1));
mlhlcc_feat{5}          = struct('spyr' , [1 , 1 , 1 , 1 ; 1/2 , 1/2 , 1/4 , 1/4] , 'lambda' , 10e-4 , 'nn' , 5  , 'assignment' , 0 , 'norm' , [0 , 0 , 4] , 'clamp' , 0.2);
mlhlcc_feat{5}.nH       = sum(floor(((1 - mlhlcc_feat{5}.spyr(:,1))./(mlhlcc_feat{5}.spyr(:,3)) + 1)).*floor((1 - mlhlcc_feat{5}.spyr(:,2))./(mlhlcc_feat{5}.spyr(:,4)) + 1));

