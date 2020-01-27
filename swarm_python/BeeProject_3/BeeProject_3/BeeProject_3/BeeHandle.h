#define _USE_MATH_DEFINES
#include <cstdlib>
#include <ctime>
#include <math.h>
#include <cstring>
#include <iostream>

using namespace std;

const int MatrixHeight = 300;
const int MatrixLength = 300;
const int MoveMax = 10;
const int FlowerStrength = 3;
const int BeeStrength = 10;
const int RandomNessFactor = M_PI / 8;
const int MatrixMaxMag = sqrt(pow(MatrixLength, 2) + pow(MatrixHeight, 2));


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
	int* bee_array_x;
	int* bee_array_y;

	int* flower_array_x;
	int* flower_array_y;

	//int* bee_flower;
	//int* flower_bee;

	int num_flowers;
	int num_bees;
public:
	BeeHandle()
	{
		bee_array_x = new int[MatrixHeight * MatrixLength];
		bee_array_y = new int[MatrixHeight * MatrixLength];
		flower_array_x = new int[MatrixHeight * MatrixLength];
		flower_array_y = new int[MatrixHeight * MatrixLength];
		//bee_flower = new int[MatrixHeight * MatrixLength];
		//flower_bee = new int[MatrixHeight * MatrixLength];
		num_flowers = 0;
		num_bees = 0;
	};
	~BeeHandle()
	{
		delete[] bee_array_x;
		delete[] bee_array_y;
		delete[] flower_array_x;
		delete[] flower_array_y;
		//delete[] bee_flower;
		//delete[] flower_bee;
	};
	void add_bee(int x = -1, int y = -1)
	{
		if (num_bees < MatrixHeight * MatrixLength)
		{
			if (x < 0)
			{
				x = rand() % MatrixLength;
			}
			if (y < 0)
			{
				y = rand() % MatrixHeight;
			}
			bee_array_x[num_bees] = x;
			bee_array_y[num_bees] = y;
			//bee_flower[num_bees] = -1;
			num_bees++;
		}
		else {
			cout << "Max number of bees reached" << endl;
		}
	}
	void add_flower(int x = -1, int y = -1)
	{
		if (num_flowers < MatrixHeight * MatrixLength)
		{
			if (x < 0)
			{
				x = rand() % MatrixLength;
			}
			if (y < 0)
			{
				y = rand() % MatrixHeight;
			}
			flower_array_x[num_flowers] = x;
			flower_array_y[num_flowers] = y;
			//flower_bee[num_flowers] = -1;
			num_flowers++;
		}
		else {
			cout << "Max number of flowers reached" << endl;
		}
	}
	void clear_flowers()
	{
		num_flowers = 0;
	}
	void update_movement()
	{
		int* bee_flower = new int[num_bees];
		int* flower_bee = new int[num_flowers];

		for (int i = 0; i < num_bees; i++) {
			bee_flower[i] = -1;
		}
		for (int i = 0; i < num_flowers; i++) {
			flower_bee[i] = -1;
		}

		if (num_bees > num_flowers) {
			int* flower_idx = new int[num_bees];
			int* bee_queue = new int[num_bees * num_flowers];

			for (int i = 0; i < num_bees; i++) {
				flower_idx[i] = 0;
				bee_queue[i] = i;
			}

			for (int i = num_bees; i < num_bees * num_flowers; i++) {
				bee_queue[i] = -1;
			}

			int bee_queue_idx = 0;
			int bee_queue_last_idx = num_bees;

			int b_idx, f_idx;
			int bee_x, bee_y, new_bee_x, new_bee_y;
			int flower_x, flower_y;
			int dist_x, dist_y, bee_dist, new_bee_dist;

			while (bee_queue[bee_queue_idx] != -1) {
				b_idx = bee_queue[bee_queue_idx];

				bee_x = bee_array_x[b_idx];
				bee_y = bee_array_y[b_idx];

				f_idx = flower_idx[b_idx];

				while (bee_flower[b_idx] == -1 && f_idx < num_flowers) {
					flower_x = flower_array_x[f_idx];
					flower_y = flower_array_y[f_idx];

					if (flower_bee[f_idx] == -1) {
						bee_flower[b_idx] = f_idx;
						flower_bee[f_idx] = b_idx;
						flower_idx[b_idx]++;
					}
					else {
						new_bee_x = bee_array_x[flower_bee[f_idx]];
						new_bee_y = bee_array_y[flower_bee[f_idx]];

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
			int* bee_idx = new int[num_flowers];
			int* flower_queue = new int[num_bees * num_flowers];

			for (int i = 0; i < num_flowers; i++) {
				bee_idx[i] = 0;
				flower_queue[i] = i;
			}

			for (int i = num_flowers; i < num_bees * num_flowers; i++) {
				flower_queue[i] = -1;
			}

			int flower_queue_idx = 0;
			int flower_queue_last_idx = num_flowers;

			int b_idx, f_idx;
			int bee_x, bee_y;
			int flower_x, flower_y, new_flower_x, new_flower_y;
			int dist_x, dist_y, flower_dist, new_flower_dist;

			while (flower_queue[flower_queue_idx] != -1) {
				f_idx = flower_queue[flower_queue_idx];

				flower_x = flower_array_x[f_idx];
				flower_y = flower_array_y[f_idx];

				b_idx = bee_idx[f_idx];

				while (flower_bee[f_idx] == -1 && b_idx < num_bees) {
					bee_x = bee_array_x[b_idx];
					bee_y = bee_array_y[b_idx];

					if (bee_flower[b_idx] == -1) {
						flower_bee[f_idx] = b_idx;
						bee_flower[b_idx] = f_idx;
						bee_idx[f_idx]++;
					}
					else {
						new_flower_x = flower_array_x[bee_flower[b_idx]];
						new_flower_y = flower_array_y[bee_flower[b_idx]];

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

		for (int i = 0; i < num_bees; i++) {
			bee_x = bee_array_x[i];
			bee_y = bee_array_y[i];

			if (bee_flower[i] != -1) {
				flower_x = flower_array_x[bee_flower[i]];
				flower_y = flower_array_x[bee_flower[i]];

				dist_x = flower_x - bee_x;
				dist_y = flower_y - bee_y;

				dist = sqrt(pow(dist_x, 2) + pow(dist_y, 2));

				if (dist > MoveMax) {
					float rads = atan2(dist_y, dist_x) + RandomFloat(-1 * RandomNessFactor, RandomNessFactor);

					if (!(dist_x == 0 && dist_y == 0)) {
						new_x = bee_x + int(cos(rads) * MoveMax);
						new_y = bee_y + int(sin(rads) * MoveMax);

						if (new_x < 0 || new_x > MatrixLength) {
							bee_array_x[i] = bee_x - int(cos(rads) * MoveMax);
						}
						else {
							bee_array_x[i] = new_x;
						}

						if (new_y < 0 || new_y > MatrixHeight) {
							bee_array_y[i] = bee_y - int(sin(rads) * MoveMax);
						}
						else {
							bee_array_y[i] = new_y;
						}
					}
				}
				else {
					bee_array_x[i] = flower_array_x[bee_flower[i]];
					bee_array_y[i] = flower_array_y[bee_flower[i]];
				}
			}
			else {
				dist_x = RandomFloat(-1.0, 1.0);
				dist_y = RandomFloat(-1.0, 1.0);

				float rads = atan2(dist_y, dist_x) + RandomFloat(-1 * RandomNessFactor, RandomNessFactor);

				if (!(dist_x == 0 && dist_y == 0)) {
					new_x = bee_x + int(cos(rads) * MoveMax);
					new_y = bee_y + int(sin(rads) * MoveMax);

					if (new_x < 0 || new_x > MatrixLength) {
						bee_array_x[i] = bee_x - int(cos(rads) * MoveMax);
					}
					else {
						bee_array_x[i] = new_x;
					}

					if (new_y < 0 || new_y > MatrixHeight) {
						bee_array_y[i] = bee_y - int(sin(rads) * MoveMax);
					}
					else {
						bee_array_y[i] = new_y;
					}
				}
			}
		}

		delete[] bee_flower;
		delete[] flower_bee;
	}
	int getNumBees()
	{
		return num_bees;
	}
	int getNumFlowers()
	{
		return num_flowers;
	}
	int* getBeeCoords(int idx)
	{
		int temp[2];
		temp[0] = bee_array_x[idx];
		temp[1] = bee_array_y[idx];
		return temp;
	}
	int* getFlowerCoords(int idx)
	{
		int temp[2];
		temp[0] = flower_array_x[idx];
		temp[1] = flower_array_y[idx];
		return temp;
	}
};
