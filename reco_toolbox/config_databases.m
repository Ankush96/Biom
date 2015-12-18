
base                            = cell(1 , length(database_name));

base{1}.maxperclasstrain        = 100*ones(1,nb_topic);
base{1}.maxperclasstest         = nb_images_per_topic - base{1}.maxperclasstrain;

base{2}.maxperclasstrain        = floor(0.7*nb_images_per_topic);
base{2}.maxperclasstest         = nb_images_per_topic - base{1}.maxperclasstrain;

base{3}.maxperclasstrain        = floor(0.7*nb_images_per_topic);
base{3}.maxperclasstest         = nb_images_per_topic - base{1}.maxperclasstrain;