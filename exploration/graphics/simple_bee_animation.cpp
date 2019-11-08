#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <vector>
#include <opencv.hpp>
#include "Bee.hpp"

using namespace cv;
using namespace std;

int main(){
  int sprite_width = 300;
  int sprite_height = 300;
  int window_width = 1500;
  int window_height = 1500;
  int num_bees = 25;
  double alpha = 1;

  Mat4b frame1;
  Mat4b frame2;
  Mat4b frame3;
  Mat4b frame4;
  Mat4b frame5;
  Mat4b frame6;
  frame1 = imread("./bee20.png", IMREAD_UNCHANGED);
  frame2 = imread("./bee21.png", IMREAD_UNCHANGED);
  frame3 = imread("./bee22.png", IMREAD_UNCHANGED);
  frame4 = imread("./bee23.png", IMREAD_UNCHANGED);
  frame5 = imread("./bee24.png", IMREAD_UNCHANGED);
  frame6 = imread("./bee25.png", IMREAD_UNCHANGED);
  resize(frame1, frame1, Size(sprite_width, sprite_height));
  resize(frame2, frame2, Size(sprite_width, sprite_height));
  resize(frame3, frame3, Size(sprite_width, sprite_height));
  resize(frame4, frame4, Size(sprite_width, sprite_height));
  resize(frame5, frame5, Size(sprite_width, sprite_height));
  resize(frame6, frame6, Size(sprite_width, sprite_height));
  waitKey(1); // wait for images to load

  vector<Mat4b> frames;
  frames.push_back(frame1);
  frames.push_back(frame2);
  frames.push_back(frame3);
  frames.push_back(frame4);
  frames.push_back(frame5);
  frames.push_back(frame6);

  if(frame1.empty() or frame2.empty()){
    cout << "Could Not Find Images" << endl;
    return -1;
  }

  vector<Bee> bees;

  for(int i = 0; i <num_bees; i++){
    Bee bee(frames, rand()%window_width, rand()%window_height, rand()%360);
    bees.push_back(bee);
  }

  String windowName = "bee";

  while(true){
    Mat background(Size(window_width, window_height), CV_8UC3, Scalar(0));

    for(unsigned i = 0; i<bees.size(); i++){
      Mat4b curFrame = bees[i].getCurFrame();

      if(bees[i].getY()+curFrame.cols > background.cols){
        bees[i].setY(background.cols-curFrame.cols-1);
      }

      if(bees[i].getY()<0){
        bees[i].setY(0);
      }

      if(bees[i].getX()+curFrame.rows > background.rows){
        bees[i].setX(background.rows-curFrame.rows-1);
      }

      if(bees[i].getX() < 0){
        bees[i].setX(0);
      }

      Mat3b roi = background(Rect(bees[i].getX(), bees[i].getY(), curFrame.cols, curFrame.rows));

      for(int r = 0; r < roi.rows; r++){
        for(int c = 0; c < roi.cols; c++){
          const Vec4b& vf = curFrame(r, c);

          if(vf[3]>0){ //alpha value greater than 0
            Vec3b& vb = roi(r, c);
            vb[0] = alpha * vf[0] + (1-alpha) * vb[0];
            vb[1] = alpha * vf[1] + (1-alpha) * vb[1];
            vb[2] = alpha * vf[2] + (1-alpha) * vb[2];
          }
        }
      }

      //curFrame.copyTo(background(Rect(bees[i].getX(), bees[i].getY(), curFrame.cols, curFrame.rows)));

      bees[i].rotate(5-rand()%11);
      bees[i].incrementFrame();
      bees[i].setX(bees[i].getX()+10-rand()%21);
      bees[i].setY(bees[i].getY()+10-rand()%21);
    }

    imshow(windowName, background);

    char k = waitKey(1);

    if(k==27){ //if user hits esc
      destroyWindow(windowName);
      break;
    }

  }

  return(0);
}
