#pragma once
#include "SwarmStrategy.h"
#include <vector>
#include <opencv2/core/types.hpp>

#define PI			3.14159265358979323846

class BeeHandle : public SwarmStrategy {
private:
	double randomFactor;
	int numThreads;
	void movePoints();
	void movePoint(int P_idx, int A_idx = -1);
public:
	BeeHandle(int xwidth = 400, int ywidth = 400, int stepsize = 10, double randomfactor = PI/4, int numthreads = 4);
	void updatePoints();
	std::vector<int> getPairedIdx();
};