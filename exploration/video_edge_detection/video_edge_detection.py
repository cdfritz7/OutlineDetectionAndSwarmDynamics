#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Sep  9 21:41:26 2019

@author: fritz

much of this code was taken from
https://www.learnopencv.com/read-write-and-display-a-video-using-opencv-cpp-python/
"""

import cv2
import numpy as np

def edge_detect_video(infile, outfile, lower_thres, upper_thres):
    '''
    Applies Canny Edge detection to a saved video file, and writes
    the result to a new file
    
    inputs:
        infile; str - the path to the original video to use
        outfile; str - the path to write to
        lower_thres; int - the lower threshold to use for Canny edge detection
        upper_thres; int - the upper threshold to use for Canny edge detection
    '''
    
    cap = cv2.VideoCapture(infile)
     
    # Check if camera opened successfully, creates video writer
    #if this doesn't work try changing the video codec to 'MJPG' or 'DIVX'
    if cap.isOpened(): 
        frame_width = int(cap.get(3))
        frame_height = int(cap.get(4))
        
        out_writer = cv2.VideoWriter(outfile,
                                     cv2.VideoWriter_fourcc(*'XVID'), 
                                     40, #20 frames per second
                                     (frame_width,frame_height))
    else:
      raise Exception("Error opening video stream or file")
    
    # Read and write until video is completed
    while(cap.isOpened()):
      # Capture frame-by-frame
      ret, frame = cap.read()
      
      if ret == True:
         new_frame = cv2.Canny(frame, lower_thres, upper_thres)
         #convert to a form our video writer can use
         new_frame = cv2.cvtColor(new_frame, cv2.COLOR_RGB2BGR) 
         out_writer.write(new_frame)
     
      else: 
        break
     
    # When everything done, release the video capture object
    cap.release()
    out_writer.release()

if __name__=='__main__':
    #range of lower threshold values
    lower_thres_start = 25
    lower_thres_end = 75
    lower_thres_iter = 25
    
    #range of upper threshold values
    upper_thres_start = 100
    upper_thres_end = 200
    upper_thres_iter = 50
    
    #infile name
    in_name = './in/vid1.mp4'
    
    for i in range(upper_thres_start, upper_thres_end+1, upper_thres_iter):
        if i < lower_thres_end:
            i2 = i
        else:
            i2 = lower_thres_end+1
        for j in range(lower_thres_start, i2, lower_thres_iter):
            print(f'Converting with lower threshold {j} upper threshold {i}')
            edge_detect_video(in_name, f'./out/vid1_{j}_{i}.mp4', j, i)
            