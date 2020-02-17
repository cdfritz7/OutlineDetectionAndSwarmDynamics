#include <vector>

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
	virtual void updatePoints();
	const std::vector<int> getPointArrayX();
	const std::vector<int> getPointArrayY();
	const std::vector<int> getAttractorArrayX();
	const std::vector<int> getAttractorArrayY();
	virtual void addP(int x = -1, int y = -1);
	virtual void addA(int x = -1, int y = -1);
	virtual void replaceAArray(std::vector<int> newAArray);
};
