1) install opencv libraries using the instructions in the following link
https://docs.opencv.org/trunk/d7/d9f/tutorial_linux_install.html

2) run using the following command
g++ -I/usr/local/include/opencv -I/usr/local/include/opencv2 -L/usr/local/lib/ -g -o real_time_edges  real_time_edge.cpp -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_video -lopencv_imgcodecs -lopencv_videoio 
