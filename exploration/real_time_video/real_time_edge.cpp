#include <iostream>
#include <opencv2/opencv.hpp>

int main() {
  cv::VideoCapture cap(0);

  if(!cap.isOpened()) {
    std::cout << "Unable to Open Camera";
    std::exit(-1);
  }

  cv::Mat image;
  double FPS = 24.0;
  //read and display camera frames until q is pressed
  while(true){
    cap >> image;
    if(image.empty()) {
      std::cout << "Can't Read Image From Camera";
      break;
    }

    cv::imshow("Camera Feed", image);

    //stop camera if user hits q
    if(cv::waitKey(1000.0/FPS)==27){
      break;
    }
  }

  return 0;
}
