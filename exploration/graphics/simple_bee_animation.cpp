#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <vector>
#include <opencv.hpp>
#include "Bee.cpp"

using namespace cv;
using namespace std;

/**
 *Draws a transparent image over a frame Mat.
 *
 * @param frame the frame where the transparent image will be drawn
 * @param transp the Mat image with transparency, read from a PNG image, with the IMREAD_UNCHANGED flag
 * @param xPos x position of the frame image where the image will start.
 * @param yPos y position of the frame image where the image will start
 https://stackoverflow.com/questions/40895785/using-opencv-to-overlay-transparent-image-onto-another-image
 */
void drawTransparency(Mat frame, Mat transp, int xPos, int yPos) {
    Mat mask;
    vector<Mat> layers;

    split(transp, layers); // seperate channels
    Mat rgb[3] = {layers[0],layers[1],layers[2]};
    mask = layers[3]; // png's alpha channel used as mask
    merge(rgb, 3, transp);  // put together the RGB channels, now transp insn't transparent
    transp.copyTo(frame.rowRange(yPos, yPos + transp.rows).colRange(xPos, xPos + transp.cols), mask);
}

int main(int argc, char* argv[]){

  int sprite_width = 25;
  int sprite_height = 25;
  int window_width = 1500;
  int window_height = 1500;
  int num_bees = 2000;

  VideoWriter oVideoWriter("./out_bees.mp4", VideoWriter::fourcc('M', 'J', 'P', 'G'),
                           20, Size(window_width, window_height), true);

  Mat frame1;
  Mat frame2;
  Mat frame3;
  Mat frame4;
  Mat frame5;
  Mat frame6;
  frame1 = imread("./b0.png", IMREAD_UNCHANGED);
  frame2 = imread("./b1.png", IMREAD_UNCHANGED);
  frame3 = imread("./b2.png", IMREAD_UNCHANGED);
  frame4 = imread("./b3.png", IMREAD_UNCHANGED);
  frame5 = imread("./b4.png", IMREAD_UNCHANGED);
  frame6 = imread("./b5.png", IMREAD_UNCHANGED);
  resize(frame1, frame1, Size(sprite_width, sprite_height));
  resize(frame2, frame2, Size(sprite_width, sprite_height));
  resize(frame3, frame3, Size(sprite_width, sprite_height));
  resize(frame4, frame4, Size(sprite_width, sprite_height));
  resize(frame5, frame5, Size(sprite_width, sprite_height));
  resize(frame6, frame6, Size(sprite_width, sprite_height));

  vector<Mat> frames;
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
      Mat curFrame = bees[i].getCurFrame();

      if(bees[i].getX()+curFrame.cols > background.cols){
        bees[i].setX(0);
      }

      if(bees[i].getY()<0){
        bees[i].setY(background.rows-curFrame.rows-1);
      }

      if(bees[i].getY()+curFrame.rows > background.rows){
        bees[i].setY(0);
      }

      if(bees[i].getX() < 0){

        bees[i].setX(background.cols-curFrame.cols-1);
      }

      drawTransparency(background, curFrame, bees[i].getX(), bees[i].getY());

      bees[i].rotate(5-rand()%11);
      bees[i].incrementFrame();
      bees[i].setX(bees[i].getX()+20-rand()%41);
      bees[i].setY(bees[i].getY()+20-rand()%41);
    }

    imshow(windowName, background);

    char k = waitKey(1);
    //oVideoWriter.write(background);

    if(k==27){ //if user hits esc
      destroyWindow(windowName);
      break;
    }

  }

  return(0);
}
