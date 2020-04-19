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

static vector<vector<Attractor>> attractorMatrix;
static vector<vector<cv::Point>> attractorHistory;
static vector<cv::Point> staticPoints;

static void UpdateAttractorMatrix(int start_idx, int end_idx, int avgPercent, int storedFrames) {
	for (int P_idx = start_idx; P_idx < end_idx; P_idx++) {
		printf("UpdateAttractorMatrix %d: %d\n", P_idx, attractorMatrix[staticPoints[P_idx].x][staticPoints[P_idx].y].score);
		printf("Score Requirement: %d\n", avgPercent * storedFrames);
		// If the position they are at has no other point and there is an attractor there with a good enough score
		if (attractorMatrix[staticPoints[P_idx].x][staticPoints[P_idx].y].pointIdx == -1 && attractorMatrix[staticPoints[P_idx].x][staticPoints[P_idx].y].score > (avgPercent * attractorHistory.size())) {
			// Save the bee to that position, don't move the bee
			attractorMatrix[staticPoints[P_idx].x][staticPoints[P_idx].y].pointIdx = P_idx;
		}
	}
}

static void movePoint(int start_idx, int end_idx, int randomFactor, int stepSize, int xWidth, int yWidth) {
	for (int P_idx = start_idx; P_idx < end_idx; P_idx++) {
		if (attractorMatrix[staticPoints[P_idx].x][staticPoints[P_idx].y].pointIdx != P_idx) {
			float dist_x, dist_y;

			//move randomly
			dist_x = RandomFloat(-PI, PI);
			dist_y = RandomFloat(-PI, PI);

			printf("Dist: (%f, %f)\n", dist_x, dist_y);

			float rads = atan2(dist_y, dist_x) + RandomFloat(-1 * randomFactor, randomFactor);

			printf("Rads: %f\n", rads);

			int new_x, new_y;

			if (!(dist_x == 0 && dist_y == 0)) {
				new_x = staticPoints[P_idx].x + int(cos(rads) * stepSize);
				new_y = staticPoints[P_idx].y + int(sin(rads) * stepSize);

				if (new_x < 0 || new_x > xWidth) {
					staticPoints[P_idx].x = staticPoints[P_idx].x - int(cos(rads) * stepSize);
				}
				else {
					staticPoints[P_idx].x = new_x;
				}

				if (new_y < 0 || new_y > yWidth) {
					staticPoints[P_idx].y = staticPoints[P_idx].y - int(sin(rads) * stepSize);
				}
				else {
					staticPoints[P_idx].y = new_y;
				}
			}
		}
	}
}

static void removeLastFrame(int start_idx, int end_idx) {
	printf("Last Frame Removed\n");
	for (int i = start_idx; i < end_idx; i++) {
		attractorMatrix[attractorHistory[0][i].x][attractorHistory[0][i].y].score = attractorMatrix[attractorHistory[0][i].x][attractorHistory[0][i].y].score - 1;
	}
	attractorHistory.erase(attractorHistory.begin());
}

BeeHandle::BeeHandle(int xwidth, int ywidth, int stepsize, double randomfactor, int numthreads, int stored_frames, double avg_percent) {
	xWidth = xwidth;
	yWidth = ywidth;
	stepSize = stepsize;
	randomFactor = randomfactor;
	numThreads = numthreads;
	avgPercent = avg_percent;
	storedFrames = stored_frames;
	attractorMatrix = vector<vector<Attractor>>(xWidth, vector<Attractor>(yWidth, Attractor(0, 0)));
	for (int i = 0; i < attractorMatrix.size(); i++) {
		for (int j = 0; j < attractorMatrix[0].size(); j++) {
			attractorMatrix[i][j].x = i;
			attractorMatrix[i][j].y = j;
		}
	}
	attractorHistory.clear();
	staticPoints.clear();
	srand(time(0));
}

void BeeHandle::movePoints() {
	for (int P_idx = 0; P_idx < points.size(); P_idx++) {
		if (attractorMatrix[points[P_idx].x][points[P_idx].y].pointIdx != P_idx) {
			float dist_x, dist_y;

			//move randomly
			dist_x = RandomFloat(-PI, PI);
			dist_y = RandomFloat(-PI, PI);

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
		}
	}
}

