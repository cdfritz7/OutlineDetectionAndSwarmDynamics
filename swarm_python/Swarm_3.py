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
        self.flower_array = []

    def add_bee(self, x: int = None, y: int = None):
        if x is None:
            x = int(rand.uniform(0, self.MatrixLength-0.5))
            y = int(rand.uniform(0, self.MatrixHeight-0.5))

        self.bee_array.append([x, y])

    def add_flower(self, x: int = None, y: int = None):
        if x is None:
            x = int(rand.uniform(0, self.MatrixLength - 0.5))
            y = int(rand.uniform(0, self.MatrixHeight - 0.5))

        self.flower_array.append([x, y])

    def set_flower_array(self, flower_array):
        self.flower_array = flower_array

    def update_movement(self):
        bee_flower = [None] * len(self.bee_array)
        flower_bee = [None] * len(self.flower_array)

        # if num bees > num flowers
        if len(self.bee_array) > len(self.flower_array):
            # keeps track of what flowers are still in consideration
            flower_idx = [0] * len(self.bee_array)

            # keeps track of bees to find flowers for
            bee_queue = list(range(len(self.bee_array)))

            # for every bee in bee_queue
            for b_idx in bee_queue:
                bee = self.bee_array[b_idx]

                # idx of flower to consider
                f_idx = flower_idx[b_idx]

                # while no flower picked and flowers still in consideration:
                while bee_flower[b_idx] is None and f_idx < len(self.flower_array):
                    flower = self.flower_array[f_idx]
                    bee_2 = self.bee_array[flower_bee[f_idx]]

                    # if there is no other bee that has claimed that flower
                    if bee_2 is None:
                        # claim flower
                        bee_flower[b_idx] = f_idx
                        flower_bee[f_idx] = b_idx
                        flower_idx[b_idx] += 1
                    else:
                        # calculate distance from bee to flower
                        dist_x = flower[0] - bee[0]
                        dist_y = flower[1] - bee[1]
                        bee_dist = sqrt(dist_x ** 2 + dist_y ** 2)

                        dist_x = flower[0] - bee_2[0]
                        dist_y = flower[1] - bee_2[1]
                        bee_2_dist = sqrt(dist_x ** 2 + dist_y ** 2)

                        # if current bee is closer than bee that already claimed flower
                        if bee_dist < bee_2_dist:
                            # take flower from bee 2 and re-add bee 2 to queue
                            bee_flower[flower_bee[f_idx]] = None
                            bee_queue.append(flower_bee[f_idx])
                            flower_bee[f_idx] = b_idx
                            bee_flower[b_idx] = f_idx
                        else:
                            # consider the next flower
                            f_idx += 1

                # ensure flower_idx is updated
                flower_idx[b_idx] = f_idx
        else:
            bee_idx = [0] * len(self.flower_array)

            flower_queue = list(range(len(self.flower_array)))

            for f_idx in flower_queue:
                flower = self.flower_array[f_idx]

                b_idx = bee_idx[f_idx]

                while flower_bee[f_idx] is None and b_idx < len(self.bee_array):
                    bee = self.bee_array[b_idx]
                    flower_2 = self.flower_array[bee_flower[b_idx]]

                    if flower_2 is None:
                        flower_bee[f_idx] = b_idx
                        bee_flower[b_idx] = f_idx
                        bee_idx[f_idx] += 1
                    else:
                        dist_x = flower[0] - bee[0]
                        dist_y = flower[1] - bee[1]
                        flower_dist = sqrt(dist_x ** 2 + dist_y ** 2)

                        dist_x = flower_2[0] - bee[0]
                        dist_y = flower_2[1] - bee[1]
                        flower_2_dist = sqrt(dist_x ** 2 + dist_y ** 2)

                        if flower_dist < flower_2_dist:
                            flower_bee[bee_flower[b_idx]] = None
                            flower_queue.append(bee_flower[b_idx])
                            bee_flower[b_idx] = f_idx
                            flower_bee[f_idx] = b_idx
                        else:
                            b_idx += 1
                bee_idx[f_idx] = b_idx

        for i in range(len(self.bee_array)):
            flower = self.flower_array[bee_flower[i]]
            bee = self.bee_array[i]

            dist_x = flower[0] - bee[0]
            dist_y = flower[1] - bee[1]

            dist = sqrt(dist_x ** 2 + dist_y ** 2)

            if dist > self.MoveMax:
                # tan = y/x
                rads = atan2(dist_y, dist_x) + rand.uniform(-1 * self.RandomNessFactor, self.RandomNessFactor)

                if not (dist_x == 0 and dist_y == 0):
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
                bee[0] = self.flower_array[bee_flower[i]][0]
                bee[1] = self.flower_array[bee_flower[i]][1]


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