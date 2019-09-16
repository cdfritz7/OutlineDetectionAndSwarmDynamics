#include <iostream>
#include <opencv4/opencv2/opencv.hpp>

int main() {
  cv::VideoCapture cap(0);

  if(!cap.isOpened()) {
    std::cout << "Unable to Open Camera";
    std::exit(-1);
  }

  cv::Mat image;
  double FPS = 60.0;
  //read and display camera frames until q is pressed
  while(true){
    cap >> image;

    if(image.empty()) {
      std::cout << "Can't Read Image From Camera";
      break;
    }

    cv::Mat cannyResult;
    cv::Canny(image, cannyResult, 50, 120, 3);
    cv::resize(cannyResult, cannyResult, cv::Size(image.cols*3, image.rows*3));

    cv::imshow("Camera Feed", cannyResult);

    //stop camera if user hits 'esc'
    if(cv::waitKey(1000.0/FPS)==27){
      break;
    }
  }

  return 0;
}
