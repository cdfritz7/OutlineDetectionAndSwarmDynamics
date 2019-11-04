#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <vector>
#include <opencv.hpp>
#include "Bee.hpp"

using namespace cv;
using namespace std;

Mat rotateImage2(Mat src, int angle, bool crop_image){
    //code heavily inspired by https://stackoverflow.com/questions/22041699/rotate-an-image-without-cropping-in-opencv-in-c

    // get rotation matrix for rotating the image around its center in pixel coordinates
    Point2f center((src.cols-1)/2.0, (src.rows-1)/2.0);
    Mat rot = cv::getRotationMatrix2D(center, angle, 1.0);
    Size final_size = src.size();

    if(!crop_image){
      // determine bounding rectangle, center not relevant
      Rect2f bbox = cv::RotatedRect(cv::Point2f(), src.size(), angle).boundingRect2f();

      // adjust transformation matrix
      rot.at<double>(0,2) += bbox.width/2.0 - src.cols/2.0;
      rot.at<double>(1,2) += bbox.height/2.0 - src.rows/2.0;

      //adjust size to new bounding rectangle
      final_size = bbox.size();
    }

    Mat dst;
    warpAffine(src, dst, rot, final_size);
    return dst;
}

int main(){
  Mat frame1;
  Mat frame2;
  frame1 = imread("./bee0.png");
  frame2 = imread("./bee1.png");
  resize(frame1, frame1, Size(100, 100));
  resize(frame2, frame2, Size(100, 100));
  waitKey(1); // wait for images to load

  vector<Mat> frames;
  frames.push_back(frame1);
  frames.push_back(frame2);

  if(frame1.empty() or frame2.empty()){
    cout << "Could Not Find Images" << endl;
    return -1;
  }

  vector<Bee> bees;

  for(unsigned i = 0; i < 20; i++){
    Bee bee(frames, rand()%1000, rand()%1000, rand()%360);
    bees.push_back(bee);
  }

  String windowName = "bee";

  while(true){
    Mat background(Size(1000, 1000), CV_8UC3, Scalar(0));

    for(unsigned i = 0; i<bees.size(); i++){
      Mat curFrame = bees[i].getCurFrame();

      if(bees[i].getY()+curFrame.cols > background.cols or bees[i].getY()<0){
        bees[i].setY(0);
      }

      if(bees[i].getX()+curFrame.rows > background.rows or bees[i].getX()<0){
        bees[i].setX(0);
      }
      curFrame.copyTo(background(Rect(bees[i].getX(),bees[i].getY(),curFrame.cols, curFrame.rows)));

      bees[i].rotateClockwise(rand()%20-5);
      bees[i].incrementFrame();
      bees[i].setX(bees[i].getX()+rand()%25-12);
      bees[i].setY(bees[i].getY()+rand()%25-12);
    }

    imshow(windowName, background);

    char k = waitKey(100);

    if(k==27){ //if user hits esc
      destroyWindow(windowName);
      break;
    }

  }

  return(0);
}
