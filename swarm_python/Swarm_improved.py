import glob
import moviepy.editor as mpy
import matplotlib.pyplot as plt
import random as rand
from math import *
from graphics import *
import time

MatrixHeight = 400
MatrixLength = 400
MoveMax = 10
# if 0, global
Bee_Power = 1.25
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
        # for every bee
        for i in range(len(self.bee_array)):
            bee = self.bee_array[i]

            # If the bee isn't on a flower
            if self.bee_flower[i] is None:
                total_x = 0
                total_y = 0

                # for every bee that isn't the original bee and is not on a flower
                for j in range(len(self.bee_array)):
                    if j != i and self.bee_flower[j] == None:
                        bee2 = self.bee_array[j]

                        # compare the distance between the two bees
                        dist_x = bee2[0] - bee[0]
                        dist_y = bee2[1] - bee[1]

                        dist = sqrt(dist_x ** 2 + dist_y ** 2)
                        dist = (MatrixMaxMag - dist)**2

                        # move original bee away from the other bee
                        total_x -= dist_x * dist * (Bee_Power/Flower_Power)
                        total_y -= dist_y * dist * (Bee_Power/Flower_Power)

                flower_in_range = -1
                flower_dist = None
                num_empty_flowers = 0

                # for every flower that  doesn't have a bee on it
                for j in range(len(self.flower_array)):
                    if self.flower_bee[j] == None:
                        num_empty_flowers += 1
                        flower = self.flower_array[j]

                        # compare the distance between the bee and the flower
                        dist_x = flower[0] - bee[0]
                        dist_y = flower[1] - bee[1]

                        dist = sqrt(dist_x ** 2 + dist_y ** 2)

                        # if the flower is within a single step and is closer than any other flower in range, record
                        if dist < MoveMax and (flower_in_range < 0 or dist >= flower_dist):
                            flower_in_range = j
                            flower_dist = dist

                        dist = (MatrixMaxMag - dist)**2

                        # move bee towards flower
                        total_x += dist_x * dist
                        total_y += dist_y * dist

                if num_empty_flowers == 0:
                    total_x = rand.uniform(-1,1)
                    total_y = rand.uniform(-1,1)

                # if there was no flower in range, move bee towards optimal position
                if flower_in_range == -1:
                    # tan = y/x
                    rads = atan2(total_y, total_x) + rand.uniform(-1*RandomNessFactor, RandomNessFactor)

                    if not (total_x == 0 and total_y == 0):
                        # x = cos(rads) * z
                        new_x = bee[0] + int(cos(rads) * MoveMax)

                        # y = sin(rads) * z
                        new_y = bee[1] + int(sin(rads) * MoveMax)

                        if new_x < 0 or new_x >= MatrixLength:
                            bee[0] = bee[0] - int(cos(rads) * MoveMax)
                        else:
                            bee[0] = new_x

                        if new_y < 0 or new_y >= MatrixHeight:
                            bee[1] = bee[1] - int(sin(rads) * MoveMax)
                        else:
                            bee[1] = new_y

                # if there was a flower in range, move bee to flower
                else:
                    self.bee_flower[i] = flower_in_range
                    self.flower_bee[flower_in_range] = i
                    bee[0] = self.flower_array[flower_in_range][0]
                    bee[1] = self.flower_array[flower_in_range][1]

            # if the bee is already on a flower
            else:
                flower_in_range = -1
                flower_dist = None
                for j in range(len(self.flower_array)):
                    if self.flower_bee[j] == None:
                        flower = self.flower_array[j]
                        dist_x = flower[0] - bee[0]
                        dist_y = flower[1] - bee[1]

                        dist = sqrt(dist_x ** 2 + dist_y ** 2)
                        if dist < MoveMax and (flower_in_range < 0 or dist > flower_dist):
                            flower_in_range = j
                            flower_dist = dist

                    # if there is a flower in range
                    if flower_in_range != -1:
                        # clear previous flower
                        self.flower_bee[self.bee_flower[i]] = None

                        # move bee to new flower
                        self.bee_flower[i] = flower_in_range
                        self.flower_bee[flower_in_range] = i
                        bee[0] = self.flower_array[flower_in_range][0]
                        bee[1] = self.flower_array[flower_in_range][1]

def mk_gif():
    gif_name = 'out.gif'
    fps = 15
    file_list = glob.glob('Images/*.png')
    #print(file_list)
    #list.sort(file_list, key=lambda x: int(x.split('_')[3]))
    list.sort(file_list, key=lambda x: int(x.split('_')[-1].split('.png')[0]))
    clip = mpy.ImageSequenceClip(file_list, fps=fps)
    clip.write_gif(gif_name, fps=fps)

def quick_demo():
    bee_handle = BeeHandle()
    win = GraphWin("Bees", MatrixLength, MatrixHeight)
    for i in range(400):
        bee_handle.add_bee()

    flower_idx = list(range(MatrixLength))
    rand.shuffle(flower_idx)
    for idx in flower_idx:
        bee_handle.add_flower(idx, idx)
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
        print(1 / time_passed)
        start_time = time.time()

def make_gif():
    bee_handle = BeeHandle()
    for i in range(400):
        bee_handle.add_bee()

    flower_idx = list(range(MatrixLength))
    rand.shuffle(flower_idx)
    for idx in flower_idx:
        bee_handle.add_flower(idx, idx)

    fps = 15
    num_frames = fps * 15
    for i in range(num_frames):
        # Reset plot
        plt.clf()
        plt.xlim(0, MatrixLength)
        plt.ylim(0, MatrixHeight)

        # plot bees and flowers
        x_bee = []
        y_bee = []
        for bee in bee_handle.bee_array:
            x_bee.append(bee[0])
            y_bee.append(bee[1])
        x_flower = []
        y_flower = []
        for flower in bee_handle.flower_array:
            x_flower.append(flower[0])
            y_flower.append(flower[1])

        plt.scatter(x_flower, y_flower, color='b')
        plt.scatter(x_bee, y_bee, color='r')

        # update movement
        bee_handle.update_movement()

        file_name = "Images/particle_sim_img_{0:03d}.png".format(i)
        plt.savefig(file_name, dpi=96)

        print(file_name)
    mk_gif()

def main():
    quick_demo()

if __name__ == "__main__":
    main()
