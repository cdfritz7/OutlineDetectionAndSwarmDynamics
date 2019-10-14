import random as rand
import time
from graphics import *
from copy import copy, deepcopy

MatrixHeight = 50
MatrixLength = 50
#MoveMin = 0
MoveMax = 5
FlowerStrength = 10
BeeStrength = 10
RandomNessFactor = 5
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
    #flower_x = [25]
    #flower_y = [25]
    #flower_x = [25,25,25,25,25,25,25,25,25,25,0,4,9,14,19,24,29,34,39,44,49]
    #flower_y = [0,4,9,14,19,24,29,34,39,44,49,25,25,25,25,25,25,25,25,25,25]
    for i in range(len(flower_x)):
        bee_handle.add_flower(flower_x[i], flower_y[i])
    #line = ""
    #for y in range(MatrixHeight):
    #    for x in range(MatrixLength):
    #        line += str(bee_handle.flower_matrix[x][y]) + " "
    #    print(line)
    #    line = ""
    current_time = time.time()
    while(1):
        bee_handle.update_bee_matrix()
        bee_handle.update_movement_matrix()
        bee_handle.update_bees()
        bee_handle.update_bee_coords()
        #for i in range(len(bee_handle.bee_loc_x)):
        #    Points[i].move(bee_handle.bee_loc_x[i] - Points[i].getX(), bee_handle.bee_loc_y[i] - Points[i].getY())
        time_passed = time.time() - current_time
        current_time = time.time()
        print(1/time_passed)

