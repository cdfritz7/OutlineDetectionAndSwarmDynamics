#pragma once
#include "SwarmStrategy.h"
#include <vector>
#include <opencv2/core/types.hpp>
#include <time.h>
#include <stdlib.h>

#define PI			3.14159265358979323846

class Attractor {
public:
	int pointIdx;
	int score;
	int x;
	int y;
	Attractor(int xcoord, int ycoord) {
		score = 0;
		x = xcoord;
		y = ycoord;
		pointIdx = -1;
	}
};

class BeeHandle : public SwarmStrategy {
private:
	double randomFactor;
	int numThreads;
	int storedFrames;
	double avgPercent;
	void movePoints();
public:
	BeeHandle(int xwidth = 400, int ywidth = 400, int stepsize = 10, double randomfactor = PI / 4, int numthreads = 4, int stored_frames = 3, double avg_percent = 0.5);
	void updatePoints();
	void addAttractorsAvg(std::vector<cv::Point> new_attractors);
};