// compile with g++ -std=c++11 -pthread -Wall multi-mcmc.cpp

#include <iostream>
#include <thread>
#include <stdlib.h>
#include <time.h>
#include <chrono>

using namespace std;

#define NUM_BEES 1000
#define NUM_THREADS 4

double bee_x [NUM_BEES];
double bee_y [NUM_BEES];

thread threads [NUM_THREADS];

int dist(int x1, int y1, int x2, int y2) {
	return (x1-x2)*(x1-x2)+(y1-y2)*(y1-y2);
}

// doesn't actually meaningfully move bees, but simulates the calculations needed to implement mcmc algo
// TODO - real mcmc logic
void move_bee(int thread_num) { 
	int bee_per_thread = NUM_BEES/NUM_THREADS;
	int start = bee_per_thread*thread_num;
	int end = start + bee_per_thread;
	for(int i=start; i<end; i++) {
		for(int j=0; j<NUM_BEES; j++) {
			double distance = dist(bee_x[i], bee_y[i], bee_x[j], bee_y[j]);
		}
		bee_x[i] += rand() % 10 - 5;
		bee_y[i] += rand() % 10 - 5;
	}
}

void init_bees() {
	for(int i=0; i<NUM_BEES; i++) {
		bee_x[i] = 0;
		bee_y[i] = 0;
	}
}

void set_threads() {
	int bee_per_thread = NUM_BEES/NUM_THREADS;
	for(int i=0; i<NUM_THREADS; i++) {
		threads[i] = thread(move_bee, i);
	}
}

void join_threads() {
	for(int i=0; i<NUM_THREADS; i++) {
		threads[i].join();
	}
}

int main() {
	int numFrames = 500;
	init_bees();
	cout << "Parallel Test\n";
	auto start = chrono::steady_clock::now();
	for(int i=0; i<numFrames; i++) {
		set_threads();
		join_threads();
		//cout << "Frame " << i << " done."; 
	}
	auto end = chrono::steady_clock::now();
	cout << "done.\n";
	cout << "Elapsed time in milliseconds : "
		<< chrono::duration_cast<chrono::milliseconds>(end - start).count()
		<< " ms" << endl;
	init_bees();
	cout << "Linear Test \n";
	auto start2 = chrono::steady_clock::now();
	for(int i=0; i<numFrames; i++) {
		for(int j=0; j<NUM_THREADS; j++) {
			move_bee(j);
		}
	}
	auto end2 = chrono::steady_clock::now();
	cout << "done.\n";
	cout << "Elapsed time in milliseconds : " 
		<< chrono::duration_cast<chrono::milliseconds>(end2 - start2).count()
		<< " ms" << endl;
	return 0;
}
