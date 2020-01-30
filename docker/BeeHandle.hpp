#define _USE_MATH_DEFINES
#include <cstdlib>
#include <ctime>
#include <math.h>
#include <cstring>
#include <iostream>
#include <vector>
#include <opencv.hpp>

using namespace std;

const int MoveMax = 10;
const int FlowerStrength = 3;
const int BeeStrength = 10;
const int RandomNessFactor = M_PI / 8;

float RandomFloat(float a, float b)
{
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}

class BeeHandle
{
private:
	vector<int> bee_array_x;
	vector<int> bee_array_y;

	vector<int> flower_array_x;
	vector<int> flower_array_y;

	int MatrixHeight;
	int MatrixLength;
	int MatrixMaxMag;

	int num_flowers;
	int num_bees;

public:
	BeeHandle(int MatrixHeight, int MatrixLength)
	{
		this->MatrixLength = MatrixLength;
		this->MatrixHeight = MatrixHeight;
	  this->MatrixMaxMag = sqrt(pow(this->MatrixLength, 2) + pow(this->MatrixHeight, 2));
		this->num_flowers = 0;
		this->num_bees = 0;
	};

	~BeeHandle()
	{
			/*
		delete[] this->bee_array_x;
		delete[] this->bee_array_y;
		delete[] this->flower_array_x;
		delete[] this->flower_array_y;
		*/
	};

	void add_bees(int num_bees){
		for(int i = 0; i < num_bees; i++){
			add_bee();
		}
	}

	void add_bees(int num_bees, vector<int> x_coords, vector<int> y_coords){
		for(int i = 0; i < num_bees; i++){
			add_bee(x_coords.at(i), y_coords.at(i));
		}
	}

	void add_flowers(int num_flowers){
		for(int i = 0; i < num_flowers; i++){
			add_flower();
		}

		cout<<this->num_flowers<<endl;
	}

	void add_flowers(int num_flowers, vector<int> x_coords, vector<int> y_coords){
		for(int i = 0; i < num_flowers; i++){
			add_flower(x_coords.at(i), y_coords.at(i));
			//cout<<this->flower_array_x.at(i]<<endl;
		}
	}

	void add_bee(int x = -1, int y = -1)
	{
		if (this->num_bees < this->MatrixHeight * this->MatrixLength)
		{
			if (x < 0)
			{
				x = rand() % this->MatrixLength;
			}
			if (y < 0)
			{
				y = rand() % this->MatrixHeight;
			}
			this->bee_array_x.push_back(x);
			this->bee_array_y.push_back(y);
			this->num_bees++;
		}
		else {
			cout << "Max number of bees reached" << endl;
		}
	}

	void add_flower(int x = -1, int y = -1)
	{
		if (this->num_flowers < this->MatrixHeight * this->MatrixLength)
		{
			if (x < 0)
			{
				x = rand() % this->MatrixLength;
			}
			if (y < 0)
			{
				y = rand() % this->MatrixHeight;
			}
			this->flower_array_x.push_back(x);
			this->flower_array_y.push_back(y);
			this->num_flowers++;
		}
		else {
			cout << "Max number of flowers reached" << endl;
		}
	}

	void clear_flowers()
	{
		this->num_flowers = 0;
		this->flower_array_x.clear();
		this->flower_array_y.clear();
	}

	void update_movement_test(){
		for(int i = 0; i < this->num_bees; i++){
			if(rand()%10 < 2)
				this->bee_array_x.at(i) = this->bee_array_x.at(i)+1%this->MatrixLength;
		}
	}

	float distance(int x1, int y1, int x2, int y2){
		return sqrt(pow(x1-x2, 2)+pow(y1-y2, 2));
	}

	cv::Point find_closest_flower(int x, int y){
		float min_dist = 1000000000.0;
		cv::Point closest = cv::Point(0, 0);

		for(int i = 0; i < this->num_flowers; i++){
			float dist = distance(x, y, this->flower_array_x.at(i), this->flower_array_y.at(i));
			if(dist < min_dist){
				min_dist = dist;
				closest.x = this->flower_array_x.at(i);
				closest.y = this->flower_array_y.at(i);
			}
		}

		return closest;
	}

	cv::Point find_closest_bee(int x, int y){
		float min_dist = 1000000000.0;
		cv::Point closest = cv::Point(0, 0);

		for(int i = 0; i < this->num_bees; i++){
			float dist = distance(x, y, this->bee_array_x.at(i), this->bee_array_y.at(i));
			if(dist < min_dist){
				min_dist = dist;
				closest.x = this->bee_array_x.at(i);
				closest.y = this->bee_array_y.at(i);
			}
		}

		return closest;
	}

	float find_potential(int x, int y){
		float pot = 0.0;
		int resistance_str = 1;
		int attraction_str = 100;

		for(int i = 0; i < this->num_bees; i++){
			float dist = distance(x, y, this->bee_array_x.at(i), this->bee_array_y.at(i));
			if(dist != 0.0){
				pot -= resistance_str/dist;
			}else{
				pot -= resistance_str/1000;
			}
		}

		for(int i = 0; i < this->num_flowers; i++){
			float dist = distance(x, y, this->flower_array_x.at(i), this->flower_array_y.at(i));
			pot += attraction_str/dist;
		}

		return pot;
	}

