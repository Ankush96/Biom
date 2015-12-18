Scenes/Objects Rocognition toolbox v0.12
---------------------------------------


This toolbox provides some basic tools for scenes/object recognition in vision systems.
Based on supervised classification, this toolbox offerts some state-of-art descriptors coupled with fast and efficient classifiers.

Descriptors are divided in two famillies: 

i)  "direct" features computed from images [1,2,3,4,5,19], 

ii) "dictionnary learning + spatial pooling" features computed from a collection of patches: 
    a) Bag of Features [6,7] and 
    b) Sparse Dictionary learning [8,9]. 

Large-Scale Linear SVM such Liblinear [10] or Pagasos [11] are used to train models since features are almost perfectly linearly separable. 
Non-linear Kernels extension for additive homogeneous kernels (chi2, intersection histogram, etc...) is performed through features map method [12,22].

The main objective of this toolbox is to deliver simple but efficient tools, easy to modify, mainly written in C codes with a matlab interface.  


Installation
------------

This toolbox has been tested on Windows system (32 & 64 bits) and should work also on Linux plateform without any problem (not fully tested). 
This toolbox use 4 third-party API freely available : SPAMS [13], FFTW[14], YAEL [15] and pthreads32[16] (for windows systems)

1) Please run "setup_reco_toolbox.m" from matlab command. It will download the 15-scene dataset [17] and compile mex-files 
(except for the SPAMS functions mexLasso, mexTrainDL, mexTrainDL_Memory where the sources code are not (yet) available, only binaries can be found in [13]). 

2) Compilation

  a) For Windows system, if you encounter some difficulties, please unzip in core folder mexw32.zip or mexw64.zip for win32/64 respectively.

  b) For Linux system, i) please recompile last build of FFTW statically, renamed it libfftw_glnx32.a/libfftw_glnx64.a and copy in core folder,
     ii) visit [13], download binaries associated to your system and untar mexLasso, mexTrainDL, mexTrainDL_Memory mex-files in core folder.

  c) For Mac system especially for mac64+I7, please recompile statically fftw as 
    1) ./configure CC="gcc -arch x86_64" CXX="g++ -arch x86_64"
    2) ./make 
    3) rename xxx.a lib as libfftw_glnx64.a and copy in core folder


3) Add recotool directory in the matlab path


How to use ?
------------

The first step is to extract advanced features from a image dataset before training models. By default, the 15-scenes dataset is downloaded 
during the setup and selected (choice_database = 1) in the 3 main matlab files : extract_bag_of_features.m, extract_direct_features.m and simple_train.m

For all descriptors (direct or dictionary learning+spatial pooling approaches), a common spatial pyramid definition is used to create a L=nspyr level spatial pyramid.
For a particular level of the pyramid, each subwindow size is defined by ratio of input image I size (ny x nx). The other parameters control the overlapping. 
The pyramyd is defined as below: 


spyr                    Spatial Pyramid (nspyr x 4) (default [1 , 1 , 1 , 1] with nspyr = 1)
                        where spyr(i,1) is the ratio of ny in y axis of the blocks at level i (by(i) = spyr(i,1)*ny)
                        where spyr(i,2) is the ratio of ny in y axis of the shifting at level i (deltay(i) = spyr(i,2)*ny)
                        where spyr(i,3) is the ratio of nx in x axis of the blocks at level i (bx(i) = spyr(i,3)*nx)
                        where spyr(i,3) is the ratio of nx in x axis of the shifting at level i (deltax(i) = spyr(i,4)*nx)

The total number of sub-windows for all levels of the pyramid is computed by the following formula:

nS = sum(floor(((1 - spyr(:,1))./(spyr(:,3)) + 1)).*floor((1 - spyr(:,2))./(spyr(:,4)) + 1))

For example if spyr = [1 , 1 , 1 , 1 ; 1/2 , 1/2 , 1/2 , 1/2 ; 1/4 , 1/4 , 1/4 , 1/4], 
then spyr is the classic 3 levels pyramid with 21=(1 + 4 + 16) non-overlapping sub-windows used in a lot of papers.


i)   If you plan to extract direct features, please edit extract_direct_features.m and xxxx_config_descriptors.m. With the first file, select descriptors with
choice_descriptors variable (which can be vector for extracting simultaneously several descriptors at the same time, eg. choice_descriptors = [3,4]).
With the second file, please change descriptors parameters in the "direct descriptor" section.


