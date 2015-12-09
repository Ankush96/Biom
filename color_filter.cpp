
#include <stdio.h>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
#include <vector>
#include <string>

using namespace cv;
using namespace std;

int cr_min=126, cr_max=175, cb_min=99, cb_max=130;
int H_thresh=17, tol=3, S_min=0, S_max=196, V_min=0, V_max=170;
int B_min=0, B_max=255, G_min=0, G_max=255, R_min=0, R_max=255;

vector<Mat> images;
vector<int> labels;

void dir_read(string root,int num,vector<Mat>& images = ::images,vector<int>& labels = ::labels)
{
    DIR *dp;
    struct dirent *dirp;
    struct stat filestat;
    int i;
    string filepath;
    for(i=1;i<=num;i++)
    {
        string dir;
        char sub[3];
        sprintf(sub,"%d",i);
        dir=root + sub + "/" ;
        dp = opendir( dir.c_str() );
        while(dirp=readdir(dp))
        {
            filepath = dir  + dirp->d_name;
            if (stat( filepath.c_str(), &filestat )) continue;
            if (S_ISDIR( filestat.st_mode )) continue;
            Mat img=imread(filepath);
            images.push_back(img);
            labels.push_back(i);
        }
        closedir(dp);
    }
}

bool R1(int R, int G, int B) {
    bool e1 = ((R >= R_min) && (G >= G_min) && (B >= B_min));
    bool e2 = ((R <= R_max) && (G <= G_max) && (B <= B_max));
    return (e1 && e2);
}

bool R2(float Y, float Cr, float Cb) {
    bool e3= Cr>=cr_min;
    bool e4= Cr<=cr_max;
    bool e5= Cb>=cb_min;
    bool e6= Cb<=cb_max;
    return e3 && e4 && e5 && e6;
}

bool R3(float H, float S, float V){
    bool e1 = ((S >= S_min) && (V >= V_min));
    bool e2 = ((S <= S_max) && (V <= V_max));
    bool e3 = ((H >= (H_thresh - tol + 180)%180) && (H <= (H_thresh + tol)%180));
    return (e1 && e2 && e3);
}
Mat segment_rgb(Mat const &src) {
    Mat dst = src.clone();
    Vec3b cwhite = Vec3b::all(255);
    Vec3b cblack = Vec3b::all(0);

    for(int i = 0; i < src.rows; i++) {
        for(int j = 0; j < src.cols; j++) {

            //  Uncomment the following section for using RGB thresholds

            Vec3b pix_bgr = src.ptr<Vec3b>(i)[j];
            int B = pix_bgr.val[0];
            int G = pix_bgr.val[1];
            int R = pix_bgr.val[2];
            // apply rgb rule
            bool a = R1(R,G,B);

            if(!(a))                                    //  If values not within thresholds
                dst.ptr<Vec3b>(i)[j] = cblack;          //  Appoint black color
            else
                dst.ptr<Vec3b>(i)[j] = cwhite;          //  Else white
        }
    }
    return dst;                                         //  Return Bitmap of facial region
}

Mat segment_hsv(Mat const &src) {
    Mat dst = src.clone();
    Vec3b cwhite = Vec3b::all(255);
    Vec3b cblack = Vec3b::all(0);

    Mat src_hsv;

    cvtColor(src, src_hsv, CV_BGR2HSV);

    for(int i = 0; i < src.rows; i++) {
        for(int j = 0; j < src.cols; j++) {

            //  Uncomment the following section for using HSV thresholds

            Vec3b pix_hsv = src_hsv.ptr<Vec3b>(i)[j];
            int H = pix_hsv.val[0];
            int S = pix_hsv.val[1];
            int V = pix_hsv.val[2];
            // apply hsv rule
            bool c = R3(H,S,V);

            //----------------------------------------//

            if(!(c))                                    //  If values not within thresholds
                dst.ptr<Vec3b>(i)[j] = cblack;          //  Appoint black color
            else
                dst.ptr<Vec3b>(i)[j] = cwhite;          //  Else white
        }
    }
    return dst;
}

Mat segment_ycrcb(Mat const &src) {
    Mat dst = src.clone();
    Vec3b cwhite = Vec3b::all(255);
    Vec3b cblack = Vec3b::all(0);

    Mat src_ycrcb, src_hsv;
    cvtColor(src, src_ycrcb, CV_BGR2YCrCb);

    for(int i = 0; i < src.rows; i++) {
        for(int j = 0; j < src.cols; j++) {

            Vec3b pix_ycrcb = src_ycrcb.ptr<Vec3b>(i)[j];
            int Y = pix_ycrcb.val[0];
            int Cr = pix_ycrcb.val[1];
            int Cb = pix_ycrcb.val[2];
            //  Apply ycrcb rule
            bool b = R2(Y,Cr,Cb);

            if(!(b))                                    
                dst.ptr<Vec3b>(i)[j] = cblack;          
            else
                dst.ptr<Vec3b>(i)[j] = cwhite;          
        }
    }
    return dst;                                         
}


int main()
{
    dir_read("../Data/",30);
    cvNamedWindow("src",WINDOW_NORMAL);
    cvNamedWindow("rgb",WINDOW_NORMAL);
    cvNamedWindow("hsv",WINDOW_NORMAL);
    cvNamedWindow("ycrcb",WINDOW_NORMAL);

    createTrackbar("cr min ","ycrcb",&cr_min,255);
    createTrackbar("cr max ","ycrcb",&cr_max,255);
    createTrackbar("cb min ","ycrcb",&cb_min,255);
    createTrackbar("cb max ","ycrcb",&cb_max,255);


    createTrackbar("r min ","rgb",&R_min,255);
    createTrackbar("r max ","rgb",&R_max,255);
    createTrackbar("g min ","rgb",&G_min,255);
    createTrackbar("g max ","rgb",&G_max,255);
    createTrackbar("b min ","rgb",&B_min,255);
    createTrackbar("b max ","rgb",&B_max,255);


    createTrackbar("h threshold ","hsv",&H_thresh,180);
    createTrackbar("h tolerance ","hsv",&tol,90);
    createTrackbar("s min ","hsv",&S_min,255);
    createTrackbar("s max ","hsv",&S_max,255);
    createTrackbar("v min ","hsv",&V_min,255);
    createTrackbar("v max ","hsv",&V_max,255);

    
    for(int i=0; i<images.size(); i++)
    {
        int key;
        Mat src = images[i];
        imshow("src",src);
        do{
            imshow("rgb", segment_rgb(src));
            imshow("hsv", segment_hsv(src));
            imshow("ycrcb", segment_ycrcb(src));

            key = cv::waitKey(30);
        }while(key<=0);
    }
    return 0;
}