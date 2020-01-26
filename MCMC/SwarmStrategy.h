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
		virtual void updatePoints();
		const int* getPointX(int index);
		const int* getPointY(int index);
		const int* getAttractorArrayX();
		const int* getAttractorArrayY();
		const int getNumPoints();
		const int getNumAttractors();
		virtual void addP(int x, int y);
		virtual void addA(int x, int y);
};
