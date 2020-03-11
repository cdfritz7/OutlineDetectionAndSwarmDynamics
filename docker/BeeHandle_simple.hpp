#include <iostream>
#include <vector>
#include <opencv.hpp>
#include <thread>

#define NUM_THREADS 8

using namespace std;

class BeeHandle
{
private:
  vector<cv::Point> bees;
  vector<cv::Point> flowers;
  thread threads [NUM_THREADS];

  void join_threads() {
  	for(int i=0; i<NUM_THREADS; i++) {
  		threads[i].join();
  	}
  }

  void set_threads() {
  	for(int i=0; i<NUM_THREADS; i++) {
  		threads[i] = thread(&BeeHandle::move_bee, this, i);
  	}
  }

  void move_bee(int thread_num) {
    int move_dist = 4; // TODO extract
  	int bee_per_thread = bees.size()/NUM_THREADS;
  	int start = bee_per_thread*thread_num;
  	int end = start + bee_per_thread;
  	for(int i=start; i<end; i++) {
      int newX = this->bees.at(i).x + ((rand() % 3)-1)*move_dist;
      int newY = this->bees.at(i).y + ((rand() % 3)-1)*move_dist;
      cv::Point new_pos = cv::Point(newX, newY);

      float currPotential = get_potential(this->bees.at(i));
      float newPotential = get_potential(new_pos);

      if(newPotential > currPotential){
        bees.at(i) = new_pos;
      }

  	}
  }
public:
  BeeHandle(){}

  void add_bees(vector<cv::Point> locations){
    for(unsigned i = 0; i < locations.size(); i++){
      this->bees.push_back(locations.at(i));
    }
  }

  void add_bees(int num_bees, int max_x, int max_y){
    for(int i = 0; i < num_bees; i++){
      cv::Point p1 = cv::Point((int)(rand()%max_x), (int)(rand()%max_y));
      this->bees.push_back(p1);
    }
  }

  void add_flowers(vector<cv::Point> locations){
    //for(unsigned i = 0; i < locations.size(); i++){
    //  this->flowers.push_back(locations.at(i));
    //}
    flowers = locations;
  }

  vector<cv::Point> get_bees(){
    return this->bees;
  }

  void clear_flowers(){
    this->flowers.clear();
  }

  void clear_bees(){
    this->bees.clear();
  }

  int distance(cv::Point p1, cv::Point p2){
    return  (int)cv::norm(p1-p2);
  }

  float get_potential(cv::Point p){
    float cur_potential = 0;
    int resistance_str = 1000;
    int attraction_str = 10000;
    int bee_stride = 32;//10;//20;//36;
    int flower_stride = 1;
    int random_off_bee = rand()%bee_stride;
    int random_off_flower = rand()%flower_stride;

    for(unsigned i = random_off_bee; i < bees.size(); i+=bee_stride){
      int dist = distance(bees.at(i), p);
      if(dist != 0)
        cur_potential -=  (float)resistance_str*bee_stride/(dist);
      else
        cur_potential -= resistance_str*bee_stride;

    }

    for(unsigned i = random_off_flower; i < flowers.size(); i+=flower_stride){
      int dist = distance(flowers.at(i), p);
      if(dist != 0)
        cur_potential += (float)attraction_str*flower_stride/dist;
      else
        cur_potential += attraction_str*attraction_str;
    }

    return cur_potential;
  }

  void update_movement(int move_dist){
    set_threads();
    join_threads();
    // for(unsigned i = 0; i < this->bees.size(); i++){
    //   int newX = this->bees.at(i).x + ((rand() % 3)-1)*move_dist;
    //   int newY = this->bees.at(i).y + ((rand() % 3)-1)*move_dist;
    //   cv::Point new_pos = cv::Point(newX, newY);
    //
    //   float currPotential = get_potential(this->bees.at(i));
    //   float newPotential = get_potential(new_pos);
    //
    //   if(newPotential > currPotential){
    //     bees.at(i) = new_pos;
    //   }
    // }
  }
};