ii)  If you plan to extract "unsupervised trained features", please edit extract_bag_of_features.m and xxxx_config_descriptors.m. 
With the first file, a) select descriptors with choice_descriptors variable (can be vector for extracting simultaneously several descriptors at the same time, eg. choice_descriptors = [3,4]).
b) Select the encoder (unsupervised training algorithm) with the variable choice_encoding (can be vector for training simultaneously several dictionary at the same time, eg. choice_encoding = [1,2]).
Please open xxxx_config_encoding.m for changing encoder parameters.
c) Finally select the way to project descriptors with the choice_feature variable. Open xxxx_config_features.m for changing parameters especially fpr the spatial pyramid pooling definition.

With the second file, please change descriptors parameters in the "Local Patches DESCRIPTORS" section.

iii) Finally edit simple_train.m. In this file you can compare several combinaisons of descriptors and classifiers with choice_descriptors and choice_classifier variables respectively.
Training is performed by K-cross validation with 1-vs-all approach.

In the config_databases.m, you can select number of training/testing examples per class.


Tips   
----

i)  For unsupervised trained features, during the spatial pooling, normalization must be done for all windows of each pyramid level with the L2-norm
    (eg. set the variable norm = [x 2 x] in the scenes15_config_features.m)

ii) For additive homogeneous kernels approximation (chi2, intersection histogram) with feature map method [12,22], please change the "n" parameter with n>0.
    Il will increase the vector dimension by a factor (2n+1).
    As mentionned in [22], in order to obtain best results, please normalize full vector with the L1 norm (eg. set the variable norm = [1 x x]).



How to use with a new dataset?
-------------------------------

i)   You must extract the new dataset says "xxxx" in its own "xxxx" subdirectory of the 'image' folder. 
     Images of each label must be located in seperated folders.

ii)  Copy/rename the scenes15_config_classifier.m, scenes15_config_descriptors.m, scenes15_config_encoding.m and scenes15_config_features.m by 
     xxxx_config_classifier.m, xxxx_config_descriptors.m, xxxx_config_encoding.m and xxxx_config_features.m

iii) Add the new string "xxxx" in the database_name variable (and the associated file extension variable database_ext) for the 3 files:
extract_bag_of_features.m, extract_direct_features.m and simple_train.m



Important : please open *.c files to read full descriptions of each function, how to use them in Matlab, specific examples and references.
----------


P.S1 For win64 system, when yeal is linked with pthreads statically, a crash occurs if nt>1. If you want to use nt>1, link yeal dynamically by building
----   pthread32 with the following msvc command line : "nmake clean VC" (see yeal_kmeans.c for more instructions)


P.S2 At least 4Gb of RAM is recommended
----



Demos
-----

demo_denseCOLOR
demo_denseMBLBP
demo_denseMBLDP
demo_densePATCH
demo_denseSIFT
demo_mlhmslbp_spyr
demo_mlhmsldp_spyr
demo_mlhmslsd_spyr
demo_mlhoee_spyr
demo_homker


Organization
------------

\ 

config_classifier                                       Config file of classifiers'parameters
config_descriptors                                      Config file of descriptors'parameters (direct & patches)  
config_encoding                                         Config file for dictionary learning
config_features                                         Config file for spatial pyramid pooling
extract_bag_of_features                                 Script for extracting descriptors with dictionary approach from 15-scenes dataset
extract_direct_features                                 Script for extracting descriptors with direct approach from 15-scenes dataset
setup_reco_toolbox                                      Setup file to run first once
mexme_reco                                              Script for (re)compiling all mex-files
simple_train                                            Script to learn models in 1-vs-all configuration with K-cv for "15 scenes" dataset.

demo_denseCOLOR
demo_denseMBLBP
demo_denseMBLDP
demo_densePATCH
demi_denseSIFT
demo_mlhmslbp_spyr
demo_mlhmsldp_spyr
demo_mlhmslsd_spyr
demo_mlhoee_spyr
demo_homker


