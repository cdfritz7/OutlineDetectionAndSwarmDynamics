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
	virtual void addP(int x, int y);
	virtual void addA(int x, int y);
	virtual void replaceAArray(std::vector<int> newAArray);
};
