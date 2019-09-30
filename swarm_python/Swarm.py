import json
import numpy as np
import matplotlib.pyplot as plt
import random as rand
import time
from graphics import *

MatrixHeight = 50
MatrixLength = 50
MoveMin = 0
MoveMax = 10
FlowerStrength = 10
BeeStrength = 10
RandomNessFactor = 10
NumBees = 100

def main():
    bee_handle = BeeHandle()
    win = GraphWin("Bees", MatrixLength, MatrixHeight)
    for i in range(NumBees-1):
        bee_handle.add_bee()
    Points = []
    for bee in bee_handle.bee_array:
        Points.append(Point(bee.x, bee.y))
    for point in Points:
        point.draw(win)
    flower_x = [25,24,24,25,26,26,26,25,24]
    flower_y = [25,25,26,26,26,25,24,24,24]
    bee_handle.add_flower(25, 25)
    bee_handle.add_flower(24, 25)
    bee_handle.add_flower(24, 26)
    bee_handle.add_flower(25, 26)
    bee_handle.add_flower(26, 26)
    bee_handle.add_flower(26, 25)
    bee_handle.add_flower(26, 24)
    bee_handle.add_flower(25, 24)
    bee_handle.add_flower(24, 24)
    while(1):
        bee_handle.update_bee_matrix()
        bee_handle.update_movement_matrix()
        bee_handle.update_bees()
        bee_handle.update_bee_coords()
        for i in range(len(bee_handle.bee_loc_x)):
            Points[i].move(bee_handle.bee_loc_x[i] - Points[i].getX(), bee_handle.bee_loc_y[i] - Points[i].getY())
        time.sleep(.1)

