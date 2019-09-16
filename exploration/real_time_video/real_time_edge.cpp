#include <iostream>
#include <opencv4/opencv2/opencv.hpp>

int main() {
  cv::VideoCapture cap(0);

  if(!cap.isOpened()) {
    std::cout << "Unable to Open Camera";
    std::exit(-1);
  }

  std::vector<cv::Mat> prev_images;
  cv::Mat image;
  cv::Mat cannyResult;
  cv::Mat avg_result;
  int prev_count = 10; //length of averaging indow
  double FPS = 30.0;

  //read and display camera frames until q is pressed
  while(true){
    cap >> image;

    if(image.empty()) {
      std::cout << "Can't Read Image From Camera";
      break;
    }

    cv::Canny(image, cannyResult, 50, 125, 3);
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
    }else{
      avg_result = prev_images[0].clone();
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
