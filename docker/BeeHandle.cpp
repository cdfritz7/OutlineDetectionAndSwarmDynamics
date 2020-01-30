#include <iostream>
#include <ctime>
#include "BeeHandle.h"

using namespace std;

void fps_test(int num_bees = 400, int num_flowers = 200, int frames = 100) {
	BeeHandle bee_handle;

	for (int i = 0; i < num_bees; i++) {
		bee_handle.add_bee();
	}

	for (int i = 0; i < num_flowers; i++) {
		bee_handle.add_flower();
	}

	clock_t time_start = clock();

	int frame = 0;
	double total_fps = 0.0;
	while (frame < frames) {
		bee_handle.update_movement();
		frame++;
		double elapsed_secs = double(clock() - time_start) / CLOCKS_PER_SEC;
		double fps = (double)1 / elapsed_secs;
		time_start = clock();
		total_fps += fps;
	}
	cout << "Average FPS: " << total_fps / frame << endl;
}

int main() {
	cout << "Bees = 100; Flowers = 200" << endl;
	fps_test(100, 200);
	cout << "Bees = 200; Flowers = 200" << endl;
	fps_test(200, 200);
	cout << "Bees = 300; Flowers = 200" << endl;
	fps_test(300, 200);
	cin.get();
}