class BeeHandle:
    def __init__(self):
        self.bee_array = []
        self.bee_matrix = []
        self.movement_matrix = []
        self.flower_matrix = []
        self.bee_loc_x = []
        self.bee_loc_y = []
        for i in range(MatrixLength):
            bee_y = []
            move_y = []
            flower_y = []
            for j in range(MatrixHeight):
                bee_y.append(0)
                move_y.append(0)
                flower_y.append(0)
            self.bee_matrix.append(bee_y)
            self.movement_matrix.append(move_y)
            self.flower_matrix.append(flower_y)

    def add_flower(self, x, y):
        for i in range(FlowerStrength):
            value = (FlowerStrength - i)**2
            if i == 0:
                self.flower_matrix[x][y] = value
            else:
                for fx in range(i+1):
                    fy = i - fx
                    if x + fx < MatrixLength and y + fy < MatrixHeight and fx != 0 and self.flower_matrix[x+fx][y+fy] < value:
                        self.flower_matrix[x+fx][y+fy] = value
                    if x + fx < MatrixLength and y - fy >= 0 and fx != 0 and fy != 0  and self.flower_matrix[x+fx][y-fy] < value:
                        self.flower_matrix[x+fx][y-fy] = value
                    if x - fx >= 0 and y + fy < MatrixHeight and fy != 0  and self.flower_matrix[x-fx][y+fy] < value:
                        self.flower_matrix[x-fx][y+fy] = value
                    if x - fx >= 0 and y - fy >= 0 and self.flower_matrix[x-fx][y-fy] < value:
                        self.flower_matrix[x-fx][y-fy] = value

    def add_bee(self, x  = None, y = None):
        if x is None:
            x = int(rand.uniform(0, MatrixLength - 1))
        if y is None:
            y = int(rand.uniform(0, MatrixHeight - 1))
        bee = Bee(x,y)
        self.bee_array.append(bee)

    def update_bee_matrix(self):
        for bee in self.bee_array:
            x = bee.x
            y = bee.y
            for i in range(BeeStrength):
                value = (BeeStrength - i)**2 * -1
                if i == 0:
                    self.bee_matrix[x][y] = value
                else:
                    for fx in range(i + 1):
                        fy = i - fx
                        if x + fx < MatrixLength and y + fy < MatrixHeight and fx != 0 and self.bee_matrix[x + fx][
                            y + fy] > value:
                            self.bee_matrix[x + fx][y + fy] += value
                        if x + fx < MatrixLength and y - fy >= 0 and fx != 0 and fy != 0 and self.bee_matrix[x + fx][
                            y - fy] > value:
                            self.bee_matrix[x + fx][y - fy] += value
                        if x - fx >= 0 and y + fy < MatrixHeight and fy != 0 and self.bee_matrix[x - fx][
                            y + fy] > value:
                            self.bee_matrix[x - fx][y + fy] += value
                        if x - fx >= 0 and y - fy >= 0 and self.bee_matrix[x - fx][y - fy] > value:
                            self.bee_matrix[x - fx][y - fy] += value

    def update_movement_matrix(self):
        for x in range(MatrixLength):
            for y in range(MatrixHeight):
                self.movement_matrix[x][y] = self.bee_matrix[x][y] + self.flower_matrix[x][y]

    def update_bees(self):
        for bee in self.bee_array:
            dist = int(rand.uniform(MoveMin, MoveMax))
            #print(dist)
            if dist != 0:
                x = bee.x
                y = bee.y
                value = NumBees * (BeeStrength ** 2) * -1 - 1
                new_x = x
                new_y = y
                for fx in range(dist + 1):
                    fy = dist - fx
                    #print(fy)
                    if x + fx < MatrixLength and y + fy < MatrixHeight:
                        #print("1: " + str(self.movement_matrix[x + fx][y + fy]))
                        temp_value = self.movement_matrix[x + fx][y + fy] - rand.uniform(0, RandomNessFactor)**2
                        if fx != 0 and temp_value > value:
                            #print("1: " + str(self.movement_matrix[x + fx][y + fy]))
                            value = temp_value
                            new_x = x + fx
                            new_y = y + fy
                    if x + fx < MatrixLength and y - fy >= 0:
                        #print("2: " + str(self.movement_matrix[x + fx][y - fy]))
                        temp_value = self.movement_matrix[x + fx][y - fy] - rand.uniform(0, RandomNessFactor)**2
                        if fx != 0 and fy != 0 and temp_value > value:
                            #print("2: " + str(self.movement_matrix[x + fx][y - fy]))
                            value = temp_value
                            new_x = x + fx
                            new_y = y - fy
                    if x - fx >= 0 and y + fy < MatrixHeight:
                        #print("3: " + str(self.movement_matrix[x - fx][y + fy]))
                        temp_value = self.movement_matrix[x - fx][y + fy] - rand.uniform(0, RandomNessFactor)**2
                        if fy != 0 and temp_value > value:
                            #print("3: " + str(self.movement_matrix[x - fx][y + fy]))
                            value = temp_value
                            new_x = x - fx
                            new_y = y + fy
                    if x - fx >= 0 and y - fy >= 0:
                        #print("4: " + str(self.movement_matrix[x - fx][y - fy]))
                        temp_value = self.movement_matrix[x - fx][y - fy] - rand.uniform(0, RandomNessFactor)**2
                        if temp_value > value:
                            #print("4: " + str(self.movement_matrix[x - fx][y - fy]))
                            value = temp_value
                            new_x = x - fx
                            new_y = y - fy
                bee.move(new_x, new_y)

    def update_bee_coords(self):
        self.bee_loc_x = []
        self.bee_loc_y = []
        for bee in self.bee_array:
            self.bee_loc_x.append(bee.x)
            self.bee_loc_y.append(bee.y)
        #print(self.bee_loc_y)

class Bee:
    def __init__(self, x = None, y = None):
        if x is None:
            x = int(rand.uniform(0, MatrixLength - 1))
        if y is None:
            y = int(rand.uniform(0, MatrixHeight - 1))
        self.x = x
        self.y = y

    def move(self, x, y):
        self.x = x
        self.y = y


if __name__ == '__main__':
    main()