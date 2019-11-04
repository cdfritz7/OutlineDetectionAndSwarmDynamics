#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <vector>
#include <opencv.hpp>
#include "Bee.hpp"

using namespace cv;
using namespace std;

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

    char k = waitKey(50);

    if(k==27){ //if user hits esc
      destroyWindow(windowName);
      break;
    }

  }

  return(0);
}
