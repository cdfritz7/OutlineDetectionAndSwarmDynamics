#include "libfreenect.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <pthread.h>
#include <opencv.hpp>
#include <fstream>
#include "BeeHandle.hpp"

using namespace cv;
using namespace std;


class myMutex {
	public:
		myMutex() {
			pthread_mutex_init( &m_mutex, NULL );
		}
		void lock() {
			pthread_mutex_lock( &m_mutex );
		}
		void unlock() {
			pthread_mutex_unlock( &m_mutex );
		}
	private:
		pthread_mutex_t m_mutex;
};


class MyFreenectDevice : public Freenect::FreenectDevice {
	public:
			//setting paramater of m_buffer_depth here is meaningless as
			//it is reset in the FreeNect device constructor in libfreenect.hpp
		MyFreenectDevice(freenect_context *_ctx, int _index)
	 		: Freenect::FreenectDevice(_ctx, _index), m_buffer_depth(FREENECT_DEPTH_MM),
			m_buffer_rgb(FREENECT_VIDEO_RGB), m_gamma(2048), m_new_rgb_frame(false),
			m_new_depth_frame(false), depthMat(Size(640,480),CV_16UC1),
			rgbMat(Size(640,480), CV_8UC3, Scalar(0)),
			ownMat(Size(640,480), CV_8UC3, Scalar(0)) {

			for( unsigned int i = 0 ; i < 2048 ; i++) {
				float v = i/2048.0;
				v = std::pow(v, 3)* 6;
				m_gamma[i] = v*6*256;
			}
		}

		// Do not call directly even in child
		void VideoCallback(void* _rgb, uint32_t timestamp) {
			m_rgb_mutex.lock();
			uint8_t* rgb = static_cast<uint8_t*>(_rgb);
			rgbMat.data = rgb;
			m_new_rgb_frame = true;
			m_rgb_mutex.unlock();
		};

		// Do not call directly even in child
		void DepthCallback(void* _depth, uint32_t timestamp) {
			m_depth_mutex.lock();
			uint16_t* depth = static_cast<uint16_t*>(_depth);
			depthMat.data = (uchar*) depth;
			m_new_depth_frame = true;
			m_depth_mutex.unlock();
		}

		bool getVideo(Mat& output) {
			m_rgb_mutex.lock();
			if(m_new_rgb_frame) {
				cv::cvtColor(rgbMat, output, cv::COLOR_RGB2BGR);
				m_new_rgb_frame = false;
				m_rgb_mutex.unlock();
				return true;
			} else {
				m_rgb_mutex.unlock();
				return false;
			}
		}

		bool getDepth(Mat& output) {
				m_depth_mutex.lock();
				if(m_new_depth_frame) {
					depthMat.copyTo(output);
					m_new_depth_frame = false;
					m_depth_mutex.unlock();
					return true;
				} else {
					m_depth_mutex.unlock();
					return false;
				}
		}

	private:
		std::vector<uint8_t> m_buffer_depth;
		std::vector<uint8_t> m_buffer_rgb;
		std::vector<uint16_t> m_gamma;
		Mat depthMat;
		Mat rgbMat;
		Mat ownMat;
		myMutex m_rgb_mutex;
		myMutex m_depth_mutex;
		bool m_new_rgb_frame;
		bool m_new_depth_frame;
};

void filter(cv::Mat mat, int threshold){
	  for(signed i = 0; i<mat.rows; i++){
			for(signed j = 0; j<mat.cols; j++){
				int mat_val = mat.data[mat.step[0]*i + mat.step[1]* j];
				if(mat_val == 255 or mat_val > threshold){
					mat.data[mat.step[0]*i + mat.step[1]* j] = 0;
				}
			}
		}
}

vector<vector<Point>> drop_contours_2d(vector<vector<Point>> contours, int prop){
	vector<vector<Point>> ret_arr;

	for(unsigned i = 0; i < contours.size(); i++){
		vector<Point> contour;
		for(unsigned j = 0; j < contours.at(i).size(); j++){
			if(j%prop == 0){
				contour.push_back(contours.at(i).at(j));
			}
		}
		ret_arr.push_back(contour);
	}

	return ret_arr;
}

vector<Point> drop_contours_1d(vector<vector<Point>> contours, int prop){
	vector<Point> ret_arr;

	for(unsigned i = 0; i < contours.size(); i++){
		for(unsigned j = 0; j < contours.at(i).size(); j++){
			if(j%prop == 0){
				ret_arr.push_back(contours.at(i).at(j));
			}
		}
	}

	return ret_arr;

}

