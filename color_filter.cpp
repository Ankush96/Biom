
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

int cr_min=126,cr_max=175,cb_min=99,cb_max=130,kernel=8;
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
    bool e1 = (R>95) && (G>40) && (B>20) && ((max(R,max(G,B)) - min(R, min(G,B)))>15) && (abs(R-G)>15) && (R>G) && (R>B);
    bool e2 = (R>220) && (G>210) && (B>170) && (abs(R-G)<=15) && (R>B) && (G>B);
    return (e1||e2);
}

bool R2(float Y, float Cr, float Cb) {
    bool e3= Cr>=cr_min;
    bool e4= Cr<=cr_max;
    bool e5= Cb>=cb_min;
    bool e6= Cb<=cb_max;
    return e3 && e4 && e5 && e6;
}

bool R3(float H, float S, float V){

    return (H<25) || (H > 230);
}

/*

    ******************************   Stage 1 of segmentation *****************************
    *   The first step converts an input image into the YCrCb colorspace.
    *   Options for converting into other color spaces are available (uncomment accordingly)
    *   If a pixel lies within the thresholds of the given colorspace, it is mapped
    *   to a white pixel, else it is mapped to a black pixel.

    *   Input  -> A color image that needs to be segmented
    *   Output -> A bitmap(3-channel) of facial region.
    **************************************************************************************

*/
Mat stage1(Mat const &src) {
    Mat dst = src.clone();
    Vec3b cwhite = Vec3b::all(255);
    Vec3b cblack = Vec3b::all(0);

    Mat src_ycrcb, src_hsv;
    // OpenCV scales the YCrCb components, so that they
    // cover the whole value range of [0,255], so there's
    // no need to scale the values:
    cvtColor(src, src_ycrcb, CV_BGR2YCrCb);
    // OpenCV scales the Hue Channel to [0,180] for
    // 8bit images, so make sure we are operating on
    // the full spectrum from [0,360] by using floating
    // point precision:

    //src.convertTo(src_hsv, CV_32FC3);
    //cvtColor(src_hsv, src_hsv, CV_BGR2HSV);

    // Now scale the values between [0,255]:
    //normalize(src_hsv, src_hsv, 0.0, 255.0, NORM_MINMAX, CV_32FC3);

    for(int i = 0; i < src.rows; i++) {
        for(int j = 0; j < src.cols; j++) {

            //  Uncomment the following section for using RGB thresholds

            // Vec3b pix_bgr = src.ptr<Vec3b>(i)[j];
            // int B = pix_bgr.val[0];
            // int G = pix_bgr.val[1];
            // int R = pix_bgr.val[2];
            // // apply rgb rule
            // bool a = R1(R,G,B);

            //----------------------------------------//

            //  The following section is for YCrCb thresholding

            Vec3b pix_ycrcb = src_ycrcb.ptr<Vec3b>(i)[j];
            int Y = pix_ycrcb.val[0];
            int Cr = pix_ycrcb.val[1];
            int Cb = pix_ycrcb.val[2];
            //  Apply ycrcb rule
            bool b = R2(Y,Cr,Cb);

            //  Uncomment the following section for using HSV thresholds

            // Vec3f pix_hsv = src_hsv.ptr<Vec3f>(i)[j];
            // float H = pix_hsv.val[0];
            // float S = pix_hsv.val[1];
            // float V = pix_hsv.val[2];
            // // apply hsv rule
            // bool c = R3(H,S,V);

            //----------------------------------------//

            if(!(b))                                    //  If values not within thresholds
                dst.ptr<Vec3b>(i)[j] = cblack;          //  Appoint black color
            else
                dst.ptr<Vec3b>(i)[j] = cwhite;          //  Else white
        }
    }
    //namedWindow("Stage1",WINDOW_NORMAL);
    //imshow("Stage1",dst);
    //imwrite("s1.jpg",dst);
    //waitKey(0);
    return dst;                                         //  Return Bitmap of facial region
}

int main()
{
    dir_read("../Data/",30);
    namedWindow("Input",WINDOW_NORMAL);
    
    for(int i=0; i<images.size(); i++)
    {
        imshow("Input",images[i]);
        waitKey(0);
    }
    return 0;
}