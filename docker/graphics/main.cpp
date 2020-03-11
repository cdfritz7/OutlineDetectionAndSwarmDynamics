#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "./graphics_module.hpp"

using namespace std;

int main( void ){
  GraphicsModule gm (1000, 10, 10, -10, -10);
  
  do{
    //gm.update_particles(x, y, stage, direction);
    gm.update_display();
  }while(!gm.should_close());

  gm.cleanup();
  return 0;
}
