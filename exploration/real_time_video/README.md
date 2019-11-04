1) install opencv libraries using the instructions in the following link
https://docs.opencv.org/trunk/d7/d9f/tutorial_linux_install.html

2) run using the following command
g++ -I/usr/local/include/opencv -I/usr/local/include/opencv2 -L/usr/local/lib/ -g -o real_time_edges  real_time_edge.cpp -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_video -lopencv_imgcodecs -lopencv_videoio 

approximate steps (adriana, ubuntu 18.04.3 LTS): 

1. download archive from https://opencv.org/releases/. unzip to home folder
2. cd into that folder, then `mkdir build && cd build/`
3. `cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/usr/local ..`
4. `make -j7` or more cores if you prefer
5. `sudo make install`
6. had to add the variable for the path: `export LD_LIBRARY_PATH=/usr/local/lib`
6. go to "/OutlineDetectionAndSwarmDynamics/exploration/real_time_video" and run:
`g++ -I/usr/local/include/opencv4/ -L/usr/local/lib/ -g -o real_time_edges  real_time_edge.cpp -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_video -lopencv_imgcodecs -lopencv_videoio`
7. also had to install a canberra module to get rid of a warning: `sudo apt-get install libcanberra-gtk-module`
