#include <iostream>
#include <sstream>
#include <vector>
#include <opencv.hpp>

using namespace cv;
using namespace std;

class Bee{
  private:
    int x;
    int y;
    int angle;
    int frame;
    int counter;
    vector<Mat> frames;
    Mat curFrame;

  public:
    Bee(vector<Mat> frames);
    Bee(vector<Mat> frames, int x, int y, int angle);
    void incrementFrame();
    void rotateClockwise(int angle);
    void setAngle(int angle);
    void setPosition(int x, int y);
    void setX(int x);
    void setY(int y);
    int getX();
    int getY();
    Mat getCurFrame();
};
