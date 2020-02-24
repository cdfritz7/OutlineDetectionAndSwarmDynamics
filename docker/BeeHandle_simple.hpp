#include <iostream>
#include <vector>
#include <opencv.hpp>

using namespace std;

class BeeHandle
{
private:
  vector<cv::Point> bees;
  vector<cv::Point> flowers;

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
    for(unsigned i = 0; i < locations.size(); i++){
      this->flowers.push_back(locations.at(i));
    }
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
    int resistance_str = 7.5;
    int attraction_str = 100.0;

    for(unsigned i = 0; i < bees.size(); i++){
      int dist = distance(bees.at(i), p);
      if(dist == 0)
        cur_potential -= resistance_str;
      else
        cur_potential -=  (float)resistance_str/(dist);
    }

    for(unsigned i = 0; i < flowers.size(); i++){
      int dist = distance(flowers.at(i), p);
      if(dist != 0)
        cur_potential += (float)attraction_str/dist;
      else
        cur_potential += attraction_str;
    }

    return cur_potential;
  }

  void update_movement(int move_dist){
    for(unsigned i = 0; i < this->bees.size(); i++){
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
};
