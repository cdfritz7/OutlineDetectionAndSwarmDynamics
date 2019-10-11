#include "libfreenect.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <pthread.h>
#include <opencv.hpp>

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
		MyFreenectDevice(freenect_context *_ctx, int _index)
	 		: Freenect::FreenectDevice(_ctx, _index), m_buffer_depth(FREENECT_DEPTH_11BIT),
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

void filter(cv::Mat mat){
	  for(signed i = 0; i<mat.rows; i++){
			for(signed j = 0; j<mat.cols; j++){
				int mat_val = mat.data[mat.step[0]*i + mat.step[1]* j + 0];
				if(mat_val == 255 or mat_val > 110){
					mat.data[mat.step[0]*i + mat.step[1]* j] = 0;
				}
			}
		}
}

vector<vector<Point>> drop_contours(vector<vector<Point>> contours, int prop){
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

int main(int argc, char **argv) {
	bool die(false);
	string filename("snapshot");
	string suffix(".png");
	int i_snap(0);

	int width = 640;
	int height = 480;

	RNG rng(1235);

	Mat depthMat(Size(width,height),CV_16UC1);
	Mat depthf (Size(width,height),CV_8UC3);
	Mat mask (Size(width,height), CV_8UC3);
	Mat rgbMat(Size(width,height),CV_8UC3, Scalar(0));
	Mat cannyResult;

	// The next two lines must be changed as Freenect::Freenect
	// isn't a template but the method createDevice:
	// Freenect::Freenect<MyFreenectDevice> freenect;
	// MyFreenectDevice& device = freenect.createDevice(0);
	// by these two lines:

	Freenect::Freenect freenect;
	MyFreenectDevice& device = freenect.createDevice<MyFreenectDevice>(0);

	namedWindow("rgb",cv::WINDOW_AUTOSIZE);
	namedWindow("depth",cv::WINDOW_AUTOSIZE);
	device.startVideo();
	device.startDepth();

	while (!die) {
		Mat outMat(Size(width, height),CV_8UC3, Scalar(0));
		vector<vector<Point>> contours;
  	vector<Vec4i> hierarchy;

		device.getVideo(rgbMat);
		device.getDepth(depthMat);

		depthMat.convertTo(depthf, CV_8UC3, 255.0/2048.0);
		filter(depthf); //remove background
		/*
		cv::threshold(depthf, mask, 1, 255, THRESH_BINARY);
		cv::cvtColor(mask, mask, cv::COLOR_GRAY2BGR);
		*/
		//find edges and contours
		Canny(depthf, cannyResult, 10, 20, 3);
		findContours(cannyResult, contours, hierarchy, cv::RETR_EXTERNAL,
									cv::CHAIN_APPROX_TC89_L1, Point(0,0));
		contours = drop_contours(contours, 4);

		//draw contours
		Mat drawing = Mat::zeros(cannyResult.size(), CV_8UC3 );
	  for(unsigned i = 0; i < contours.size(); i++ )
    {
      Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
      drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
    }

		imshow("depth",depthf);
		imshow("rgb", rgbMat);
		imshow("canny", cannyResult);
		imshow("contours", drawing);

		char k = cv::waitKey(5);

		if( k == 27 ){
			cv::destroyWindow("rgb");
			cv::destroyWindow("depth");
			break;
		}

		if( k == 8 ) {
			std::ostringstream file;
			file << filename << i_snap << suffix;
			cv::imwrite(file.str(),rgbMat);
			i_snap++;
		}
	}

	device.stopVideo();
	device.stopDepth();
	return 0;
}
