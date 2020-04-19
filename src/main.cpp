#include "libfreenect.hpp"
#include <iostream>
#include <utility>
#include <vector>
#include <cmath>
#include <pthread.h>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <chrono>
#include <stdlib.h>
#include "BeeHandle.cpp"
#include "MyFreenectDevice.hpp"
#include "AudioHandler.hpp"
#include "./graphics/graphics_module.hpp"
#include "utils.h"

#include "tensorflow/cc/ops/const_op.h"
#include "tensorflow/cc/ops/image_ops.h"
#include "tensorflow/cc/ops/standard_ops.h"
#include "tensorflow/core/framework/graph.pb.h"
#include "tensorflow/core/graph/default_device.h"
#include "tensorflow/core/graph/graph_def_builder.h"
#include "tensorflow/core/lib/core/threadpool.h"
#include "tensorflow/core/lib/io/path.h"
#include "tensorflow/core/lib/strings/stringprintf.h"
#include "tensorflow/core/platform/init_main.h"
#include "tensorflow/core/public/session.h"
#include "tensorflow/core/util/command_line_flags.h"


using namespace cv;
using namespace std;

using tensorflow::Flag;
using tensorflow::Tensor;
using tensorflow::Status;
using tensorflow::string;
using tensorflow::int32;

//convert all values in mat > threshold to 0
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

//used to test whether a string is an integer -> for arg parsing
//https://stackoverflow.com/questions/2844817/how-do-i-check-if-a-c-string-is-an-int
inline bool isInteger(const std::string & s)
{
   if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false;

   char * p;
   strtol(s.c_str(), &p, 10);

   return (*p == 0);
}

/*
given a list of contours, create a list of points by concatenating each contour
Also, drop every <prop> contours
*/
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

