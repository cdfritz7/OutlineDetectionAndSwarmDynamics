all: test

CFLAGS=-fPIC -g -Wall `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv`
INCLUDE = -I/usr/local/include/opencv2
FREE_LIBS = -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_video -lopencv_imgcodecs -lopencv_videoio

test:  simple_bee_animation.cpp
	g++ $(INCLUDE) $(CFLAGS) $? -o $@  $(LIBS) $(FREE_LIBS)

clean:
	rm -rf *.o test
