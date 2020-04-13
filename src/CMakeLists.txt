cmake_minimum_required(VERSION 3.7)
project(main)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp BeeHandle.hpp BeeHandle_simple.hpp utils.cpp utils.h)
add_executable(main ${SOURCE_FILES})

# OpenCV libs
find_package(OpenCV REQUIRED)
find_package(libfreenect REQUIRED)
find_package (PkgConfig REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})


# ==================== PATHS TO SPECIFY! ==================== #

# Eigen lib headers
include_directories("/usr/local/include/eigen3")

#protobuf lib headers
include_directories("/usr/local/include/google/protobuf/src")


include_directories("/usr/include/libusb-1.0/")
include_directories("/usr/local/include/libfreenect")

# TensorFlow headers
include_directories("/usr/local/include/tf/")
include_directories("/usr/local/include/tf/bazel-genfiles/")
include_directories("/usr/local/include/tf/tensorflow/")
include_directories("/usr/local/include/tf/third-party/")

# Link OpenCV libs
target_link_libraries(main ${OpenCV_LIBS} ${CMAKE_THREAD_LIBS_INIT} ${FREENECT_LIBRARIES})

set(THREADS_PREFER_PTHREAD_FLAG ON)

target_link_libraries(main -lpthread)
target_link_libraries(main usb-1.0)

# Link TensorFlow libs
target_link_libraries(main "/usr/local/lib/libtensorflow_cc.so")
target_link_libraries(main "/usr/local/lib/libtensorflow_framework.so")

