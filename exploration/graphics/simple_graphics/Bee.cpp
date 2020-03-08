#include <iostream>
#include <sstream>
#include <vector>
#include <opencv.hpp>
#include "Bee.hpp"

using namespace cv;
using namespace std;

Mat rotateImage(Mat src, int angle, bool crop_image){
    //code heavily inspired by https://stackoverflow.com/questions/22041699/rotate-an-image-without-cropping-in-opencv-in-c

    // get rotation Matrix for rotating the image around its center in pixel coordinates
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

Bee::Bee(vector<Mat> frames){
  this->x = 0;
  this->y = 0;
  this->angle = 0;
  this->frames = frames;
  this->curFrame = frames[0];
  this->counter = 0;
}

Bee::Bee(vector<Mat> frames, int x, int y, int angle){
  this->x = x;
  this->y = y;
  this->angle = angle;
  this->frames = frames;
  this->curFrame = frames[0];
  this->counter = 0;
}

void Bee::incrementFrame(){
  this->counter = (this->counter+1)%(this->frames.size());
  this->curFrame = this->frames.at(this->counter);
}

void Bee::rotate(int angle) {
  this->angle = (this->angle+angle)%360;
}

void Bee::setAngle(int angle){
  this->angle = (angle)%360;
}

void Bee::setPosition(int x, int y){
  this->x = x;
  this->y = y;
}

void Bee::setX(int x){
  this->x = x;
}

void Bee::setY(int y){
  this->y = y;
}

int Bee::getX(){
  return this->x;
}

int Bee::getY(){
  return this->y;
}

Mat Bee::getCurFrame(){
  return rotateImage(this->curFrame, this->angle, true);
}
