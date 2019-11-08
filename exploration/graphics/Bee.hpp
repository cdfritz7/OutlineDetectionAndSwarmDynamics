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
    vector<Mat4b> frames;
    Mat4b curFrame;

  public:
    Bee(vector<Mat4b> frames);
    Bee(vector<Mat4b> frames, int x, int y, int angle);
    void incrementFrame();
    void rotate(int angle);
    void setAngle(int angle);
    void setPosition(int x, int y);
    void setX(int x);
    void setY(int y);
    int getX();
    int getY();
    Mat4b getCurFrame();
};
