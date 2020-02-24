#include "SwarmStrategy.h"
#include <math.h>
#include <random>
#include <cstdlib>
#include <thread>

#define PI 3.14159265358979323846

class BeeHandle : public SwarmStrategy {
private:
	double randomFactor;
	int numThreads;
	std::vector<int> candidateX;
	std::vector<int> candidateY;
	std::vector<int> pickX;
	std::vector<int> pickY;
public:
	BeeHandle(int xWidth = 400, int yWidth = 400, int stepSize = 10, double randomFactor = PI/4, int numThreads = 4) {
		this->xWidth = xWidth;
		this->yWidth = yWidth;
		this->stepSize = stepSize;
		this->randomFactor = randomFactor;
		this->numThreads = numThreads;
		if (this->pointX.size() > this->attractorX.size()) {
			this->candidateX = this->pointX;
			this->candidateY = this->pointY;
			this->pickX = this->attractorX;
			this->pickY = this->attractorY;
		}
		else {
			this->candidateX = this->attractorX;
			this->candidateY = this->attractorY;
			this->pickX = this->pointX;
			this->pickY = this->pointY;
		}
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
		std::vector<int> paired_idx;
		std::vector<std::vector<int>> repeats;

		// Set candidate to first pick in pick list
		for (int i = 0; i < candidateX.size(); i++) {
			paired_idx.push_back(0);
		}

		do {
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
					idx_array.insert(idx_array.end(), repeats[i].begin(), repeats[i].end());
				}
			}

			int subSize = idx_array.size() / this->numThreads;
			int subRem = idx_array.size() % this->numThreads;
			
			std::vector<int> subVec;

			for (int i = 0; i < this->numThreads; i++) {
				int start;
				int end;
				// If there are less than 3 candidates in a thread, there is no point of multithreading
				if (idx_array.size() < (this->numThreads * 3)) {
					start = 0;
					end = idx_array.size();
					if (i < 1) {
						// create thread (once)
					}
				}
				// Assign a subvector of idx_array to a thread
				else if (i < subRem){
					start = i * (subSize + 1);
					end = (i + 1) * (subSize + 1);
					// create thread
				}
				// Assign a subvector of idx_array to a thread
				else {
					start = subRem * (subSize + 1) + (i - subRem) * (subSize);
					end = subRem * (subSize + 1) + (i - subRem + 1) * (subSize);
					// create thread
				}
			}

			// TODO: delete this when working
			idx_array.clear();
		} while (idx_array.size() > 0);
	}

	void stableMarriage(std::vector<int> idx_array, std::vector<int>& paired_idx) {
		// No picks = no reason for stable marriage
		if (pickX.size() == 0) {
			return;
		}

		// Initialize 2D vector 
		std::vector<std::vector<int>> threadRepeats;
		for (int i = 0; i < (pickX.size() + 1); i++) {
			threadRepeats.push_back(std::vector<int>());
		}

		// repeats[pick] contains all the candidates who picked pick (localized to the thread)
		for (int i = 0; i < idx_array.size(); i++) {
			threadRepeats[paired_idx[i]].push_back(i);
		}

		int pick = 0;
		std::vector<int> competitors;

		int competitor;
		int competitor_mag;
		int closest_comp;
		int closest_mag = -1;

		// for each group threadRepeats[pick] of size > 1
		while (pick < this->pickX.size()) {
			competitors = threadRepeats[pick];

			// find the candidate that is closest to the pick, increment the pick for all others
			if (competitors.size() > 1) {
				closest_mag = -1;
				for (int i = (competitors.size() - 1); i >= 0; i--) {
					competitor = competitors[i];

					// remove candidate from competitors list
					competitors.pop_back();

					competitor_mag = pow(candidateX[competitor] - pickX[pick], 2) + pow(candidateY[competitor] - pickY[pick], 2);

					// no closest candidate yet
					if (closest_mag == -1) {
						closest_comp = competitor;
						closest_mag = competitor_mag;
					}
					// new candidate is closer than old closest candidate
					else if (competitor_mag < closest_mag) {
						threadRepeats[pick + 1].push_back(closest_comp);
						paired_idx[closest_comp]++;
						
						closest_comp = competitor;
						closest_mag = competitor_mag;
					}
					// new candidate is not closer that closest candidate
					else {
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