#include <iostream>
#include "SwarmStrategy.h"
#include "LinearMetroAlg.h"
#include <stdlib.h>
#include <time.h>
#include <chrono>

// compile with: g++ -o bin/a.out -Wall -std=c++11 SwarmStrategy.h LinearMetroAlg.h main.cpp 
using namespace std;
//using std::cout;
//using std::endl;
//using std::chrono;
//using chrono::duration_cast;
//using chrono::steady_clock::now;
//using chrono::milliseconds;

int main() {
	std::cout << "start\n";
	SwarmStrategy *swarm = new LinearMetroAlg();
	
	int screenWidth = 200;
	for(int i=0; i<screenWidth; i++) {
		swarm -> addA(i,  i);
	}

	/*for(int i=0; i<screenWidth; i+=(screenWidth/20)) {
		for(int j=0; j<screenWidth; j+=(screenWidth/20)) {
			swarm -> addP(i, j);
		}
	}*/
	for(int i=0; i<700; i++) {
		swarm -> addP(rand() % screenWidth, rand() % screenWidth);
	}
	
	cout << "numPoints: " << swarm -> getNumPoints() << endl;
	cout << "numAttractors: " << swarm -> getNumAttractors() << endl;

	auto start = chrono::steady_clock::now();
	int numFrames = 30 * 10;
	for(int frame=0; frame<numFrames; frame++) {
		swarm -> updatePoints();
	}
	auto end = chrono::steady_clock::now();
	int ms = chrono::duration_cast<chrono::milliseconds>(end - start).count();
	cout << "Elapsed time in milliseconds : " 
		<< ms
		<< " ms" << endl;
	cout << "FPS: " << numFrames/(ms/1000.0) << endl;

	delete swarm;
	std::cout << "end\n";
	return 0;
}
