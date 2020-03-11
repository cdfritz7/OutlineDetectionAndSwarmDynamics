#pragma once
#include "SwarmStrategy.h"
#include <vector>
#include <opencv2/core/types.hpp>

#define PI			3.14159265358979323846

class BeeHandle : public SwarmStrategy {
private:
	bool updateReady;
	bool next_updated;
	double percent_landed;
	double randomFactor;
	int numThreads;
	void movePoints();
	int movePoint(int P_idx, int A_idx = -1);
	std::vector<cv::Point> next_attractors;
public:
	BeeHandle(int xwidth = 400, int ywidth = 400, int stepsize = 10, double randomfactor = PI/4, int numthreads = 4, double landing_percent = 0.0);
	void updatePoints();
	std::vector<int> getPairedIdx();
	void safeReplaceAttractors(std::vector<cv::Point> new_attractors);
};