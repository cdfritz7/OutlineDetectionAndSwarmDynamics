#include <iostream>
#include <ctime>
#include "BeeHandle.h"

using namespace std;

int main() {

	BeeHandle bee_handle;

	for (int i = 0; i < 200; i++) {
		bee_handle.add_bee();
	}
	for (int i = 0; i < 200; i++) {
		bee_handle.add_flower(i + 50, 100);
	}
	clock_t time_start = clock();
	int i = 0;
	while (i < 100) {
		bee_handle.update_movement();
		double elapsed_secs = double(clock() - time_start) / CLOCKS_PER_SEC;
		double fps = (double)1 / elapsed_secs;
		cout << fps << endl;
		time_start = clock();
		i++;
	}
}
