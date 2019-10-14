#include <iostream>
#include <sstream>
#include <opencv4/opencv2/opencv.hpp>

int main() {
  cv::VideoCapture cap(0);

  if(!cap.isOpened()) {
    std::cout << "Unable to Open Camera";
    std::exit(-1);
  }

  //variables for background masking
  cv::Ptr<cv::BackgroundSubtractor> pBackSub = cv::createBackgroundSubtractorMOG2();
  cv::Mat frame;
  cv::Mat mask;

  //build up our count
  std::time_t start, current;
  std::time(&start);
  std::time(&current);
  while(current-start < 10){
    cap >> frame;

    if(frame.empty()){
      std::cout << "can't read Image from camera";
      return 0;
    }

    pBackSub->apply(frame, mask);
    std::time(&current);
  }

  //params for edge detection
  std::vector<cv::Mat> prev_images;
  cv::Mat image;
  cv::Mat cannyResult;
  cv::Mat avg_result;
  int prev_count = 6; //length of averaging indow
  double FPS = 20.0;

  //read and display camera frames until q is pressed
  while(true){
    image = cv::Mat::zeros(1,1, CV_64F);

    cap >> frame;

    if(frame.empty()) {
      std::cout << "Can't Read Image From Camera";
      break;
    }

    pBackSub->apply(frame, mask, 0.0);
    frame.copyTo(image, mask);

    cv::imshow("mask", image);

    cv::Canny(image, cannyResult, 75, 175, 3);
    cv::resize(cannyResult, cannyResult, cv::Size(image.cols*3, image.rows*3));

    //insert into window
    if(prev_images.size() < prev_count){
        prev_images.insert(prev_images.begin(), cannyResult/prev_count);
    }else{
        prev_images.pop_back();
        prev_images.insert(prev_images.begin(), cannyResult/prev_count);
    }

    //find average from window
    if(prev_images.size() < prev_count){
      avg_result = cannyResult.clone();
      //avg_result = cannyResult; //for white out
    }else{
      avg_result = prev_images[0].clone();
      //avg_result = prev_images[0]; //for white out
      for(int i = 1; i < prev_images.size(); i++){
        avg_result = avg_result + prev_images[i];
      }
    }

    cv::imshow("Camera Feed", avg_result);

    //stop camera if user hits 'esc'
    if(cv::waitKey(1000.0/FPS)==27){
      break;
    }
  }

  return 0;
}
