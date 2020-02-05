#include "libfreenect.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <pthread.h>
#include <opencv.hpp>
#include <fstream>
#include <chrono>
#include "BeeHandle_simple.hpp"

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
			m_buffer_rgb(FREENECT_VIDEO_RGB), m_gamma(2048),
			depthMat(Size(640,480),CV_16UC1),
			rgbMat(Size(640,480), CV_8UC3, Scalar(0)),
			ownMat(Size(640,480), CV_8UC3, Scalar(0)),
			m_new_rgb_frame(false),
			m_new_depth_frame(false) {

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

	//various parameters
	int width = 640;
	int height = 480;
	int down_width = 320; //we resize our input arrays for faster computation
	int down_height = 240;
	int contour_drop = 6; //we keep 1/<contour_drop> contours
	int depth_threshold = 1500; //threshold depth in mm

	//create bee handler for calculating bee dynamics
	int num_bees = 800;
	BeeHandle bee_handle = BeeHandle();
	bee_handle.add_bees(num_bees, down_width, down_height);

	//variables used for temperature
	/*
	int frame_count = 0;
	const int window_size = 40; //number of frames to use for temperature smoothing
	const int diff_window = 20; //numer of frames to use for difference smoothing
	int average_temp = 0;
	int average_diff = 0;
	std::vector<int> past_temps;
	std::vector<int> past_diffs;
	*/

	//seed our random number generator
	RNG rng(1235);

	//create the matrices we'll use
	Mat depthIn(Size(width,height), CV_16UC1);
	Mat rgbIn(Size(width,height), CV_8UC3, Scalar(0));
	Mat depth_down(Size(down_width,down_height),CV_16UC1);
	Mat rgb_down(Size(down_width,down_height),CV_8UC3);
	Mat depthf (Size(down_width,down_height),CV_8UC3);
	Mat mask (Size(down_width,down_height), CV_8UC3);
	Mat grayMat(Size(down_width,down_height), CV_8UC1);
	Mat cannyResult;
	Mat lastFrame(Size(down_width, down_height), CV_8UC3);
	Mat outMat(Size(width, height),CV_8UC1, Scalar(0));
	Mat finalFrame(Size(down_width, down_height), CV_8UC1, Scalar(0));

	vector<vector<Point>> contours;
	vector<Point> flat_contours;
	vector<Vec4i> hierarchy;
	vector<Point> bee_positions;

	Freenect::Freenect freenect;
	MyFreenectDevice& device = freenect.createDevice<MyFreenectDevice>(0);

	cv::namedWindow("Bees",cv::WINDOW_NORMAL);
	device.startVideo();
	device.startDepth();

	//set variables for timing
	chrono::time_point<std::chrono::high_resolution_clock> time_start;
	chrono::time_point<std::chrono::high_resolution_clock> time_stop;
	chrono::time_point<std::chrono::high_resolution_clock> bee_start;
	chrono::time_point<std::chrono::high_resolution_clock> bee_stop;
	int bee_total;
	int iterations = 0;
	bool time_it = false;

	//start timing
	if(argc > 1 && String(argv[1]).compare("time")==0){
		time_it = true;
		bee_total = 0;
		time_start = chrono::high_resolution_clock::now();
	}

	//variables for recording video, just press p
	chrono::time_point<std::chrono::high_resolution_clock> video_time_start;
	chrono::time_point<std::chrono::high_resolution_clock> video_time_end;
	bool is_recording = false;
	VideoWriter video("outcpp.avi",VideoWriter::fourcc('M','J','P','G'),10, Size(down_width,down_height));

	while (!die) {

		outMat = Scalar(0);
		finalFrame = Scalar(0);

		contours.clear();
		flat_contours.clear();
  	hierarchy.clear();

		device.getVideo(rgbIn);
		device.getDepth(depthIn);

		//resize input image and depth for decreased computation
		cv::resize(rgbIn, rgb_down, Size(down_width, down_height));
		cv::resize(depthIn, depth_down, Size(down_width, down_height));


		depth_down.convertTo(depthf, CV_8UC3, 255.0/10000.0); //kinect caps out at 10000 mm
		filter(depthf, depth_threshold*255.0/10000.0); //remove background

		cv::threshold(depthf, mask, 1, 255, THRESH_BINARY);
		cv::cvtColor(rgb_down, grayMat, cv::COLOR_BGR2GRAY);
		grayMat.copyTo(outMat, mask);

		//find edges and contours
		cv::medianBlur(outMat, outMat, 7);
		cv::Canny(outMat, cannyResult, 50, 100, 3);
		cv::findContours(cannyResult, contours, hierarchy, cv::RETR_EXTERNAL,
									cv::CHAIN_APPROX_TC89_L1, Point(0,0));
		//contours = drop_contours_2d(contours, contour_drop);
		flat_contours = drop_contours_1d(contours, contour_drop);

		//start timer for bee module if timing is enabled
		if(time_it){
			bee_start = chrono::high_resolution_clock::now();
		}

		//clear flowers for the next state
		bee_handle.clear_flowers();

		//flatten contours and add as flowers to bee_handle
		bee_handle.add_flowers(flat_contours);
		bee_handle.update_movement(3);

		//get bee positions
		bee_positions.clear();
		bee_positions = bee_handle.get_bees();

		for(unsigned i = 0; i < bee_positions.size(); i++){
			int yPos = bee_positions.at(i).y;
			int xPos = bee_positions.at(i).x;
			finalFrame.at<uchar>(yPos%down_height, xPos%down_width) = 255;
			finalFrame.at<uchar>((yPos+1)%down_height, (xPos)%down_width) = 255;
			finalFrame.at<uchar>((yPos)%down_height, (xPos+1)%down_width) = 255;
			finalFrame.at<uchar>((yPos+1)%down_height, (xPos+1)%down_width) = 255;
		}

		//end timer for bees if timing is enabled
		if(time_it){
			bee_stop = chrono::high_resolution_clock::now();
			auto temp_bee_total = chrono::duration_cast<chrono::microseconds>(bee_stop - bee_start);
			bee_total += (int)temp_bee_total.count();
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

		imshow("Bees", 255-finalFrame);

		char k = cv::waitKey(5);
		if( k == 27 ){
			cv::destroyWindow("Bees");
			break;
		}

		//checks if we are recording and saves frame
		if(is_recording){
			video.write(255-finalFrame);
			video_time_end = chrono::high_resolution_clock::now();
			auto video_total = chrono::duration_cast<chrono::seconds>(time_stop-time_start);
			if((int)video_total.count()>=30){
				is_recording = false;
			}
		}

		if(k == 80 && is_recording==false){
			is_recording = true;
			video_time_start = chrono::high_resolution_clock::now();
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

		iterations++;
	}

	if(time_it){
		time_stop = chrono::high_resolution_clock::now();
		auto total = chrono::duration_cast<chrono::microseconds>(time_stop-time_start);
		cout<<"frames: "<<iterations<<endl;
		cout<<"Frames per second: "<<(float)iterations/(total.count()/1000000.0)<<endl;
		cout<<"Percentage bee module: "<<100*(float)bee_total/total.count()<<"%\n";
		cout<<"Percentage edge module: "<<100*(float)(total.count()-bee_total)/total.count()<<"%\n";
	}

	device.stopVideo();
	device.stopDepth();
	finalFrame.release();
	outMat.release();
	depthIn.release();
	rgbIn.release();
	depth_down.release();
	rgb_down.release();
	depthf.release();
	mask.release();
	grayMat.release();
	cannyResult.release();
	lastFrame.release();
	video.release();

	return 0;
}
