all: main

CFLAGS=-fPIC -g -Wall
LIBS = ``
INCLUDE = -I/usr/include/libpng16/
FREE_LIBS = -lglfw3 -lGLEW -lGL -lm -lXrandr -lXi -lX11 -ldl -lXxf86vm -lpthread -lpng


main:  main.cpp ./common/shader.o ./common/objloader.o ./common/texture.o ./common/controls.o
	$(CXX) $(INCLUDE) $(CFLAGS) $? -o $@  $(LIBS) $(FREE_LIBS)

./common/shader.o: ./common/shader.cpp
	$(CXX) -c $(CFLAGS) $< -o $@ $(FREE_LIBS)

./common/objloader.o: ./common/objloader.cpp
	$(CXX) -c $(CFLAGS) $< -o $@ $(FREE_LIBS)

./common/texture.o: ./common/texture.cpp
	$(CXX) -c $(CFLAGS) $< -o $@ $(FREE_LIBS)

./common/controls.o: ./common/controls.cpp
	$(CXX) -c $(CFLAGS) $< -o $@ $(FREE_LIBS)

clean:
	rm -rf *.o main
	rm -rf ./common/*.o