	void update_movement_simple(){

		for(int i = 0; i < this->num_bees; i++){
			cv::Point closest_flower = find_closest_flower(this->bee_array_x.at(i), this->bee_array_y.at(i));
			cv::Point closest_bee = find_closest_bee(this->bee_array_x.at(i), this->bee_array_y.at(i));
			float closest_flower_dist = distance(this->bee_array_x.at(i), this->bee_array_y.at(i), closest_flower.x, closest_flower.y);
			float closest_bee_dist = distance(this->bee_array_x.at(i), this->bee_array_y.at(i), closest_bee.x, closest_bee.y);
			int chance;

			int newX = this->bee_array_x.at(i) + ((rand() % 5)-2);
			int newY = this->bee_array_y.at(i) + ((rand() % 5)-2);

			float currPotential = find_potential(this->bee_array_x.at(i), this->bee_array_y.at(i));
			float newPotential = find_potential(newX, newY);

			if(newPotential > currPotential){
				bee_array_x.at(i) = newX;
				bee_array_y.at(i) = newY;
			}

		}
	}


	void update_movement_b()
	{
		int* bee_flower = new int[this->num_bees];
		int* flower_bee = new int[this->num_flowers];

		for (int i = 0; i < this->num_bees; i++) {
			bee_flower[i] = -1;
		}
		for (int i = 0; i < this->num_flowers; i++) {
			flower_bee[i] = -1;
		}

		if (this->num_bees > this->num_flowers) {
			int* flower_idx = new int[this->num_bees];
			int* bee_queue = new int[this->num_bees * this->num_flowers];

			for (int i = 0; i < this->num_bees; i++) {
				flower_idx[i] = 0;
				bee_queue[i] = i;
			}

			for (int i = this->num_bees; i < this->num_bees * this->num_flowers; i++) {
				bee_queue[i] = -1;
			}

			int bee_queue_idx = 0;
			int bee_queue_last_idx = this->num_bees;

			int b_idx, f_idx;
			int bee_x, bee_y, new_bee_x, new_bee_y;
			int flower_x, flower_y;
			int dist_x, dist_y, bee_dist, new_bee_dist;

			while (bee_queue[bee_queue_idx] != -1) {
				b_idx = bee_queue[bee_queue_idx];

				bee_x = this->bee_array_x.at(b_idx);
				bee_y = this->bee_array_y.at(b_idx);

				f_idx = flower_idx[b_idx];

				while (bee_flower[b_idx] == -1 && f_idx < this->num_flowers) {
					flower_x = this->flower_array_x.at(f_idx);
					flower_y = this->flower_array_y.at(f_idx);

					if (flower_bee[f_idx] == -1) {
						bee_flower[b_idx] = f_idx;
						flower_bee[f_idx] = b_idx;
						flower_idx[b_idx]++;
					}
					else {
						new_bee_x = this->bee_array_x.at(flower_bee[f_idx]);
						new_bee_y = this->bee_array_y.at(flower_bee[f_idx]);

						dist_x = flower_x - bee_x;
						dist_y = flower_y - bee_y;
						bee_dist = sqrt(pow(dist_x, 2) + pow(dist_y, 2));

						dist_x = flower_x - new_bee_x;
						dist_y = flower_y - new_bee_y;
						new_bee_dist = sqrt(pow(dist_x, 2) + pow(dist_y, 2));

						if (bee_dist < new_bee_dist) {
							bee_flower[flower_bee[f_idx]] = -1;
							bee_queue[bee_queue_last_idx] = flower_bee[f_idx];
							bee_queue_last_idx++;

							flower_bee[f_idx] = b_idx;
							bee_flower[b_idx] = f_idx;

							f_idx++;
							flower_idx[b_idx] = f_idx;
						}
						else {
							f_idx++;
							flower_idx[b_idx]++;
						}
					}
				}

				bee_queue_idx++;
			}

			delete[] flower_idx;
			delete[] bee_queue;
		}
		else {
			int* bee_idx = new int[this->num_flowers];
			int* flower_queue = new int[this->num_bees * this->num_flowers];

			for (int i = 0; i < this->num_flowers; i++) {
				bee_idx[i] = 0;
				flower_queue[i] = i;
			}

			for (int i = this->num_flowers; i < this->num_bees * this->num_flowers; i++) {
				flower_queue[i] = -1;
			}

			int flower_queue_idx = 0;
			int flower_queue_last_idx = this->num_flowers;

			int b_idx, f_idx;
			int bee_x, bee_y;
			int flower_x, flower_y, new_flower_x, new_flower_y;
			int dist_x, dist_y, flower_dist, new_flower_dist;

			while (flower_queue[flower_queue_idx] != -1) {
				f_idx = flower_queue[flower_queue_idx];

				flower_x = this->flower_array_x.at(f_idx);
				flower_y = this->flower_array_y.at(f_idx);

				b_idx = bee_idx[f_idx];

				while (flower_bee[f_idx] == -1 && b_idx < this->num_bees) {
					bee_x = this->bee_array_x.at(b_idx);
					bee_y = this->bee_array_y.at(b_idx);

					if (bee_flower[b_idx] == -1) {
						flower_bee[f_idx] = b_idx;
						bee_flower[b_idx] = f_idx;
						bee_idx[f_idx]++;
					}
					else {
						new_flower_x = this->flower_array_x.at(bee_flower[b_idx]);
						new_flower_y = this->flower_array_y.at(bee_flower[b_idx]);

						dist_x = flower_x - bee_x;
						dist_y = flower_y - bee_y;
						flower_dist = sqrt(pow(dist_x, 2) + pow(dist_y, 2));

						dist_x = new_flower_x - bee_x;
						dist_y = new_flower_y - bee_y;
						new_flower_dist = sqrt(pow(dist_x, 2) + pow(dist_y, 2));

						if (flower_dist < new_flower_dist) {
							flower_bee[bee_flower[b_idx]] = -1;
							flower_queue[flower_queue_last_idx] = bee_flower[b_idx];
							flower_queue_last_idx++;

							bee_flower[b_idx] = f_idx;
							flower_bee[f_idx] = b_idx;

							b_idx++;
							bee_idx[f_idx] = b_idx;
						}
						else {
							b_idx++;
							bee_idx[f_idx]++;
						}
					}
				}

				flower_queue_idx++;
			}

			delete[] bee_idx;
			delete[] flower_queue;
		}

		int bee_x, bee_y;
		int flower_x, flower_y;
		int dist_x, dist_y, dist;
		int new_x, new_y;

		for (int i = 0; i < this->num_bees; i++) {
			bee_x = this->bee_array_x.at(i);
			bee_y = this->bee_array_y.at(i);

			if (bee_flower[i] != -1) {
				flower_x = this->flower_array_x.at(bee_flower[i]);
				flower_y = this->flower_array_x.at(bee_flower[i]);

				dist_x = flower_x - bee_x;
				dist_y = flower_y - bee_y;

				dist = sqrt(pow(dist_x, 2) + pow(dist_y, 2));

				if (dist > MoveMax) {
					float rads = atan2(dist_y, dist_x) + RandomFloat(-1 * RandomNessFactor, RandomNessFactor);

					if (!(dist_x == 0 && dist_y == 0)) {
						new_x = bee_x + int(cos(rads) * MoveMax);
						new_y = bee_y + int(sin(rads) * MoveMax);

						if (new_x < 0 || new_x > this->MatrixLength) {
							this->bee_array_x.at(i) = bee_x - int(cos(rads) * MoveMax);
						}
						else {
							this->bee_array_x.at(i) = new_x;
						}

						if (new_y < 0 || new_y > this->MatrixHeight) {
							this->bee_array_y.at(i) = bee_y - int(sin(rads) * MoveMax);
						}
						else {
							this->bee_array_y.at(i) = new_y;
						}
					}
				}
				else {
					this->bee_array_x.at(i) = this->flower_array_x.at(bee_flower[i]);
					this->bee_array_y.at(i) = this->flower_array_y.at(bee_flower[i]);
				}
			}
			else {
				dist_x = RandomFloat(-1.0, 1.0);
				dist_y = RandomFloat(-1.0, 1.0);

				float rads = atan2(dist_y, dist_x) + RandomFloat(-1 * RandomNessFactor, RandomNessFactor);

				if (!(dist_x == 0 && dist_y == 0)) {
					new_x = bee_x + int(cos(rads) * MoveMax);
					new_y = bee_y + int(sin(rads) * MoveMax);

					if (new_x < 0 || new_x > this->MatrixLength) {
						this->bee_array_x.at(i) = bee_x - int(cos(rads) * MoveMax);
					}
					else {
						this->bee_array_x.at(i) = new_x;
					}

					if (new_y < 0 || new_y > this->MatrixHeight) {
						this->bee_array_y.at(i) = bee_y - int(sin(rads) * MoveMax);
					}
					else {
						this->bee_array_y.at(i) = new_y;
					}
				}
			}
		}

		delete[] bee_flower;
		delete[] flower_bee;
	}


	int getNumBees()
	{
		return this->num_bees;
	}

	int getNumFlowers()
	{
		return this->num_flowers;
	}

	//returns all current flower coordinates in a C++ vector
	vector<cv::Point> getFlowerCoordinates(){
		vector<cv::Point> flowers;

		for(int i = 0; i < this->num_flowers; i++){
			cv::Point new_point =  cv::Point(this->flower_array_x.at(i), this->flower_array_y.at(i));
			flowers.push_back(new_point);
		}

		return flowers;
	}

	//returns all current bee in a c++ vector
	vector<cv::Point> getBeeCoordinates(){
		vector<cv::Point> bees;

		for(int i = 0; i < this->num_bees; i++){
			cv::Point new_point = cv::Point(this->bee_array_x.at(i), this->bee_array_y.at(i));
			bees.push_back(new_point);
		}

		return bees;
	}

};
