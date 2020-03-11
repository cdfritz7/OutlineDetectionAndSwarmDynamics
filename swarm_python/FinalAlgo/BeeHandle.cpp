#include "BeeHandle.h"
#include <iostream>
#include <thread>

using namespace std;

float RandomFloat(float a, float b)
{
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}

static std::vector<int> paired_idx;

void static stableMarriage(std::vector<int> idx_array, std::vector<cv::Point> picks, std::vector<cv::Point> candidates) {
	// No picks = no reason for stable marriage
	if (picks.size() == 0) {
		std::cout << "No picks" << std::endl;
		return;
	}

	// Initialize 2D vector 
	std::vector<std::vector<int>> threadRepeats;

	for (int i = 0; i < (picks.size() + 1); i++) {
		threadRepeats.push_back(std::vector<int>());
	}

	// repeats[pick] contains all the candidates who picked pick (localized to the thread)
	for (int i = 0; i < idx_array.size(); i++) {
		threadRepeats[paired_idx[idx_array[i]]].push_back(idx_array[i]);
	}

	int pick = 0;
	std::vector<int> competitors;

	int competitor;
	int competitor_mag;
	int closest_comp;
	int closest_mag = -1;

	// for each group threadRepeats[pick] of size > 1
	while (pick < picks.size()) {
		competitors = threadRepeats[pick];

		// find the candidate that is closest to the pick, increment the pick for all others
		if (competitors.size() > 1) {
			closest_mag = -1;
			for (int i = (competitors.size() - 1); i >= 0; i--) {
				competitor = competitors[i];

				// remove candidate from competitors list
				competitors.pop_back();

				competitor_mag = pow(candidates[competitor].x - picks[pick].y, 2) + pow(candidates[competitor].x - picks[pick].y, 2);

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

void BeeHandle::movePoints() {
	int num_landed = 0;
	if (this->points.size() > this->attractors.size()) {
		//points are candidates, flowers are picks
		for (int i = 0; i < points.size(); i++) {
			int flower = paired_idx[i];
			if (flower < attractors.size()) {
				num_landed += movePoint(i, flower);
			}
			else {
				movePoint(i);
			}
		}
		if (((double)num_landed / (double)attractors.size()) > percent_landed) {
			updateReady = true;
			if (next_updated) {
				attractors = next_attractors;
				next_updated = false;
			}
		}
	}
	else {
		//points are picks, flowers are candidates
		for (int i = 0; i < attractors.size(); i++) {
			int bee = paired_idx[i];
			if (bee < points.size()) {
				num_landed += movePoint(bee, i);
			}
		}
		if (((double)num_landed / (double)points.size()) > percent_landed) {
			updateReady = true;
			if (next_updated) {
				attractors = next_attractors;
				next_updated = false;
			}
		}
	}
}
int BeeHandle::movePoint(int P_idx, int A_idx) {
	int dist_x, dist_y;
	if (A_idx == -1) {
		//move randomly
		dist_x = RandomFloat(-1.0, 1.0);
		dist_y = RandomFloat(-1.0, 1.0);
	}
	else {
		dist_x = attractors[A_idx].x - points[P_idx].x;
		dist_y = attractors[A_idx].y - points[P_idx].y;
	}

	if (A_idx != -1) {
		int mag = pow(dist_x, 2) + pow(dist_y, 2);
		if (mag <= pow(stepSize, 2)) {
			//move bee to flower
			points[P_idx].x = attractors[A_idx].x;
			points[P_idx].y = attractors[A_idx].y;
			return 1;
		}
	}

	float rads = atan2(dist_y, dist_x) + RandomFloat(-1 * randomFactor, randomFactor);

	int new_x, new_y;

	if (!(dist_x == 0 && dist_y == 0)) {
		new_x = points[P_idx].x + int(cos(rads) * stepSize);
		new_y = points[P_idx].y + int(sin(rads) * stepSize);

		if (new_x < 0 || new_x > xWidth) {
			points[P_idx].x = points[P_idx].x - int(cos(rads) * stepSize);
		}
		else {
			points[P_idx].x = new_x;
		}

		if (new_y < 0 || new_y > yWidth) {
			points[P_idx].y = points[P_idx].y - int(sin(rads) * stepSize);
		}
		else {
			points[P_idx].y = new_y;
		}
	}
	return 0;
}

BeeHandle::BeeHandle(int xwidth, int ywidth, int stepsize, double randomfactor, int numthreads, double landing_percent) {
	xWidth = xwidth;
	yWidth = ywidth;
	stepSize = stepsize;
	randomFactor = randomfactor;
	numThreads = numthreads;
	paired_idx = std::vector<int>();
	updateReady = true;
	next_updated = false;
	percent_landed = landing_percent;
	next_attractors.clear();
}

void BeeHandle::updatePoints() {
	if (updateReady) {
		updateReady = false;
		std::vector<std::thread> threads;
		std::vector<int> idx_array;
		std::vector<std::vector<int>> repeats;
		paired_idx.clear();

		std::vector<cv::Point> picks;
		std::vector<cv::Point> candidates;

		if (this->points.size() > this->attractors.size()) {
			candidates = this->points;
			picks = this->attractors;
		}
		else {
			candidates = this->attractors;
			picks = this->points;
		}

		// Set candidate to first pick in pick list
		for (int i = 0; i < candidates.size(); i++) {
			paired_idx.push_back(0);
		}

		do {
			idx_array.clear();
			repeats.clear();

			// Initialize 2D vector
			for (int i = 0; i < (picks.size() + 1); i++) {
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
					if (i < 1) {
						// Run stable marriage with everything
						stableMarriage(idx_array, picks, candidates);
					}
				}
				// Assign a subvector of idx_array to a thread
				else if (i < subRem) {
					//if (i < 1) std::cout << "case 2" << std::endl;
					start = i * (subSize + 1);
					end = (i + 1) * (subSize + 1);

					// Create a thread
					threads.push_back(std::thread(stableMarriage, std::vector<int>(idx_array.cbegin() + start, idx_array.cbegin() + end), picks, candidates));
				}
				// Assign a subvector of idx_array to a thread
				else {
					//if (i < 1) std::cout << "case 3" << std::endl;
					start = subRem * (subSize + 1) + (i - subRem) * (subSize);
					end = subRem * (subSize + 1) + (i - subRem + 1) * (subSize);

					// Create a thread
					threads.push_back(std::thread(stableMarriage, std::vector<int>(idx_array.cbegin() + start, idx_array.cbegin() + end), picks, candidates));
				}
			}

			for (int j = 0; j < threads.size(); j++) {
				threads[j].join();
			}

			threads.clear();
		} while (idx_array.size() > 0);
	}
	
	movePoints();
}

std::vector<int> BeeHandle::getPairedIdx() {
	return paired_idx;
}

void BeeHandle::safeReplaceAttractors(std::vector<cv::Point> new_attractors) {
	if (updateReady) {
		attractors = new_attractors;
	}
	else {
		next_attractors = new_attractors;
		next_updated = true;
	}
}