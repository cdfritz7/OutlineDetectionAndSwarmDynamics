#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "./graphics_module.hpp"

using namespace std;

int main( void ){
  int maxX = 500;
  int maxY = 1000;
  GraphicsModule gm (4, maxX, maxY, 1, 2.0f,
                     "abee.png", "./");

  int bee_x_arr[] = {0, maxX, maxX, 0};
  int bee_y_arr[] = {0, 0, maxY, maxY};

  vector<int> bee_x (bee_x_arr, bee_x_arr+4);
  vector<int> bee_y (bee_y_arr, bee_y_arr+4);
  vector<int> zeros (4);

  gm.update_particles(bee_x, bee_y, zeros, zeros);

  do{
    gm.update_display();
  }while(!gm.should_close());

  gm.cleanup();
  return 0;
}
