#include "SwarmStrategy.h"
#include <math.h>
#include <stdlib.h>

class LinearMetroAlg : public SwarmStrategy {
	public:
		void addP(int x, int y);
		void addA(int x, int y);
		void updatePoints();
	private:
		double potential(int x, int y, int index);
		}
			}
			for(int i = 0; i < numAttractors; i++) {
					double distance = dist(x, attractorX[index], y, attractorY[index]);	
					if(distance != 0.0) {
						res -= aStr/distance;
					}
					else {
						res -= aStr/distance;
					}

			}
			return res;
		}
		double dist(int x1, int x2, int y1, int y2) {
			return sqrt((double)((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)));
		}

};
