#include <vector>
#include <iostream>
#include <opencv.hpp>

class SwarmStrategy {
protected:
	std::vector<cv::Point> points;
	std::vector<cv::Point> attractors;
	int xWidth;
	int yWidth;
	int stepSize;
public:
	virtual void updatePoints() = 0;
	const std::vector<cv::Point> getPoints() {
		return points;
	};
	const std::vector<cv::Point> getAttractors() {
		return attractors;
	};
	const void addP(cv::Point point) {
		points.push_back(point);
	};
	const void addA(cv::Point point) {
		attractors.push_back(point);
	};
	const void addP(int x = -1, int y = -1) {
		if (x == -1) {
			x = rand() % xWidth;
		}
		if (y == -1) {
			y = rand() % yWidth;
		}
		cv::Point newPoint = cv::Point();
		newPoint.x = x;
		newPoint.y = y;

		points.push_back(newPoint);
	};
	const void addA(int x = -1, int y = -1) {
		if (x == -1) {
			x = rand() % xWidth;
		}
		if (y == -1) {
			y = rand() % yWidth;
		}
		cv::Point newPoint = cv::Point();
		newPoint.x = x;
		newPoint.y = y;

		attractors.push_back(newPoint);
	};
	const void replaceAArray(std::vector<cv::Point> attractors) {
		this->attractors.clear();
		for (int i = 0; i < attractors.size(); i++){
			this->attractors.push_back(attractors[i]);
		}
	};
};
