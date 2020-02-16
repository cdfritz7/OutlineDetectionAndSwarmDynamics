#ifndef LINEARMETROALG_H
#define LINEARMETROALG_H

#include "SwarmStrategy.h"
#include <math.h>
#include <stdlib.h>

#define DEFAULT_POINT_ARRAY_SIZE 2000
#define DEFAULT_ATTR_ARRAY_SIZE 1000

class LinearMetroAlg : public SwarmStrategy {
	public:
		void addP(int x, int y) {
      if(numPoints < DEFAULT_POINT_ARRAY_SIZE) {
				pointX[numPoints] = x;
      	pointY[numPoints] = y;
      	numPoints++;
			}
    }
    void addA(int x, int y) {
      if(numAttractors < DEFAULT_ATTR_ARRAY_SIZE) {
				attractorX[numAttractors] = x;
      	attractorY[numAttractors] = y;
      	numAttractors++;
			}
    }
		LinearMetroAlg() {
			pointX = new int[DEFAULT_POINT_ARRAY_SIZE];
			pointY = new int[DEFAULT_POINT_ARRAY_SIZE];
			attractorX = new int[DEFAULT_ATTR_ARRAY_SIZE];
			attractorY = new int[DEFAULT_ATTR_ARRAY_SIZE];
		}
		~LinearMetroAlg() {
			delete pointX;
			delete pointY;
			delete attractorX;
			delete attractorY;
		}

    // TODO have different step sizes
    // TODO let particles increase their potential
    void updatePoints() {
      for(int i = 0; i < numPoints; i++) {
        int newX = pointX[i] + ((rand() % 3) - 1);
        int newY = pointY[i] + ((rand() % 3) - 1);
        double currPotential = potential(pointX[i], pointY[i], i);
        double newPotential = potential(newX, newY, i);
        if(newPotential < currPotential) {
          pointX[i] = newX;
          pointY[i] = newY;
        }
      }
    }
	private:
		double potential(int x, int y, int index) {
      double res = 0.0;
      for(int i = 0; i < numPoints; i++) {
        if(i != index) {
          double distance = dist(x, pointX[i], y, pointY[i]);
          if(distance != 0.0) {
            res += pStr/distance;
          }
          else {
            res += pStr/0.0001;
          }
        }
      }
      for(int i = 0; i < numAttractors; i++) {
          double distance = dist(x, attractorX[index], y, attractorY[index]);
          if(distance != 0.0) {
            res -= aStr/distance;
          }
          else {
            res -= aStr/0.0001;
          }

      }
      return res;
    }
    double dist(int x1, int x2, int y1, int y2) {
      return sqrt((double)((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)));
    }
};

#endif

