all: main

CFLAGS=-fPIC -g -Wall `pkg-config --cflags opencv` -fpermissive
LIBS = `pkg-config --libs opencv`
INCLUDE = -I/usr/include/libusb-1.0/ -I/usr/local/include/opencv4/opencv2/
FREE_LIBS = -lfreenect -lpthread -lglfw3 -lGLEW -lGL -lm -lXrandr -lXi -lX11 -ldl -lXxf86vm -lpthread -lopenal -lalut

GRAPHICS_CFLAGS = -fPIC -g -Wall
GRAPHICS_LIBS = -lglfw3 -lGLEW -lGL -lm -lXrandr -lXi -lX11 -ldl -lXxf86vm -lpthread

main:  main.cpp ./graphics/graphics_module.o ./graphics/common/shader.o ./graphics/common/objloader.o ./graphics/common/texture.o ./graphics/common/controls.o ./graphics/common/text2D.o
	$(CXX) $(INCLUDE) $(CFLAGS) $? -o $@ $(LIBS) $(FREE_LIBS)

./graphics/graphics_module.o: ./graphics/graphics_module.cpp
	$(CXX) -c $(GRAPHICS_CFLAGS) $< -o $@ $(GRAPHICS_LIBS)

./graphics/common/text2D.o: ./graphics/common/text2D.cpp
	$(CXX) -c $(CFLAGS) $< -o $@ $(FREE_LIBS)

./graphics/common/shader.o: ./graphics/common/shader.cpp
	$(CXX) -c $(GRAPHICS_CFLAGS) $< -o $@ $(GRAPHICS_LIBS)

./graphics/common/objloader.o: ./graphics/common/objloader.cpp
	$(CXX) -c $(GRAPHICS_CFLAGS) $< -o $@ $(GRAPHICS_LIBS)

./graphics/common/texture.o: ./graphics/common/texture.cpp ./graphics/common/stb_image.h
	$(CXX) -c $(GRAPHICS_CFLAGS) $< -o $@ $(GRAPHICS_LIBS)

./graphics/common/controls.o: ./graphics/common/controls.cpp
	$(CXX) -c $(GRAPHICS_CFLAGS) $< -o $@ $(GRAPHICS_LIBS)

clean:
	rm -rf *.o ./graphics/*.o ./graphics/common/*.o main


