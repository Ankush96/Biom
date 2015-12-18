%%%%%% Kmean with YAEL %%%%%%

yael               = cell(1 , length(descriptors_name));
yael{1}            = struct('K' , 1024 , 'niter' , 200 , 'nt' , 1 , 'redo' , 1 , 'verbose' , 0 , 'nbpatchesperclass' , 20000);
yael{2}            = struct('K' , 1024 , 'niter' , 200 , 'nt' , 1 , 'redo' , 1 , 'verbose' , 0 , 'nbpatchesperclass' , 60000);
yael{3}            = struct('K' , 1024 , 'niter' , 200 , 'nt' , 1 , 'redo' , 1 , 'verbose' , 0 , 'nbpatchesperclass' , 20000);
yael{4}            = struct('K' , 1024 , 'niter' , 200 , 'nt' , 1 , 'redo' , 1 , 'verbose' , 0 , 'nbpatchesperclass' , 20000);
yael{5}            = struct('K' , 1024 , 'niter' , 200 , 'nt' , 1 , 'redo' , 1 , 'verbose' , 0 , 'nbpatchesperclass' , 20000);

%%% Sparse Dictionary Learning with SPAMS 2.0 %%%%%

spams              = cell(1 , length(descriptors_name));
spams{1}           = struct('K' , 1024 , 'fast' , 0 , 'mode' , 2 , 'posD' , 0 , 'whiten' , 0 , 'posAlpha' , 1 , 'lambda' , 0.2 , 'numThreads' , '-1', 'iter' , 200 , 'nbpatchesperclass' , 20000);
spams{2}           = struct('K' , 1024 , 'fast' , 0 , 'mode' , 2 , 'posD' , 0 , 'whiten' , 0 , 'posAlpha' , 1 , 'lambda' , 0.2 , 'numThreads' , '-1', 'iter' , 200 , 'nbpatchesperclass' , 60000);
spams{3}           = struct('K' , 1024 , 'fast' , 0 , 'mode' , 2 , 'posD' , 0 , 'whiten' , 0 , 'posAlpha' , 1 , 'lambda' , 0.2 , 'numThreads' , '-1', 'iter' , 200 , 'nbpatchesperclass' , 20000);
spams{4}           = struct('K' , 1024 , 'fast' , 0 , 'mode' , 2 , 'posD' , 0 , 'whiten' , 0 , 'posAlpha' , 1 , 'lambda' , 0.2 , 'numThreads' , '-1', 'iter' , 200 , 'nbpatchesperclass' , 20000);
spams{5}           = struct('K' , 1024 , 'fast' , 0 , 'mode' , 2 , 'posD' , 0 , 'whiten' , 0 , 'posAlpha' , 1 , 'lambda' , 0.2 , 'numThreads' , '-1', 'iter' , 200 , 'nbpatchesperclass' , 20000);
