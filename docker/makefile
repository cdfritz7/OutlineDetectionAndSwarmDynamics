all: main

CFLAGS=-fPIC -g -Wall `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv`
INCLUDE = -I/usr/include/libusb-1.0/ -I/usr/local/include/opencv4/opencv2
FREE_LIBS = -lfreenect -lpthread

main:  main.cpp
	$(CXX) $(INCLUDE) $(CFLAGS) $? -o $@  $(LIBS) $(FREE_LIBS)

%.o: %.cpp
	$(CXX) -c $(CFLAGS) $< -o $@

clean:
	rm -rf *.o main
