#include <iostream>
#include "BeeHandle.h"

using namespace std;

int main() {
	BeeHandle bee_handle;

	bee_handle.add_flower(25, 25);

	bee_handle.update_bee_coords();

	int* beelocX = bee_handle.getBeeLocX();
	int* beelocY = bee_handle.getBeeLocY();

	for (int i = 0; i < NumBees; i++) {
		cout << "(" << beelocX[i] << "," << beelocY[i] << ")" << endl;
	}

	cin.get();
}