class BeeHandle:
    def __init__(self):
        self.bee_array = []
        self.empty_matrix = []
        self.bee_matrix = []
        self.movement_matrix = []
        self.flower_matrix = []
        self.bee_loc_x = []
        self.bee_loc_y = []
        for i in range(MatrixLength):
            bee_y = []
            empty_y = []
            move_y = []
            flower_y = []
            for j in range(MatrixHeight):
                bee_y.append(0)
                empty_y.append(0)
                move_y.append(0)
                flower_y.append(0)
            self.bee_matrix.append(bee_y)
            self.empty_matrix.append(empty_y)
            self.movement_matrix.append(move_y)
            self.flower_matrix.append(flower_y)

    def add_flower(self, x, y):
        for i in range(FlowerStrength):
            value = (FlowerStrength - i)**2
            if i == 0:
                self.flower_matrix[x][y] = value
            else:
                for j in range(2*i+1):
                    fx = x - i
                    if 0 <= fx and 0 <= y - i + j < MatrixHeight and self.flower_matrix[fx][y - i + j] < value:
                        self.flower_matrix[fx][y - i + j] = value
                    fx = x + i
                    if fx < MatrixLength and 0 <= y - i + j < MatrixHeight and self.flower_matrix[fx][y - i + j] < value:
                        self.flower_matrix[fx][y - i + j] = value
                    fy = y - i
                    if 0 <= fy and 0 <= x - i + j < MatrixLength and self.flower_matrix[x - i + j][fy] < value:
                        self.flower_matrix[x - i + j][fy] = value
                    fy = y + i
                    if fy < MatrixHeight and 0 <= x - i + j < MatrixLength and self.flower_matrix[x - i + j][fy] < value:
                        self.flower_matrix[x - i + j][fy] = value

                #for fx in range(i+1):
                #    fy = i - fx
                #    if x + fx < MatrixLength and y + fy < MatrixHeight and fx != 0 and self.flower_matrix[x+fx][y+fy] < value:
                #        self.flower_matrix[x+fx][y+fy] = value
                #    if x + fx < MatrixLength and y - fy >= 0 and fx != 0 and fy != 0  and self.flower_matrix[x+fx][y-fy] < value:
                #        self.flower_matrix[x+fx][y-fy] = value
                #    if x - fx >= 0 and y + fy < MatrixHeight and fy != 0  and self.flower_matrix[x-fx][y+fy] < value:
                #        self.flower_matrix[x-fx][y+fy] = value
                #    if x - fx >= 0 and y - fy >= 0 and self.flower_matrix[x-fx][y-fy] < value:
                #       self.flower_matrix[x-fx][y-fy] = value

    def add_bee(self, x  = None, y = None):
        if x is None:
            x = int(rand.uniform(0, MatrixLength - 1))
        if y is None:
            y = int(rand.uniform(0, MatrixHeight - 1))
        bee = Bee(x,y)
        self.bee_array.append(bee)

    def update_bee_matrix(self):
        self.bee_matrix = ""
        self.bee_matrix = deepcopy(self.empty_matrix)
        for bee in self.bee_array:
            x = bee.x
            y = bee.y
            for i in range(BeeStrength):
                value = (BeeStrength - i)**2 * -1
                if i == 0:
                    self.bee_matrix[x][y] = value
                else:
                    for j in range(2 * i + 1):
                        fx = x - i
                        if 0 <= fx and 0 <= y - i + j < MatrixHeight and self.bee_matrix[fx][y - i + j] > value:
                            self.bee_matrix[fx][y - i + j] += value
                        fx = x + i
                        if fx < MatrixLength and 0 <= y - i + j < MatrixHeight and self.bee_matrix[fx][
                            y - i + j] > value:
                            self.bee_matrix[fx][y - i + j] += value
                        fy = y - i
                        if 0 <= fy and 0 <= x - i + j < MatrixLength and self.bee_matrix[x - i + j][fy] > value:
                            self.bee_matrix[x - i + j][fy] += value
                        fy = y + i
                        if fy < MatrixHeight and 0 <= x - i + j < MatrixLength and self.bee_matrix[x - i + j][
                            fy] > value:
                            self.bee_matrix[x - i + j][fy] += value
                    #for fx in range(i + 1):
                    #    fy = i - fx
                    #    if x + fx < MatrixLength and y + fy < MatrixHeight and fx != 0 and self.bee_matrix[x + fx][
                    #        y + fy] > value:
                    #        self.bee_matrix[x + fx][y + fy] += value
                    #    if x + fx < MatrixLength and y - fy >= 0 and fx != 0 and fy != 0 and self.bee_matrix[x + fx][
                    #        y - fy] > value:
                    #        self.bee_matrix[x + fx][y - fy] += value
                    #    if x - fx >= 0 and y + fy < MatrixHeight and fy != 0 and self.bee_matrix[x - fx][
                    #        y + fy] > value:
                    #        self.bee_matrix[x - fx][y + fy] += value
                    #    if x - fx >= 0 and y - fy >= 0 and self.bee_matrix[x - fx][y - fy] > value:
                    #        self.bee_matrix[x - fx][y - fy] += value

    def update_movement_matrix(self):
        for x in range(MatrixLength):
            for y in range(MatrixHeight):
                self.movement_matrix[x][y] = self.bee_matrix[x][y] + self.flower_matrix[x][y]

    def update_bees(self):
        for bee in self.bee_array:
            x = bee.x
            y = bee.y
            value = NumBees * (BeeStrength ** 2) * -1 - 1
            if x + MoveMax >= MatrixLength:
                xmov_max = (MatrixLength - x) - 1
            else:
                xmov_max = MoveMax
            if x - MoveMax < 0:
                xmov_min = -1 * x
            else:
                xmov_min = -1 * MoveMax
            if y + MoveMax >= MatrixHeight:
                ymov_max = (MatrixHeight - y) - 1
            else:
                ymov_max = MoveMax
            if y - MoveMax < 0:
                ymov_min = -1 * y
            else:
                ymov_min = -1 * MoveMax

            new_x = x + int(rand.uniform(xmov_min, xmov_max))
            new_y = y + int(rand.uniform(ymov_min, ymov_max))

            current_value = self.movement_matrix[x][y] + int(rand.uniform(-1 * RandomNessFactor, RandomNessFactor)**2)
            new_value = self.movement_matrix[new_x][new_y] + int(rand.uniform(-1 * RandomNessFactor, RandomNessFactor)**2)

            if new_value > current_value:
                bee.move(new_x, new_y)

            #dist = int(rand.uniform(MoveMin, MoveMax))
            ##print(dist)
            #if dist != 0:
            #    x = bee.x
            #    y = bee.y
            #    value = NumBees * (BeeStrength ** 2) * -1 - 1
            #    new_x = x
            #    new_y = y
            #    for fx in range(dist + 1):
            #        fy = dist - fx
            #        #print(fy)
            #        if x + fx < MatrixLength and y + fy < MatrixHeight:
            #            #print("1: " + str(self.movement_matrix[x + fx][y + fy]))
            #            temp_value = self.movement_matrix[x + fx][y + fy] - rand.uniform(0, RandomNessFactor)**2
            #            if fx != 0 and temp_value > value:
            #                #print("1: " + str(self.movement_matrix[x + fx][y + fy]))
            #                value = temp_value
            #                new_x = x + fx
            #                new_y = y + fy
            #        if x + fx < MatrixLength and y - fy >= 0:
            #            #print("2: " + str(self.movement_matrix[x + fx][y - fy]))
            #            temp_value = self.movement_matrix[x + fx][y - fy] - rand.uniform(0, RandomNessFactor)**2
            #            if fx != 0 and fy != 0 and temp_value > value:
            #                #print("2: " + str(self.movement_matrix[x + fx][y - fy]))
            #                value = temp_value
            #                new_x = x + fx
            #                new_y = y - fy
            #        if x - fx >= 0 and y + fy < MatrixHeight:
            #            #print("3: " + str(self.movement_matrix[x - fx][y + fy]))
            #            temp_value = self.movement_matrix[x - fx][y + fy] - rand.uniform(0, RandomNessFactor)**2
            #            if fy != 0 and temp_value > value:
            #                #print("3: " + str(self.movement_matrix[x - fx][y + fy]))
            #                value = temp_value
            #                new_x = x - fx
            #                new_y = y + fy
            #        if x - fx >= 0 and y - fy >= 0:
            #            #print("4: " + str(self.movement_matrix[x - fx][y - fy]))
            #            temp_value = self.movement_matrix[x - fx][y - fy] - rand.uniform(0, RandomNessFactor)**2
            #            if temp_value > value:
            #                #print("4: " + str(self.movement_matrix[x - fx][y - fy]))
            #                value = temp_value
            #                new_x = x - fx
            #                new_y = y - fy
            #    bee.move(new_x, new_y)

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