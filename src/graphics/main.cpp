#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "./graphics_module.hpp"

using namespace std;

int main( void ){
  int maxX = 500;
  int maxY = 1000;
  GraphicsModule gm (6, maxX, maxY, 1, 2.0f,
                     "abee.png", "./");

  int bee_x_arr[] = {0, maxX, maxX, 0, maxX/4, maxX/2};
  int bee_y_arr[] = {0, 0, maxY, maxY, maxY/4, maxY/2};

  vector<int> bee_x (bee_x_arr, bee_x_arr+6);
  vector<int> bee_y (bee_y_arr, bee_y_arr+6);
  vector<int> zeros (6);

  gm.update_particles(bee_x, bee_y, zeros, zeros);

  do{
    gm.update_qr(true, "./qr.png", 500, 1000, 4.0f);
    gm.update_display();
  }while(!gm.should_close());

  gm.cleanup();
  return 0;
}
