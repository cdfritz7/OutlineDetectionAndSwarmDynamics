#include <cstdlib>
#include <ctime>
#include <math.h>

const int MatrixHeight = 50;
const int MatrixLength = 50;
const int MoveMax = 5;
const int FlowerStrength = 3;
const int BeeStrength = 10;
const int RandomNessFactor = 3;
const int NumBees = 100;

class Bee {
	private:
		int x, y;
	public:
		Bee() {
			x = rand() % MatrixLength;
			y = rand() % MatrixHeight;
		}
		Bee(int x_loc, int y_loc) {
			if (x_loc == -1) {
				x = rand() % MatrixLength;
			}
			else {
				x = x_loc;
			}
			if (y_loc == -1) {
				y = rand() % MatrixHeight;
			}
			else {
				y = y_loc;
			}
		}
		void move(int x_loc, int y_loc) {
			x = x_loc;
			y = y_loc;
		}
		int getX() {
			return x;
		}
		int getY() {
			return y;
		}
};

class BeeHandle
{
	private:
		Bee bee_array[NumBees];
		int bee_matrix[MatrixLength][MatrixHeight];
		int flower_matrix[MatrixLength][MatrixHeight];
		int movement_matrix[MatrixLength][MatrixHeight];
		int bee_loc_x[NumBees];
		int bee_loc_y[NumBees];
	public:
		BeeHandle() {
			memset(bee_matrix, 0, sizeof(flower_matrix));
			memset(flower_matrix, 0, sizeof(flower_matrix));
			memset(movement_matrix, 0, sizeof(flower_matrix));
		};
		~BeeHandle() {};
		void add_flower(int x, int y) {
			for (int i = 0; i < FlowerStrength; i++) {
				int value = (int) pow(FlowerStrength - i, 2);
				if (i == 0) {
					flower_matrix[x][y] = value;
				}
				else {
					int fx;
					int fy;
					for (int j = 0; j < (2 * i + 1); j++) {
						fx = x - i;
						if (0 <= fx && 0 <= (y - i + j) && (y - i + j) <= MatrixHeight && flower_matrix[fx][y - i + j] < value) {
							flower_matrix[fx][y - i + j] = value;
						}
						fx = x + i;
						if (fx < MatrixLength && 0 <= (y - i + j) && (y - i + j) <= MatrixHeight && flower_matrix[fx][y - i + j] < value) {
							flower_matrix[fx][y - i + j] = value;
						}
						fy = y - i;
						if (0 <= fy && 0 <= (x - i + j) && (x - i + j) <= MatrixLength && flower_matrix[x - i + j][fy] < value) {
							flower_matrix[x - i + j][fy] = value;
						}
						fy = y + i;
						if (fy < MatrixHeight && 0 <= (x - i + j) && (x - i + j) <= MatrixLength && flower_matrix[x - i + j][fy] < value) {
							flower_matrix[x - i + j][fy] = value;
						}
					}
				}
			}
		}
		void update_bee_matrix() {
			memset(bee_matrix, 0, sizeof(bee_matrix));
			for (Bee &bee : bee_array) {
				int x = bee.getX();
				int y = bee.getY();
				for (int i = 0; i < BeeStrength; i++) {
					int value = (int) pow(BeeStrength - i, 2) * -1;
					if (i == 0) {
						bee_matrix[x][y] = value;
					}
					else {
						int fx;
						int fy;
						for (int j = 0; j < (2 * i + 1); j++) {
							fx = x - i;
							if (0 <= fx && 0 <= (y - i + j) && (y - i + j) <= MatrixHeight && bee_matrix[fx][y - i + j] < value) {
								bee_matrix[fx][y - i + j] = value;
							}
							fx = x + i;
							if (fx < MatrixLength && 0 <= (y - i + j) && (y - i + j) <= MatrixHeight && bee_matrix[fx][y - i + j] < value) {
								bee_matrix[fx][y - i + j] = value;
							}
							fy = y - i;
							if (0 <= fy && 0 <= (x - i + j) && (x - i + j) <= MatrixLength && bee_matrix[x - i + j][fy] < value) {
								bee_matrix[x - i + j][fy] = value;
							}
							fy = y + i;
							if (fy < MatrixHeight && 0 <= (x - i + j) && (x - i + j) <= MatrixLength && bee_matrix[x - i + j][fy] < value) {
								bee_matrix[x - i + j][fy] = value;
							}
						}
					}
				}
			}
		}
		void update_movement_matrix() {
			for (int y = 0; y < MatrixLength; y++) {
				for (int x = 0; x < MatrixHeight; x++) {
					movement_matrix[x][y] = bee_matrix[x][y] + flower_matrix[x][y];
				}
			}
		}
		void update_bees() {
			int x, y;
			int xmov_max, xmov_min;
			int ymov_max, ymov_min;
			int new_x, new_y;
			int current_value, new_value;
			for (Bee &bee : bee_array) {
				x = bee.getX();
				y = bee.getY();
				if (x + MoveMax >= MatrixLength) {
					xmov_max = MatrixLength - x - 1;
				}
				else {
					xmov_max = MoveMax;
				}
				if (x - MoveMax < 0) {
					xmov_min = -1 * x;
				}
				else {
					xmov_min = -1 * MoveMax;
				}
				if (y + MoveMax >= MatrixHeight) {
					ymov_max = MatrixHeight - y - 1;
				}
				else {
					ymov_max = MoveMax;
				}
				if (y - MoveMax < 0) {
					ymov_min = -1 * y;
				}
				else {
					ymov_min = -1 * MoveMax;
				}
			
				new_x = x + xmov_min + (rand() % (xmov_max - xmov_min));
				new_y = y + ymov_min + (rand() % (ymov_max - ymov_min));

				current_value = movement_matrix[x][y] + (int) pow(rand() % (2 * RandomNessFactor) - RandomNessFactor, 2);
				new_value = movement_matrix[new_x][new_y] + (int) pow(rand() % (2 * RandomNessFactor) - RandomNessFactor, 2);

				if (new_value > current_value) {
					bee.move(new_x, new_y);
				}
			}
		}
		void update_bee_coords() {
			for (int i = 0; i < NumBees; i++) {
				bee_loc_x[i] = bee_array[i].getX();
				bee_loc_y[i] = bee_array[i].getY();
			}
		}
		int* getBeeLocX() {
			return bee_loc_x;
		}
		int* getBeeLocY() {
			return bee_loc_y;
		}
};
