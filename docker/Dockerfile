FROM ubuntu:18.04

#name our working directory in the container /project
WORKDIR /project

#add our essential repos to our container
RUN apt-get -y update && apt-get -y upgrade && \
    apt-get install -y build-essential cmake pkg-config

#make apt-get installs noninteractive - because we won't be able to interact with them
ENV DEBIAN_FRONTEND=noninteractive

#add opencv to our container https://docs.opencv.org/trunk/d7/d9f/tutorial_linux_install.html
RUN apt-get -y update & apt-get install --fix-missing -y git libgtk2.0-dev libavcodec-dev libavformat-dev libswscale-dev
RUN git clone https://github.com/opencv/opencv.git && \
    git clone https://github.com/opencv/opencv_contrib.git && \
    cd ./opencv && \
    mkdir release && \
    cd release && \
    cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/usr/local .. && \
    make -j4 && \
    make install && \
    apt-get install -y libopencv-dev && \
    apt-get install -y freenect #add libfreenect

#add gdb
RUN apt-get install -y gdb valgrind

#add the stuff for graphics we can download with apt-get
RUN apt-get -y update && apt-get -y install libglu1-mesa-dev freeglut3-dev && \
    apt-get -y install mesa-common-dev libglm-dev && \
    apt-get -y install unzip nano

#copy over all the files in the current directory to our container
#anything after this line will be rerun at every build
ADD . /project

#build graphics stuff from the dependencies folder
RUN cd dependencies && tar -xvzf glew-2.1.0.tgz && cd glew-2.1.0 && make install && cd ../ && \
    unzip glfw-3.3.2.zip && cd glfw-3.3.2 && cmake . && make install && cd ../../

#move updated version of libfreenect header and make the executable
RUN mv /project/libfreenect.hpp /usr/include/libfreenect.hpp && \
    make

#run the executable when the container is run
CMD ./main