\core

     A) Patch functions
            
            denseCOLOR                                  Compute histogram of color projection on a regular dense grid
            denseMBLBP                                  Extract Histogram of Multi-Block LBP on a regular dense grid and computed on image I after color projection
            denseMBLDP                                  Extract Histogram of Multi-Block LDP on a regular dense grid and computed on image I after color projection
            densePATCH                                  Extract patches of pixels after color projection on a regular dense grid
            denseSIFT                                   Compute SIFT (Scale-invariant feature transform) descriptors on a regular dense grid

     B) Direct descriptors

            mlhmslbp_spyr                               Color Multi-Level Histogram of Multi-Scale Local Binary Pattern with Spatial Pyramid
            mlhmsldp_spyr                               Color Multi-Level Histogram of Multi-Scale Local Derivative Pattern with Spatial Pyramid
            mlhmslsd_spyr                               Color Multi-Level Histogram of Multi-Scale Line Segment Detector with Spatial Pyramid
            mlhoee_spyr                                 Color Multi-Level Histogram of Oriented Edge Energy with Spatial Pyramid

     C)  Dictionary learning

            yael_kmeans                                 Fast K-means algorithm to learn codebook
            mexTrainDL                                  Sparse dictionary learning algoritm
            mexTrainDL_Memory                           Faster Sparse dictionary learning algoritm but more memory consumming
            mexLasso                                    Lasso algorithm to compute alpha's weights

     D)  Spatial pyramidal pooling

            mlhbow_spyr                                 Histogram of color visual words with a Multi-Level Spatial pyramid
            dl_spyr                                     Pooling with a multi-Level spatial pyramid
            mlhlcc_spyr                                 Pooling with a multi-Level spatial pyramid and Locality-Constraint Linear Coding 

     E) Classifiers

            homker_pegasos_train                        Pegasos solver with Homeogeneous additive kernel transform included
            homker_predict                              Predict new instances with trained model
            train_dense                                 Liblinear training algorithm for dense data
            svmtrain                                    Train SVM model via Libsvm for dense data
            svmpredict                                  Predict new instances with trained model
            pegasos_train                               Pegasos solver
            predict_dense                               Liblinear predict algorithm for dense data

     F) MISCELLANEOUS 

            display_dico                                Display learned dictionary
            getmapping                                  Mapping feature's values for MBLBP approach (from Marko Heikkilä and Timo Ahonen LBP toolbox)
            imresize                                    Resize images by bilinear interpolation
            rgb2gray                                    Convert RGB image in gray format
            gen_dgauss                                  Laplacian of Gaussian
            gen_gauss                                   Gaussian Kernel
            gen_weight 
            basicroc                                    Compute ROC curves
            auroc                                       Compute Area Under ROC Curve
            homkermap                                   Homeogeneous additive kernel transform (the same as in vlfeat [18])
            confmatrix                                  Display confusion matrix with labels
            errorbarbar                                 Display errorbar and confident intervals (http://www.mathworks.com/matlabcentral/fileexchange/24718-bar-with-errorbars)
            rotateticklabel                             Rotate tick labels (http://www.mathworks.com/matlabcentral/fileexchange/8722-rotate-tick-label)


\descriptors

\dico

\features

\images


Author : Sébastien PARIS : sebastien.paris@lsis.org for contact and bugs reporting 
-------  Date : 12/04/2010


Main References  :       [1] Subhransu Maji and Alexander C. Berg and Jitendra Malik,
------------------           "Classification Using Intersection Kernel Support Vector Machines is efficient".
                             In Proceedings, CVPR 2008, Anchorage, Alaska

                         [2] Zhang, L. and Chu, R.F. and Xiang, S.M. and Liao, S.C. and Li, S.Z, 
                             "Face Detection Based on Multi-Block LBP Representation", ICB07

                         [3] Jianxin Wu and James M. Rehg, 
                             "CENTRIST: A Visual Descriptor for Scene Categorization",
		                 IEEE Transactions on Pattern Analysis and Machine Intelligence, accepted 
 
                         [4] Baochang Zhang, Yongsheng Gao, Sanqiang Zhao, and Jianzhuang Liu,
                             "Local Derivative Pattern Versus Local BinaryPattern: Face Recognition With High-Order Local Pattern Descriptor"
                              IEEE TRANSACTIONS ON IMAGE PROCESSING, VOL. 19, NO. 2, FEBRUARY 2010

                         [5] R. Grompone von Gioi, J. Jakubowicz, J.-M. Morel, G. Randall,
                             "LSD: A Fast Line Segment Detector with a False Detection Control".
                              IEEE Trans. on PAMI, 19 Dec. 2008
                         
                         [6] Lowe, David G. (1999). 
                            "Object recognition from local scale-invariant features".
                             Proceedings of the International Conference on Computer Vision. 2. pp. 1150–1157

                         [7] Koen E. A. van de Sande, Theo Gevers and Cees G. M. Snoek, 
			           "Evaluating Color Descriptors for Object and Scene Recognition", 
				     IEEE Transactions on Pattern Analysis and Machine Intelligence, volume 32 (9), pages 1582-1596, 2010

                         [8] J. Mairal, F. Bach, J. Ponce and G. Sapiro,
                             "Online Dictionary Learning for Sparse Coding". 
                             International Conference on Machine Learning, Montreal, Canada, 2009

                         [9] Jianchao Yang, Kai Yu, Yihong Gong, Thomas Huang,
                             "Linear Spatial Pyramid Matching using Sparse Coding for Image Classification",
                             CVPR'09

                         [10] C.-J. Hsieh, K.-W. Chang, C.-J. Lin, S. S. Keerthi, and S. Sundararajan
                              "A dual coordinate descent method for large-scale linear SVM", ICML 2008
                               http://www.csie.ntu.edu.tw/~cjlin/liblinear/

                         [11] Shai Shalev-Shwartz, Yoram Singer, and Nathan Srebro
                              "Pegasos: Primal Estimated sub-GrAdient SOlver for SVM"
                               ICML 2007

                         [12] A. Vedaldi and A. Zisserman, 
                              "Efficient Additive Kernels via Explicit Feature Maps",
                              in Proceedings of the IEEE Conf. on Computer Vision and Pattern Recognition (CVPR), 2010

                         [13] Julien Mairal, http://www.di.ens.fr/willow/SPAMS/

                         [14] http://www.fftw.org/

                         [15] Hervé Jegou and all, https://gforge.inria.fr/projects/yael

                         [16] http://sourceware.org/pthreads-win32/

                         [17] http://www.cs.unc.edu/~lazebnik/research/scene_categories.zip
                   
                         [18] A. Vedaldi, http://www.vlfeat.org/

                         [19] S. Paris and H. Glotin
                              "Pyramidal Multi-level Features for the Robot Vision@ICPR 2010 Challenge",
                              ICPR 2010: 2949-2952

                         [20] Shenghua Gao, Ivor W. Tsang, Liang-Tien Chia, Peilin Zhao. 
                              "Local Features Are Not Lonely - Laplacian Sparse Coding for Image Classification", 
                              (CVPR 2010), San Francisco, CA, June 2010

                         [21] Adam Coates, Honglak Lee and Andrew Y. Ng,
                              "An Analysis of Single-Layer Networks in Unsupervised Feature Learning",
                               AISTATS'2011

                         [22] A. Vedaldi and A. Zisserman,
                              "Efficient Additive Kernels via Explicit Feature Maps", 
                              in Pattern Analysis and Machine Intelligence, under revision



Greetings to :  i)   Julien Mairal for his SPAMS toolbox and his works on sparse learning,  
------------    ii)  Herve Jegou and al for his yael package
                iii) Y-Lan Boureau for her help


Changelogs :
------------
            0.12 - Add new normalization method. Now histograms can be normalized for each local subwindows and/or for all subwindows of a pyramid level 
                   and/or for the full vector. For denseXXXX patches, better results are obtained when normalizing for each pyramid level (especially for
                   denseSIFT + SPM)
                 - Better default parameters which improve results.
                 - Better databases handling.
                 - Add "triangle" soft assignement for K-means dictionary learning approach [21]

            0.11 -Add Rotation invariance for LBP code in mlhmslbp_spyr.c and denseMBLBP.c
                 -Compile SPAMS with -largeArrayDims flag for WIN64 system
                 -Can handle any many databases where images per topics are in a specific folder with specific config files
                 -Add withning preprocessing [21]
                 -Fix compilation issue for yeal and Linux system (working when linked with GOTOBlas)
  
            0.1  Initial release

To do :
-------    - Add Laplacian sparse Learning [20]

