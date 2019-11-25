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
const int RandomNessFactor = M_PI/8;
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
	int* bee_flower;
	int* flower_array_x;
	int* flower_array_y;
	int* flower_bee;
	int num_flowers;
	int num_bees;
public:
	BeeHandle() 
	{
		bee_array_x = new int[MatrixHeight * MatrixLength];
		bee_array_y = new int[MatrixHeight * MatrixLength];
		flower_array_x = new int[MatrixHeight * MatrixLength];
		flower_array_y = new int[MatrixHeight * MatrixLength];
		bee_flower = new int[MatrixHeight * MatrixLength];
		flower_bee = new int[MatrixHeight * MatrixLength];
		num_flowers = 0;
		num_bees = 0;
	};
	~BeeHandle() 
	{
		delete[] bee_array_x;
		delete[] bee_array_y;
		delete[] flower_array_x;
		delete[] flower_array_y;
		delete[] bee_flower;
		delete[] flower_bee;
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
			bee_flower[num_bees] = -1;
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
			flower_bee[num_flowers] = -1;
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
		for (int i = 0; i < num_bees; i++) 
		{
			int bee_x = bee_array_x[i];
			int bee_y = bee_array_y[i];

			if (bee_flower[i] == -1) 
			{
				int total_x = 0;
				int total_y = 0;

				for (int j = 0; j < num_bees; j++) 
				{
					if (j != i && bee_flower[j] == -1) 
					{
						int bee2_x = bee_array_x[j];
						int bee2_y = bee_array_y[j];
						int dist_x = bee2_x - bee_x;
						int dist_y = bee2_y - bee_y;

						float dist = sqrt(pow(dist_x, 2) + pow(dist_y, 2));
						dist = pow((MatrixMaxMag - dist), 2);

						total_x -= dist_x * dist * (BeeStrength / FlowerStrength);
						total_y -= dist_y * dist * (BeeStrength / FlowerStrength);
					}
				}

				int flower_in_range = -1;
				int flower_dist = -1;
				int num_empty_flowers = 0;

				for (int j = 0; j < num_flowers; j++) 
				{
					if (flower_bee[j] == -1) 
					{
						num_empty_flowers++;
						int flower_x = flower_array_x[j];
						int flower_y = flower_array_y[j];
						int dist_x = flower_x - bee_x;
						int dist_y = flower_y - bee_y;

						float dist = sqrt(pow(dist_x, 2) + pow(dist_y, 2));

						if (dist < MoveMax && (flower_in_range < 0 || dist < flower_dist)) 
						{
							flower_in_range = j;
							flower_dist = j;
						}

						dist = pow((MatrixMaxMag - dist), 2);

						total_x += dist_x * dist * (BeeStrength / FlowerStrength);
						total_y += dist_y * dist * (BeeStrength / FlowerStrength);
					}
				}

				if (num_empty_flowers == 0) 
				{
					total_x = RandomFloat(-1.0, 1.0);
					total_y = RandomFloat(-1.0, 1.0); 
				}

				if (flower_in_range < 0) 
				{
					float rads = atan2(total_y, total_x) + RandomFloat(-1 * RandomNessFactor, RandomNessFactor);
					
					if (!(total_x == 0 && total_y == 0)) 
					{
						int new_x = bee_x + int(cos(rads) * RandomFloat(0, (float)MoveMax));
						int new_y = bee_y + int(sin(rads) * RandomFloat(0, (float)MoveMax));

						if (new_x < 0 or new_x >= MatrixLength)
						{
							bee_array_x[i] = bee_x - int(cos(rads) * RandomFloat(0, (float)MoveMax));
						}
						else
						{
							bee_array_x[i] = new_x;
						}
						
						if (new_y < 0 or new_y >= MatrixHeight) 
						{
							bee_array_y[i] = bee_y + int(sin(rads) * RandomFloat(0, (float)MoveMax));
						}
						else {
							bee_array_y[i] = new_y;
						}
					}
				}
				else {
					bee_flower[i] = flower_in_range;
					flower_bee[flower_in_range] = i;
					bee_array_x[i] = flower_array_x[flower_in_range];
					bee_array_y[i] = flower_array_y[flower_in_range];
				}
			}
			else {
				int flower_in_range = -1;
				int flower_dist = -1;

				for (int j = 0; j < num_flowers; j++) 
				{
					if (flower_bee[j] == -1) 
					{
						int flower_x = flower_array_x[j];
						int flower_y = flower_array_y[j];
						int dist_x = flower_x - bee_x;
						int dist_y = flower_y - bee_y;

						float dist = sqrt(pow(dist_x, 2) + pow(dist_y, 2));

						if (dist < MoveMax && (flower_in_range < 0 || dist < flower_dist)) 
						{
							flower_in_range = j;
							flower_dist = j;
						}
					}

					if (flower_in_range != -1) {
						bee_flower[i] = flower_in_range;
						flower_bee[flower_in_range] = i;
						bee_array_x[i] = flower_array_x[flower_in_range];
						bee_array_y[i] = flower_array_y[flower_in_range];
					}
				}
			}
		}
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
