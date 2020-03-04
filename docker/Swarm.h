#include <vector>
#include <math.h>
#include <random>
#include <cstdlib>
#include <thread>
#include <iostream>

// combines all of Brandon's swarm code into one header file

class SwarmStrategy {
protected:
	std::vector<int> pointX;
	std::vector<int> pointY;
	std::vector<int> attractorX;
	std::vector<int> attractorY;
	int xWidth;
	int yWidth;
	int stepSize;
public:
	void updatePoints() {};
	const std::vector<int> getPointX() {
		return pointX;
	}
	const std::vector<int> getPointY() {
		return pointY;
	}
	const std::vector<int> getAttractorArrayX() {
		return attractorX;
	}
	const std::vector<int> getAttractorArrayY() {
		return attractorY;
	}
	void addP(int x = -1, int y = -1) {
		if (x == -1) {
			x = rand() % xWidth;
		}
		if (y == -1) {
			y = rand() % yWidth;
		}
		pointX.push_back(x);
		pointY.push_back(y);
	};
	void addA(int x = -1, int y = -1) {
		if (x == -1) {
			x = rand() % xWidth;
		}
		if (y == -1) {
			y = rand() % yWidth;
		}
		pointX.push_back(x);
		pointY.push_back(y);
	};
	void replaceAArray(std::vector<int> attractorX, std::vector<int> attractorY) {
		this->attractorX = attractorX;
		this->attractorY = attractorY;
	};
};

float RandomFloat(float a, float b)
{
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}

class BeeHandle : public SwarmStrategy {
private:
	double randomFactor;
	int numThreads;
	static std::vector<int> paired_idx;

	void movePoints() {
		if (this->pointX.size() > this->attractorX.size()) {
			//points are candidates, flowers are picks
			for (int i = 0; i < pointX.size(); i++) {
				int flower = paired_idx[i];
				if (flower < attractorX.size()) {
					movePoint(i, flower);
				}
				else {
					movePoint(i);
				}
			}
		}
		else {
			//points are picks, flowers are candidates
			for (int i = 0; i < attractorX.size(); i++) {
				int bee = paired_idx[i];
				if (bee < pointX.size()) {
					movePoint(bee, i);
				}
			}
		}
	}

	void movePoint(int P_idx, int A_idx = -1) {
		int dist_x, dist_y;
		if (A_idx == -1) {
			//move randomly
			dist_x = RandomFloat(-1.0, 1.0);
			dist_y = RandomFloat(-1.0, 1.0);
		}
		else {
			dist_x = pointX[P_idx] - attractorX[A_idx];
			dist_y = pointY[P_idx] - attractorY[A_idx];
		}

		if (A_idx != -1) {
			int mag = pow(dist_x, 2) + pow(dist_y, 2);
			if (mag <= pow(stepSize, 2)) {
				//move bee to flower
				pointX[P_idx] = attractorX[A_idx];
				pointY[P_idx] = attractorY[A_idx];
				return;
			}
		}

		float rads = atan2(dist_y, dist_x) + RandomFloat(-1 * randomFactor, randomFactor);

		int new_x, new_y;

		if (!(dist_x == 0 && dist_y == 0)) {
			new_x = pointX[P_idx] + int(cos(rads) * stepSize);
			new_y = pointY[P_idx] + int(sin(rads) * stepSize);

			if (new_x < 0 || new_x > xWidth) {
				pointX[P_idx] = pointX[P_idx] - int(cos(rads) * stepSize);
			}
			else {
				pointX[P_idx] = new_x;
			}

			if (new_y < 0 || new_y > yWidth) {
				pointY[P_idx] = pointY[P_idx] - int(sin(rads) * stepSize);
			}
			else {
				pointY[P_idx] = new_y;
			}
		}
	}
public:
	BeeHandle(int xWidth = 400, int yWidth = 400, int stepSize = 10, double randomFactor = PI/4, int numThreads = 4) {
		this->xWidth = xWidth;
		this->yWidth = yWidth;
		this->stepSize = stepSize;
		this->randomFactor = randomFactor;
		this->numThreads = numThreads;
	}
	std::vector<int> getPaired_idx() {
		return paired_idx;
	}

