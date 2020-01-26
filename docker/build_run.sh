xhost local:root
docker build -t image_name .
sudo docker run --privileged -v /tmp/.X11-unix:/tmp/.X11-unix -e DISPLAY=$DISPLAY -it image_name /bin/bash

