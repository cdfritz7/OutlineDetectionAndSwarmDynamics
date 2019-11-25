import re
import glob
import moviepy.editor as mpy
import matplotlib.pyplot as plt
import random as rand
from math import *
from graphics import *
import time


class BeeHandle:
    def __init__(self, matrix_height: int = 400, matrix_length: int = 400, move_max: int = 10,
                 random_factor: float = (pi/4)):
        self.MatrixHeight = matrix_height
        self.MatrixLength = matrix_length
        self.MoveMax = move_max
        self.RandomNessFactor = random_factor
        self.MatrixMaxMag = sqrt(self.MatrixHeight**2 + self.MatrixLength**2)
        self.bee_array = []
        self.bee_flower = []
        self.flower_array = []
        self.flower_bee = []

    def add_bee(self, x: int = None, y: int = None):
        if x is None:
            x = int(rand.uniform(0, self.MatrixLength-0.5))
            y = int(rand.uniform(0, self.MatrixHeight-0.5))

        self.bee_array.append([x, y])
        self.bee_flower.append(None)

    def add_flower(self, x: int = None, y: int = None):
        if x is None:
            x = int(rand.uniform(0, self.MatrixLength - 0.5))
            y = int(rand.uniform(0, self.MatrixHeight - 0.5))

        self.flower_array.append([x, y])
        self.flower_bee.append(None)

    def set_flower_array(self, flower_array):
        self.flower_array = flower_array
        self.flower_bee = [None] * len(self.flower_array)
        self.bee_flower = [None] * len(self.bee_array)

    def update_movement(self):
        # for every bee
        for i in range(len(self.bee_array)):
            bee = self.bee_array[i]

            # If the bee isn't on a flower
            if self.bee_flower[i] is None:
                total_x = 0
                total_y = 0

                closest_dist = None

                flower_in_range = -1
                flower_dist = None
                num_empty_flowers = 0

                # for every flower that  doesn't have a bee on it
                for j in range(len(self.flower_array)):
                    if self.flower_bee[j] is None:
                        num_empty_flowers += 1
                        flower = self.flower_array[j]

                        # compare the distance between the bee and the flower
                        dist_x = flower[0] - bee[0]
                        dist_y = flower[1] - bee[1]

                        dist = sqrt(dist_x ** 2 + dist_y ** 2)

                        # if the flower is within a single step and is closer than any other flower in range, record
                        if dist < self.MoveMax and (flower_in_range < 0 or dist >= flower_dist):
                            flower_in_range = j
                            flower_dist = dist

                        # move bee towards flower
                        if closest_dist is None or dist < closest_dist:
                            closest_dist = dist
                            total_x = dist_x
                            total_y = dist_y

                if num_empty_flowers == 0:
                    total_x = rand.uniform(-1, 1)
                    total_y = rand.uniform(-1, 1)

                # if there was no flower in range, move bee towards optimal position
                if flower_in_range == -1:
                    # tan = y/x
                    rads = atan2(total_y, total_x) + rand.uniform(-1*self.RandomNessFactor, self.RandomNessFactor)

                    if not (total_x == 0 and total_y == 0):
                        # x = cos(rads) * z
                        new_x = bee[0] + int(cos(rads) * self.MoveMax)

                        # y = sin(rads) * z
                        new_y = bee[1] + int(sin(rads) * self.MoveMax)

                        if new_x < 0 or new_x >= self.MatrixLength:
                            bee[0] = bee[0] - int(cos(rads) * self.MoveMax)
                        else:
                            bee[0] = new_x

                        if new_y < 0 or new_y >= self.MatrixHeight:
                            bee[1] = bee[1] - int(sin(rads) * self.MoveMax)
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
                    if self.flower_bee[j] is None:
                        flower = self.flower_array[j]
                        dist_x = flower[0] - bee[0]
                        dist_y = flower[1] - bee[1]

                        dist = sqrt(dist_x ** 2 + dist_y ** 2)
                        if dist < self.MoveMax and (flower_in_range < 0 or dist > flower_dist):
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