	void addP(int x = -1, int y = -1) {
		if (x == -1) {
			x = rand() % xWidth;
		}
		if (y == -1) {
			y = rand() % yWidth;
		}
		pointX.push_back(x);
		pointY.push_back(y);
	}
	void addA(int x = -1, int y = -1) {
		if (x == -1) {
			x = rand() % xWidth;
		}
		if (y == -1) {
			y = rand() % yWidth;
		}
		attractorX.push_back(x);
		attractorY.push_back(y);
	}
	void replaceAArray(std::vector<int> attractorX, std::vector<int> attractorY) {
		this->attractorX = attractorX;
		this->attractorY = attractorY;
	}
	void updatePoints() {
		std::vector<std::thread> threads;
		std::vector<int> idx_array;
		std::vector<std::vector<int>> repeats;
		paired_idx.clear();

		std::vector<int> pickX;
		std::vector<int> pickY;
		std::vector<int> candidateX;
		std::vector<int> candidateY;

		if (this->pointX.size() > this->attractorX.size()) {
			candidateX = this->pointX;
			candidateY = this->pointY;
			pickX = this->attractorX;
			pickY = this->attractorY;
		}
		else {
			candidateX = this->attractorX;
			candidateY = this->attractorY;
			pickX = this->pointX;
			pickY = this->pointY;
		}

		// Set candidate to first pick in pick list
		for (int i = 0; i < candidateX.size(); i++) {
			paired_idx.push_back(0);
		}

		do {
			idx_array.clear();
			repeats.clear();

			// Initialize 2D vector
			for (int i = 0; i < (pickX.size() + 1); i++) {
				repeats.push_back(std::vector<int>());
			}

			// repeats[pick] contains all the candidates who picked pick
			for (int i = 0; i < (paired_idx.size()); i++) {
				repeats[paired_idx[i]].push_back(i);
			}

			// idx_array is the combination of all candidates with repeat picks
			for (int i = 0; i < (repeats.size() - 1); i++) {
				if (repeats[i].size() > 1) {
					//std::cout << "Repeats Array Size (" << i << "): " << repeats[i].size() << std::endl;
					for (int m = 0; m < repeats[i].size(); m++) {
						//std::cout << repeats[i][m] << std::endl;
					}
					idx_array.insert(idx_array.end(), repeats[i].begin(), repeats[i].end());
				}
			}

			//std::cout << "Idx Array Size: " << idx_array.size() << std::endl;

			for (int l = 0; l < idx_array.size(); l++) {
				//std::cout << idx_array[l] << std::endl;
			}

			int subSize = idx_array.size() / this->numThreads;
			int subRem = idx_array.size() % this->numThreads;

			std::vector<int> subVec;

			for (int i = 0; i < this->numThreads; i++) {
				int start;
				int end;
				// If there are less than 3 candidates in a thread, there is no point of multithreading
				if (idx_array.size() < (this->numThreads * 3)) {
					if (i < 1) {
						//std::cout << "case 1" << std::endl;
						//std::cout << pickX.size() << std::endl;

						// Run stable marriage with everything
						stableMarriage(idx_array, pickX, pickY, candidateX, candidateY);
					}
				}
				// Assign a subvector of idx_array to a thread
				else if (i < subRem){
					//if (i < 1) std::cout << "case 2" << std::endl;
					start = i * (subSize + 1);
					end = (i + 1) * (subSize + 1);

					// Create a thread
					threads.push_back(std::thread(stableMarriage, std::vector<int>(idx_array.cbegin() + start, idx_array.cbegin() + end) , pickX, pickY, candidateX, candidateY));
				}
				// Assign a subvector of idx_array to a thread
				else {
					//if (i < 1) std::cout << "case 3" << std::endl;
					start = subRem * (subSize + 1) + (i - subRem) * (subSize);
					end = subRem * (subSize + 1) + (i - subRem + 1) * (subSize);

					// Create a thread
					threads.push_back(std::thread(stableMarriage, std::vector<int>(idx_array.cbegin() + start, idx_array.cbegin() + end), pickX, pickY, candidateX, candidateY));
				}
			}

			for (int j = 0; j < threads.size(); j++) {
				threads[j].join();
			}

			threads.clear();
		} while (idx_array.size() > 0);

		movePoints();
	}

	void static stableMarriage(std::vector<int> idx_array, std::vector<int> pickX, std::vector<int> pickY, std::vector<int> candidateX, std::vector<int> candidateY) {
		//std::cout << "Running Stable Marrige" << std::endl;

		// No picks = no reason for stable marriage
		if (pickX.size() == 0) {
			std::cout << "No picks" << std::endl;
			return;
		}

		//std::cout << "Initializing threadRepeats" << std::endl;
		// Initialize 2D vector
		std::vector<std::vector<int>> threadRepeats;

		for (int i = 0; i < (pickX.size() + 1); i++) {
			threadRepeats.push_back(std::vector<int>());
		}

		// repeats[pick] contains all the candidates who picked pick (localized to the thread)
		for (int i = 0; i < idx_array.size(); i++) {
			threadRepeats[paired_idx[idx_array[i]]].push_back(idx_array[i]);
		}

		//std::cout << "Finished threadRepeats" << std::endl;

		int pick = 0;
		std::vector<int> competitors;

		int competitor;
		int competitor_mag;
		int closest_comp;
		int closest_mag = -1;

		//std::cout << "Starting Pick Loop" << std::endl;

		// for each group threadRepeats[pick] of size > 1
		while (pick < pickX.size()) {
			//std::cout << "Create competitors" << std::endl;
			competitors = threadRepeats[pick];

			// find the candidate that is closest to the pick, increment the pick for all others
			if (competitors.size() > 1) {
				closest_mag = -1;
				//std::cout << "Create competitors loop" << std::endl;
				for (int i = (competitors.size() - 1); i >= 0; i--) {
					competitor = competitors[i];

					//std::cout << "remove last candidate" << std::endl;
					// remove candidate from competitors list
					competitors.pop_back();

					//std::cout << "calc their mag" << std::endl;
					//std::cout << "Comp: " << competitor << std::endl;
					//std::cout << "Cand X: " << candidateX[competitor] << std::endl;
					//std::cout << "Cand Y: " << candidateY[competitor] << std::endl;
					competitor_mag = pow(candidateX[competitor] - pickX[pick], 2) + pow(candidateY[competitor] - pickY[pick], 2);

					//std::cout << "competitors cases" << std::endl;
					// no closest candidate yet
					if (closest_mag == -1) {
						//std::cout << "competitors case 1" << std::endl;
						closest_comp = competitor;
						closest_mag = competitor_mag;
					}
					// new candidate is closer than old closest candidate
					else if (competitor_mag < closest_mag) {
						//std::cout << "competitors case 2" << std::endl;
						threadRepeats[pick + 1].push_back(closest_comp);
						paired_idx[closest_comp]++;

						closest_comp = competitor;
						closest_mag = competitor_mag;
					}
					// new candidate is not closer that closest candidate
					else {
						//std::cout << "competitors case 3" << std::endl;
						threadRepeats[pick + 1].push_back(competitor);
						paired_idx[competitor]++;
					}
				}
				// Add closest candidate back for that pick
				threadRepeats[pick].push_back(closest_comp);
			}
			pick++;
		}
	}
};
