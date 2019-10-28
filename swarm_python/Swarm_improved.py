import random as rand
from math import *
from graphics import *
import time

MatrixHeight = 200
MatrixLength = 400
MoveMax = 10
# if 0, global
Bee_Power = 20
Flower_Power = 30
# Between 0 and pi
RandomNessFactor = pi/4

MatrixMaxMag = sqrt(MatrixHeight**2 + MatrixLength**2)

class BeeHandle:
    def __init__(self):
        self.bee_array = []
        self.bee_flower = []
        self.flower_array = []
        self.flower_bee = []

    def add_bee(self, x: int = None, y: int = None):
        if x is None:
            x = int(rand.uniform(0,MatrixLength-0.5))
            y = int(rand.uniform(0,MatrixHeight-0.5))

        self.bee_array.append([x, y])
        self.bee_flower.append(None)

    def add_flower(self, x: int = None, y: int = None):
        if x is None:
            x = int(rand.uniform(0, MatrixLength - 0.5))
            y = int(rand.uniform(0, MatrixHeight - 0.5))

        self.flower_array.append([x, y])
        self.flower_bee.append(None)

    def update_movement(self):
        for i in range(len(self.bee_array)):
            bee = self.bee_array[i]
            #if self.bee_flower[i] is None:
            total_x = 0
            total_y = 0
            for j in range(len(self.bee_array)):
                #if j != i and self.bee_flower[j] == None:
                if j != i:
                    bee2 = self.bee_array[j]
                    dist_x = bee2[0] - bee[0]
                    dist_y = bee2[1] - bee[1]

                    dist = sqrt(dist_x ** 2 + dist_y ** 2)
                    dist = (MatrixMaxMag - dist)**2

                    total_x -= dist_x * dist * (Bee_Power/Flower_Power)
                    total_y -= dist_y * dist * (Bee_Power/Flower_Power)
            flower_in_range = -1
            flower_dist = None
            for j in range(len(self.flower_array)):
                #if self.flower_bee[j] == None:
                if self.flower_bee[j] != i:
                    flower = self.flower_array[j]
                    dist_x = flower[0] - bee[0]
                    dist_y = flower[1] - bee[1]

                    dist = sqrt(dist_x ** 2 + dist_y ** 2)
                    if dist < MoveMax and (flower_in_range < 0 or dist < flower_dist):
                        flower_in_range = j
                        flower_dist = dist

                    dist = (MatrixMaxMag - dist)**2

                    total_x += dist_x * dist
                    total_y += dist_y * dist

                if self.flower_bee[j] == i:
                    self.bee_flower[i] = None
                    self.flower_bee[j] = None
            if flower_in_range < 0:
                # tan = y/x
                rads = atan2(total_y, total_x) + rand.uniform(-1*RandomNessFactor, RandomNessFactor)

                if not (total_x == 0 and total_y == 0):
                    # x = cos(rads) * z
                    bee[0] = bee[0] + int(cos(rads) * MoveMax)

                    # y = sin(rads) * z
                    bee[1] = bee[1] + int(sin(rads) * MoveMax)
            else:
                self.bee_flower[i] = flower_in_range
                self.flower_bee[flower_in_range] = i
                bee[0] = self.flower_array[flower_in_range][0]
                bee[1] = self.flower_array[flower_in_range][1]

def main():
    bee_handle = BeeHandle()
    win = GraphWin("Bees", MatrixLength, MatrixHeight)
    for i in range(300):
        bee_handle.add_bee()
    for i in range(200):
        bee_handle.add_flower(i+100,100)
    Points = []
    for bee in bee_handle.bee_array:
        Points.append(Point(bee[0], bee[1]))
    for point in Points:
        point.draw(win)
    start_time = time.time()
    while True:
        bee_handle.update_movement()
        for i in range(len(bee_handle.bee_array)):
            Points[i].move(bee_handle.bee_array[i][0] - Points[i].getX(), bee_handle.bee_array[i][1] - Points[i].getY())
        time_passed = time.time() - start_time
        print(1/time_passed)
        start_time = time.time()



if __name__ == "__main__":
    main()