int main(int argc, char **argv) {
	bool die(false);
	string filename("snapshot");
	string suffix(".png");
	int i_snap(0);

	//various parameters
	int width = 640;
	int height = 480;
	int down_width = 320; //we resize our input arrays for faster computation
	int down_height = 240;
	int final_width = 1280; //final display width and height
	int final_height = 960;
	int contour_drop = 2; //we keep 1/<contour_drop> contours
	int depth_threshold = 1500; //threshold depth in mm
  double FPS = 30.0;

	//create bee handler for calculating bee dynamics
	int num_bees = 400;
	BeeHandle bee_handle(down_width, 240);
	bee_handle.add_bees(num_bees);

	//variables used for temperature
	int frame_count = 0;
	const int window_size = 40; //number of frames to use for temperature smoothing
	const int diff_window = 20; //numer of frames to use for difference smoothing
	int average_temp = 0;
	int average_diff = 0;
	std::vector<int> past_temps;
	std::vector<int> past_diffs;

	//seed our random number generator
	RNG rng(1235);

	//create the matrices we'll use
	Mat depthIn(Size(width,height), CV_16UC1);
	Mat rgbIn(Size(width,height), CV_8UC3, Scalar(0));
	Mat depthMat(Size(down_width,down_height),CV_16UC1);
	Mat rgbMat(Size(down_width,down_height),CV_8UC3);
	Mat depthf (Size(down_width,down_height),CV_8UC3);
	Mat mask (Size(down_width,down_height), CV_8UC3);
	Mat grayMat(Size(down_width,down_height), CV_8UC1);
	Mat cannyResult;
	Mat lastFrame(Size(down_width, down_height), CV_8UC3);
	Mat finalFrame(Size(final_width, final_height), CV_8UC1);

	Freenect::Freenect freenect;
	MyFreenectDevice& device = freenect.createDevice<MyFreenectDevice>(0);

	cv::namedWindow("canny",cv::WINDOW_AUTOSIZE);
	cv::namedWindow("masked",cv::WINDOW_AUTOSIZE);
	device.startVideo();
	device.startDepth();

	while (!die) {

		Mat outMat(Size(width, height),CV_8UC1, Scalar(0));
		vector<vector<Point>> contours;
		vector<Point> flat_contours;
  	vector<Vec4i> hierarchy;

		device.getVideo(rgbIn);
		device.getDepth(depthIn);

		cv::resize(rgbIn, rgbMat, Size(down_width, down_height));
		cv::resize(depthIn, depthMat, Size(down_width, down_height));

		depthMat.convertTo(depthf, CV_8UC3, 255.0/10000.0); //kinect caps out at 10000 mm
		filter(depthf, depth_threshold*255.0/10000.0); //remove background

		cv::threshold(depthf, mask, 1, 255, THRESH_BINARY);
		cv::cvtColor(rgbMat, grayMat, cv::COLOR_BGR2GRAY);
		grayMat.copyTo(outMat, mask);

		//find edges and contours
		cv::medianBlur(outMat, outMat, 7);
		cv::Canny(outMat, cannyResult, 75, 120, 3);
		cv::findContours(cannyResult, contours, hierarchy, cv::RETR_EXTERNAL,
									cv::CHAIN_APPROX_TC89_L1, Point(0,0));
		contours = drop_contours_2d(contours, contour_drop);
		flat_contours = drop_contours_1d(contours, contour_drop);

		//clear flowers for the next state
		bee_handle.clear_flowers();

		//flatten contours and add as flowers to bee_handle
		vector<int> flower_x;
		vector<int> flower_y;

		for(unsigned i = 0; i < flat_contours.size(); i++){
			flower_x.push_back(flat_contours.at(i).x);
			flower_y.push_back(flat_contours.at(i).y);
		}

		bee_handle.add_flowers(flower_x.size(), flower_x, flower_y);

		bee_handle.update_movement_simple();

		//get bee positions
		vector<Point> bee_positions = bee_handle.getBeeCoordinates();

		for(unsigned i = 0; i < bee_positions.size(); i++){
			int yPos = bee_positions.at(i).y;
			int xPos = bee_positions.at(i).x;
			cannyResult.at<uchar>(yPos%down_height, xPos%down_width) = 255;
		}

		/*
		//draw contours
		Mat drawing = Mat::zeros(cannyResult.size(), CV_8UC3 );
	  for(unsigned i = 0; i < contours.size(); i++ )
    {
      Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
      drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
    }
		*/

		//imshow("depth",depthf);
	  //imshow("masked", outMat);
		//imshow("rgb", rgbMat);
		imshow("canny", cannyResult);
		//imshow("contours", drawing);
	  //cv::resize(cannyResult, finalFrame, Size(final_width, final_height));
		//imshow("large edges", finalFrame);

		char k = cv::waitKey(5);

		if( k == 27 ){
			//cv::destroyWindow("rgb");
			//cv::destroyWindow("depth");
			cv::destroyWindow("canny");
			//cv::destroyWindow("masked");
			break;
		}

		if( k == 8 ) {
			std::ostringstream file;
			file << filename << i_snap << suffix;
			cv::imwrite(file.str(),rgbMat);
			i_snap++;
		}

		/*calculation of temperature, takes the average of the differences between
		the last <window_size> frames, then takes the difference between the current
		difference and the average difference. Averages the difference over the last
		<diff_window> frames and prints out the result. This will be the excitement
		metric
		*/
		/*
		int temperature = cv::sum(lastFrame-rgbMat)[0];
		rgbMat.copyTo(lastFrame);

		if(frame_count < window_size){
			frame_count++;
			average_temp=average_temp+temperature/window_size;
			past_temps.push_back(temperature/window_size);

		}else{
			int new_average = average_temp-*past_temps.begin()+temperature/window_size;
			int diff = std::abs(temperature-average_temp);

			if(frame_count-window_size < diff_window){
				frame_count++;
				average_diff = average_diff+diff/diff_window;
				past_diffs.push_back(diff/diff_window);
			}else{
			  int new_avg_diff = average_diff-*past_diffs.begin()+diff/diff_window;
				past_diffs = std::vector<int>(past_diffs.begin()+1, past_diffs.end());
				past_diffs.push_back(diff/diff_window);
				average_diff = new_avg_diff;
			}

			past_temps = std::vector<int>(past_temps.begin()+1, past_temps.end());
			past_temps.push_back(temperature/window_size);
			average_temp = new_average;

			std::cout<<"\rTemperature is: "<<average_diff;
			std::cout<<std::flush;
		}
		*/



	}
	device.stopVideo();
	device.stopDepth();
	return 0;
}
