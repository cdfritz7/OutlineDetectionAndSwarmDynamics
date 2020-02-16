#ifndef SWARMSTRATEGY_H
#define SWARMSTRATEGY_H

class SwarmStrategy {
  protected:
    int* pointX;
    int* pointY;
    int* attractorX;
    int* attractorY;
    int xWidth;
    int yWidth;
    int pCapacity;
		int aCapacity;
		int stepSize;
		int pStr;
		int aStr;
		int numPoints;
		int numAttractors;
	public:
		virtual void updatePoints() {}
		const int getPointX(int index) {
			return pointX[index];
		}
		const int getPointY(int index) {
			return pointY[index];
		}
		const int getAttractorX(int index) {
			return attractorX[index];
		}
		const int getAttractorY(int index) {
			return attractorY[index];
		}
		const int getNumPoints() {
			return numPoints;
		}
		const int getNumAttractors() {
			return numAttractors;
		}
		virtual void addP(int x, int y) {}
		virtual void addA(int x, int y) {}
		virtual ~SwarmStrategy() {}
};

#endif