void BeeHandle::updatePoints() {
	vector<thread> threads_update;
	vector<thread> threads_move;

	int subSize = points.size() / this->numThreads;
	int subRem = points.size() % this->numThreads;

	staticPoints = points;

	for (int i = 0; i < this->numThreads; i++) {
		int start;
		int end;
		// If there are less than 3 candidates in a thread, there is no point of multithreading
		if (points.size() < numThreads) {
			if (i < points.size()) {
				start = i;
				end = i + 1;
				//UpdateAttractorMatrix(points, start, end, avgPercent, storedFrames);
				threads_update.push_back(thread(UpdateAttractorMatrix, start, end, avgPercent, storedFrames));
			}
		}
		// Assign a subvector of idx_array to a thread
		else if (i < subRem) {
			//if (i < 1) std::cout << "case 2" << std::endl;
			start = i * (subSize + 1);
			end = (i + 1) * (subSize + 1);
			//UpdateAttractorMatrix(points, start, end, avgPercent, storedFrames);
			threads_update.push_back(thread(UpdateAttractorMatrix, start, end, avgPercent, storedFrames));
		}
		// Assign a subvector of idx_array to a thread
		else {
			//if (i < 1) std::cout << "case 3" << std::endl;
			start = subRem * (subSize + 1) + (i - subRem) * (subSize);
			end = subRem * (subSize + 1) + (i - subRem + 1) * (subSize);
			//UpdateAttractorMatrix(points, start, end, avgPercent, storedFrames);
			threads_update.push_back(thread(UpdateAttractorMatrix, start, end, avgPercent, storedFrames));
		}
	}

	for (int j = 0; j < threads_update.size(); j++) {
		threads_update[j].join();
	}

	threads_update.clear();

	/*
	for (int i = 0; i < this->numThreads; i++) {
		int start;
		int end;
		// If there are less than 3 candidates in a thread, there is no point of multithreading
		if (points.size() < numThreads) {
			if (i < points.size()) {
				start = i;
				end = i + 1;
				//UpdateAttractorMatrix(points, start, end, avgPercent, storedFrames);
				threads_move.push_back(thread(movePoint, start, end, randomFactor, stepSize, xWidth, yWidth));
			}
		}
		// Assign a subvector of idx_array to a thread
		else if (i < subRem) {
			//if (i < 1) std::cout << "case 2" << std::endl;
			start = i * (subSize + 1);
			end = (i + 1) * (subSize + 1);
			//UpdateAttractorMatrix(points, start, end, avgPercent, storedFrames);
			threads_move.push_back(thread(movePoint, start, end, randomFactor, stepSize, xWidth, yWidth));
		}
		// Assign a subvector of idx_array to a thread
		else {
			//if (i < 1) std::cout << "case 3" << std::endl;
			start = subRem * (subSize + 1) + (i - subRem) * (subSize);
			end = subRem * (subSize + 1) + (i - subRem + 1) * (subSize);
			//UpdateAttractorMatrix(points, start, end, avgPercent, storedFrames);
			threads_move.push_back(thread(movePoint, start, end, randomFactor, stepSize, xWidth, yWidth));
		}
	}
	

	for (int j = 0; j < threads_move.size(); j++) {
		threads_move[j].join();
	}

	threads_move.clear();
	*/

	points = staticPoints;

	movePoints();
}

void BeeHandle::addAttractorsAvg(vector<cv::Point> new_attractors) {

	vector<vector<int>> temp(xWidth, vector<int> (yWidth, 0));

	// Remove duplicates
	// Cannot be done in parallel - O(f)
	for (int i = new_attractors.size()-1; i >= 0; i--) {
		// If a contour in that position already exists, remove
		if (temp[new_attractors[i].x][new_attractors[i].y] == 1) {
			new_attractors.erase(new_attractors.begin() + i);
		}
		// If a contour in that position doesn't exist, increment attractorMatrix in that position and add reference to the position to attractorRefs
		else {
			temp[new_attractors[i].x][new_attractors[i].y] = 1;
			attractorMatrix[new_attractors[i].x][new_attractors[i].y].score = attractorMatrix[new_attractors[i].x][new_attractors[i].y].score + 1;
		}
	}

	//Now that dups are removed, add to history
	attractorHistory.push_back(new_attractors);

	// Remove expired frame
	// Can be done in parallel (no duplicates) - O(f/t)
	/*
	if (attractorHistory.size() > storedFrames){
		for (int i = 0; i < attractorHistory[0].size(); i++) {
			attractorMatrix[attractorHistory[0][i].x][attractorHistory[0][i].y].score = attractorMatrix[attractorHistory[0][i].x][attractorHistory[0][i].y].score - 1;
		}
		attractorHistory.erase(attractorHistory.begin());
	}*/


	if (attractorHistory.size() > storedFrames) {
		printf("There is a Last Frame to remove\n");
		vector<thread> threads;
		int subSize = attractorHistory[0].size() / this->numThreads;
		int subRem = attractorHistory[0].size() % this->numThreads;

		for (int i = 0; i < this->numThreads; i++) {
			int start;
			int end;
			// If there are less than 3 candidates in a thread, there is no point of multithreading
			if (points.size() < numThreads) {
				if (i < points.size()) {
					start = i;
					end = i + 1;
					//UpdateAttractorMatrix(points, start, end, avgPercent, storedFrames);
					threads.push_back(thread(removeLastFrame, start, end));
				}
			}
			// Assign a subvector of idx_array to a thread
			else if (i < subRem) {
				//if (i < 1) std::cout << "case 2" << std::endl;
				start = i * (subSize + 1);
				end = (i + 1) * (subSize + 1);
				//UpdateAttractorMatrix(points, start, end, avgPercent, storedFrames);
				threads.push_back(thread(removeLastFrame, start, end));
			}
			// Assign a subvector of idx_array to a thread
			else {
				//if (i < 1) std::cout << "case 3" << std::endl;
				start = subRem * (subSize + 1) + (i - subRem) * (subSize);
				end = subRem * (subSize + 1) + (i - subRem + 1) * (subSize);
				//UpdateAttractorMatrix(points, start, end, avgPercent, storedFrames);
				threads.push_back(thread(removeLastFrame, start, end));
			}
		}

		for (int j = 0; j < threads.size(); j++) {
			threads[j].join();
		}

		threads.clear();
	}
}