def mk_gif(fps: int = 30):
    gif_name = 'out.gif'
    file_list = glob.glob('Images/*.png')
    #print(file_list)
    #list.sort(file_list, key=lambda x: int(x.split('_')[3]))
    list.sort(file_list, key=lambda x: int(x.split('_')[-1].split('.png')[0]))
    clip = mpy.ImageSequenceClip(file_list, fps=fps)
    clip.write_gif(gif_name, fps=fps)


def quick_demo(MatrixLength: int = 400, MatrixHeight: int = 400, bee_points: list = None, flower_frames: list = None):
    bee_handle = BeeHandle(MatrixHeight, MatrixLength)

    win = GraphWin("Bees", MatrixLength, MatrixHeight)

    if bee_points is None:
        for i in range(800):
            bee_handle.add_bee()
    else:
        for point in bee_points:
            bee_handle.add_bee(int(point[0]), int(point[1]))

    Points = []
    for bee in bee_handle.bee_array:
        Points.append(Point(bee[0], bee[1]))
    for point in Points:
        point.draw(win)
    start_time = None

    if flower_frames is None:
        flower_idx = list(range(MatrixLength))
        rand.shuffle(flower_idx)
        for idx in flower_idx:
            bee_handle.add_flower(idx, idx)

    frame_idx = 0
    while flower_frames is None or frame_idx < len(flower_frames):
        if flower_frames is not None and (start_time is None or time.time() - start_time > 0.5):
            start_time = time.time()
            bee_handle.set_flower_array(flower_frames[frame_idx])
            frame_idx += 1

        bee_handle.update_movement()
        for i in range(len(bee_handle.bee_array)):
            Points[i].move(bee_handle.bee_array[i][0] - Points[i].getX(), bee_handle.bee_array[i][1] - Points[i].getY())

def make_gif(MatrixLength: int = 400, MatrixHeight: int = 400, fps: int = 30, time_len = 10, bee_points: list = None,
             flower_frames: list = None):
    bee_handle = BeeHandle(MatrixHeight, MatrixLength)

    if bee_points is None:
        for i in range(800):
            bee_handle.add_bee()
    else:
        for point in bee_points:
            bee_handle.add_bee(int(point[0]), int(point[1]))

    if flower_frames is None:
        flower_idx = list(range(MatrixLength))
        rand.shuffle(flower_idx)
        for idx in flower_idx:
            bee_handle.add_flower(idx, idx)

    frames_idx = 0
    num_frames = fps * time_len
    for i in range(num_frames):
        if flower_frames is not None and frames_idx < len(flower_frames) and i % int(fps/10) == 0:
            bee_handle.set_flower_array(flower_frames[frames_idx])
            frames_idx += 1

        # Reset plot
        plt.clf()
        plt.xlim(0, MatrixLength)
        plt.ylim(-1 * MatrixHeight, 0)

        # plot bees and flowers
        x_bee = []
        y_bee = []
        for bee in bee_handle.bee_array:
            x_bee.append(bee[0])
            y_bee.append(-1 * bee[1])
        x_flower = []
        y_flower = []
        for flower in bee_handle.flower_array:
            x_flower.append(flower[0])
            y_flower.append(-1 * flower[1])

        plt.scatter(x_flower, y_flower, color='b')
        plt.scatter(x_bee, y_bee, color='r')

        # update movement
        bee_handle.update_movement()

        file_name = "Images/particle_sim_img_{0:03d}.png".format(i)
        plt.savefig(file_name, dpi=96)

        print(file_name)
    mk_gif(fps)


def read_contours(filename: str):
    file_in = None
    with open(filename) as f:
        file_in = f.read()

    if file_in is not None:
        # Get each frame
        frames = file_in.split(":")

        points_per_frame = []

        for frame in frames:
            # Get all points in frame
            points = re.findall("\[([0-9]*?),([0-9]*?)\]", frame)
            # Make sure all points are unique
            points = list(set(points))

            for i in range(len(points)):
                points[i] = (int(points[i][0]), int(points[i][1]))

            points_per_frame.append(points)

        return points_per_frame
    else:
        return None


def main():
    make_gif(700, 500, flower_frames=read_contours("video_contours.txt"))


if __name__ == "__main__":
    main()