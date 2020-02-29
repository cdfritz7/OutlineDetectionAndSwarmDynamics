#include <vector>
#include <random>

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