/*
potential arguments :
--bees <number of bees>
--time
--scale <scale> (currently multiplied by 320x240 to get screen size)
--size <size>   (the size of each bee)
--soundb <sound divisor> (the sound divisor used for the audiohandler)
*/
int main(int argc, char **argv) {

	//various parameters
	int width = 640;
	int height = 480;
	int down_width = 320; //we resize our input arrays for faster computation
	int down_height = 240;
	int contour_drop = 1; //we keep 1/<contour_drop> contours
	int depth_threshold = 1500; //threshold depth in mm
  int scale = 2; //scale for graphics window
  int bee_size = 2; //size of each bee
	int num_bees = 1200; //number of bees
  int bee_total = 0; //time spent on bee module
	bool time_it = false; //whether we use timing or not
  int sound_divisor = 20; //parameter for audiohandler

  //set variables for timing
	chrono::time_point<std::chrono::high_resolution_clock> time_start;
	chrono::time_point<std::chrono::high_resolution_clock> time_stop;
	chrono::time_point<std::chrono::high_resolution_clock> bee_start;
	chrono::time_point<std::chrono::high_resolution_clock> bee_stop;
	int iterations = 0;

  int contour_count = 0;
  vector<Point> set_contour;

	//argument parsing
	if(argc > 1){
		for(int i = 0; i < argc; i++){
			if(String(argv[i]).compare("--bees")==0 && argc>(i+1) && isInteger(argv[i+1])){
				num_bees = stoi(String(argv[i+1]));
			}

      if(String(argv[i]).compare("--time")==0){
        time_it = true;
        bee_total = 0;
        time_start = chrono::high_resolution_clock::now();
      }

      if(String(argv[i]).compare("--scale")==0 && argc>(i+1) && isInteger(argv[i+1])){
        scale = stoi(String(argv[i+1]));
      }

      if(String(argv[i]).compare("--size")==0 && argc>(i+1) && isInteger(argv[i+1])){
        bee_size = stoi(String(argv[i+1]));
      }

      if(String(argv[i]).compare("--soundb")==0 && argc>(i+1) && isInteger(argv[i+1])){
        sound_divisor = stoi(String(argv[i+1]));
      }
    }
  }

	//create bee handler for calculating bee dynamics
  //xwidth - width of matrix
  //ywidth - height of matrix
  //stepSize - how far a bee will move in a single frame
  //randomFactor - how random the bee movement is [-pi/2, pi/2]
  //numThreads - how many threads
  //storedFrames - how many frames are stored for averaging
  //avgPercent - % of the stored frames that must contain a contour before considering that contour
	BeeHandle bee_handle = BeeHandle(down_width, down_height, 5, 0.8, 4, 15, (double) 1/5);
	//bee_handle.add_bees(num_bees);
  for (int i = 0; i < num_bees; i++){
    bee_handle.addP();
  }

	//BeeHandle bee_handle = BeeHandle(down_width, down_height);
	//bee_handle.add_bees(num_bees);
	int num_sound_bees = num_bees/sound_divisor;

	AudioHandler audio = AudioHandler((int)num_sound_bees);

	//seed our random number generator
	RNG rng(1235);

	//create the matrices we'll use
	Mat depthIn = Mat::zeros(Size(width,height), CV_16UC1);
	Mat rgbIn = Mat::zeros(Size(width,height), CV_8UC3);
	Mat depth_down = Mat::zeros(Size(down_width,down_height),CV_16UC1);
	Mat rgb_down = Mat::zeros(Size(down_width,down_height),CV_8UC3);
	Mat depthf = Mat::zeros(Size(down_width,down_height),CV_8UC3);
	Mat mask = Mat::zeros(Size(down_width,down_height), CV_8UC3);
	Mat grayMat = Mat::zeros(Size(down_width,down_height), CV_8UC1);
	Mat cannyResult;
	Mat lastFrame = Mat::zeros(Size(down_width, down_height), CV_8UC3);
	Mat outMat = Mat::zeros(Size(width, height),CV_8UC1);
	Mat finalFrame = Mat::zeros(Size(down_width, down_height), CV_8UC1);

//path variables 
        string rootdir = "./pbfiles/";
    	string hand_labels = "labels_map.pbtxt";
	string hand_graph = "frozen_inference_graph.pb";
	string gesture_graph = "output_graph.pb";
	string inputLayer = "image_tensor:0";
	vector<string> outputLayer = {"detection_boxes:0", "detection_scores:0", "detection_classes:0", "num_detections:0"};

	//load and initialize the hand model
	std::unique_ptr<tensorflow::Session> session;
	string graph_path = tensorflow::io::JoinPath(rootdir, hand_graph);
	LOG(INFO) << "handgraphPath:" << graph_path;
	Status load_graph_status = loadGraph(graph_path, &session);
	if (!load_graph_status.ok()) {
	    LOG(ERROR) << "loadGraph(): ERROR" << load_graph_status;
	    return -1;
	}

	//load hand labels
	std::map<int, std::string> labels_map = std::map<int,std::string>();
	Status read_labels_status = readLabelsMapFile(tensorflow::io::JoinPath(rootdir, hand_labels), labels_map);
	if (!read_labels_status.ok()) {
	    LOG(ERROR) << "readLabelsMapFile(): ERROR" << read_labels_status;
	    return -1;
	}

	//load and initialize the gesture model
	std::unique_ptr<tensorflow::Session> session2;
	string graph_path2 = tensorflow::io::JoinPath(rootdir, gesture_graph);
	LOG(INFO) << "handgraphPath:" << graph_path2;
	Status load_graph_status2 = loadGraph(graph_path2, &session2);
	if (!load_graph_status2.ok()) {
	    LOG(ERROR) << "loadGraph(): ERROR" << load_graph_status2;
	    return -1;
	} 

	//tensor shape
	Tensor tensor;
    	vector<Tensor> outputs;
	double thresholdScore = 0.5;
    	double thresholdIOU = 0.8;
	tensorflow::TensorShape shape = tensorflow::TensorShape();
    	shape.AddDim(1);
    	shape.AddDim(down_height);
    	shape.AddDim(down_width);
    	shape.AddDim(3);

	//create all the vectors that we'll need
	vector<vector<Point>> contours;
	vector<Point> flat_contours;
	vector<Vec4i> hierarchy;
	vector<Point> bee_positions;
	vector<Point> bee_attractors;
	vector<int> landed;
	//vector<bool> bee_trigger;

	//create our connection to the connect
	Freenect::Freenect freenect;
	MyFreenectDevice& device = freenect.createDevice<MyFreenectDevice>(0);

	//start up our kinect
	device.startVideo();
	device.startDepth();

	/*sometimes frames are dropped, this variable is used to tell if a
	frame has been dropped, or if there's just nothing being
	recorded*/
	int num_dropped = 0;

	//initialization for graphics module
	GraphicsModule gm (num_bees, down_width, down_height,
		                 scale, bee_size/10.0f,
		                 "./graphics/abee.png",
	                   "./graphics/");
	vector<int> bee_x (num_bees);
	vector<int> bee_y (num_bees);
	vector<int> bee_stage (num_bees);

  // 0 - 7 starting north going clockwise
	vector<int> bee_dir (num_bees);

  cv::namedWindow("RGB IN", cv::WINDOW_AUTOSIZE);
  //cv::waitKey(0);
	//main loop
  do {

		//reset our matrices
		outMat = Scalar(0);
		finalFrame = Scalar(0);

		//get one frame of video and one frame of depth from the kinect
		device.getVideo(rgbIn);
		device.getDepth(depthIn);

		cvtColor(rgb_down, rgb_down, COLOR_BGR2RGB);

        	// Convert mat to tensor
        	tensor = Tensor(tensorflow::DT_FLOAT, shape);
        	Status read_tensor_status = readTensorFromMat(rgb_down, tensor);
        	if (!read_tensor_status.ok()) {
         	   LOG(ERROR) << "Mat->Tensor conversion failed: " << read_tensor_status;
   	         return -1;
        	}

 	      	// Run the graph on tensor
 	       	outputs.clear();
 	       	Status runStatus = session->Run({{inputLayer, tensor}}, outputLayer, {}, &outputs);
 	       	if (!runStatus.ok()) {
 	           LOG(ERROR) << "Running model failed: " << runStatus;
 	           return -1;
   	     	}

    	    	// Extract results from the outputs vector
        	tensorflow::TTypes<float>::Flat scores = outputs[1].flat<float>();
        	//tensorflow::TTypes<float>::Flat classes = outputs[2].flat<float>();
        	//tensorflow::TTypes<float>::Flat numDetections = outputs[3].flat<float>();
        	tensorflow::TTypes<float, 3>::Tensor boxes = outputs[0].flat_outer_dims<float,3>();

		vector<size_t> goodIdxs = filterBoxes(scores, boxes, thresholdIOU, thresholdScore);

        	// Draw boxes and captions
        	cvtColor(rgb_down, rgb_down, COLOR_BGR2RGB);
 		// LOG(INFO)<<"rgb_down cols:"<<rgb_down.cols<<endl;
		// LOG(INFO)<<"rgb_down height:"<<rgb_down.size().height<<endl;
		if(scores(goodIdxs.at(0))>0.95){
			bool expected;
        		detect(session2, rgb_down, scores, boxes, goodIdxs, &expected);
			if(expected){

			// Write something here is gesture hookem is detected


			}
		}
		imshow("rgb", rgb_down);

		//resize input image and depth for decreased computation
		cv::resize(rgbIn, rgb_down, Size(down_width, down_height));
		cv::resize(depthIn, depth_down, Size(down_width, down_height));

		cv::imshow("RGB IN", rgbIn);
		cv::waitKey(1);

		//normalize depth to 0-255 range and filter every depth past our threshold
		depth_down.convertTo(depthf, CV_8UC3, 255.0/10000.0); //kinect caps out at 10000 mm
		filter(depthf, depth_threshold*255.0/10000.0); //remove background

		//background subtraction, filter our video mased on our depth thresholding
		cv::threshold(depthf, mask, 1, 255, THRESH_BINARY);
		cv::cvtColor(rgb_down, grayMat, cv::COLOR_BGR2GRAY);
		grayMat.copyTo(outMat, mask);

		//find edges and contours
		cv::medianBlur(outMat, outMat, 3);
		cv::Canny(outMat, cannyResult, 50, 100, 3);
		cv::findContours(cannyResult, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_TC89_L1, cv::Point(0,0));
		flat_contours = drop_contours_1d(contours, contour_drop);
		int old_size = flat_contours.size();	

		// Duplicate edges randomly for better outlines		
		for(int i=0; i < old_size; i++) {
			int duplicate_max = 5;
			int num_to_duplicate = (rand() % duplicate_max);			
			for(int j=0; j<num_to_duplicate; j++) {
				int x_diff = (rand()%3)-1;
				int y_diff = (rand()%3)-1;
				int new_x = flat_contours.at(i).x + x_diff;
				int new_y = flat_contours.at(i).y + y_diff;
				flat_contours.push_back(cv::Point(new_x, new_y));
			
			}	
		}

		//if we have a dropped frame, check if it's a one off, or if there's nothing
		//being recorded
		if(flat_contours.size() < 100){
			if(num_dropped<5){
				num_dropped++;
				continue;
			}
		}else{
			num_dropped = 0;
		}

		//start timer for bee module if timing is enabled
		if(time_it){
			bee_start = chrono::high_resolution_clock::now();
		}

    //if(1000 > contour_count){
      //printf("%d\n", contour_count);
    //  contour_count++;
    //  bee_handle.addAttractorsAvg(flat_contours);
    //} else if (contour_count == 1000){
    //  printf("%d\n", contour_count);
    //  contour_count++;
    //  set_contour = flat_contours;
    //  bee_handle.addAttractorsAvg(set_contour);
    //} else if (contour_count > 2000){
    //  bee_handle.addAttractorsAvg(flat_contours);
    //} else {
    //  //printf("%d\n", contour_count);
    //  contour_count++;
    //  bee_handle.addAttractorsAvg(set_contour);
    //}

		//flatten contours and add as "flowers" to bee_handle
		//bee_handle.add_flowers(flat_contours);

    bee_handle.addAttractorsAvg(flat_contours);
		bee_handle.updatePoints();
		
		int new_size = flat_contours.size();
		for(int i=0; i<(new_size-old_size); i++) {
			flat_contours.pop_back();
		}

		//get bee positions
		bee_positions.clear();
    
		//bee_positions = bee_handle.get_bees();
    bee_positions = bee_handle.getPoints();
    //bee_positions = flat_contours;
    //std::vector<Point> combined(flat_contours);
    //std::vector<Point> bh_points = bee_handle.getPoints();
    //combined.insert(combined.end(), bh_points.begin(), bh_points.end());
    //bee_positions = combined;
    bee_dir = bee_handle.get_dirs();

		//update our graphics module
		for(int i = 0; i < num_bees; i++){
			bee_x[i] = bee_positions[i].x;
			bee_y[i] = bee_positions[i].y;
			bee_stage[i] = (bee_stage[i]+1)%12;
			//bee_dir[i] = (bee_dir[i]+1)%8;
		}
		gm.update_particles(bee_x, bee_y, bee_stage, bee_dir);
    		gm.update_qr(true, "./graphics/qr.png", 320, 240, 2.0f);
		gm.update_display();

		landed = bee_handle.get_landed();
		for(unsigned i = 0; i < landed.size(); i++){
			if(landed.at(i) == 1){
				audio.play_sound(i);
			}
		}

		//end timer for bees if timing is enabled
		if(time_it){
			bee_stop = chrono::high_resolution_clock::now();
			auto temp_bee_total = chrono::duration_cast<chrono::microseconds>(bee_stop - bee_start);
			bee_total += (int)temp_bee_total.count();
		}

		iterations++;
	}while(!gm.should_close());


	//output the results of our timing
	if(time_it){
		time_stop = chrono::high_resolution_clock::now();
		auto total = chrono::duration_cast<chrono::microseconds>(time_stop-time_start);
		cout<<"frames: "<<iterations<<endl;
		cout<<"Frames per second: "<<(float)iterations/(total.count()/1000000.0)<<endl;
		cout<<"Percentage bee module: "<<100*(float)bee_total/total.count()<<"%\n";
		cout<<"Percentage edge module: "<<100*(float)(total.count()-bee_total)/total.count()<<"%\n";
	}

	//clean up everything we have
	audio.delete_sources();
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

	return 0;
}
