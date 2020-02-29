#include "SwarmStrategy.h"
#include <vector>

class BeeHandle : public SwarmStrategy {
private:
	double randomFactor;
	int numThreads;
	static std::vector<int> paired_idx;
	void movePoints();
	void movePoint(int P_idx, int A_idx = -1);
public:
	std::vector<int> getPaired_idx();
	void updatePoints();
	void static stableMarriage(std::vector<int> idx_array, std::vector<int> pickX, std::vector<int> pickY, std::vector<int> candidateX, std::vector<int> candidateY);	
};