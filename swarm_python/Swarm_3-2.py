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
                 random_factor: float = (pi/4), num_threads: int = 4):
        self.MatrixHeight = matrix_height
        self.MatrixLength = matrix_length
        self.MoveMax = move_max
        self.RandomNessFactor = random_factor
        self.MatrixMaxMag = sqrt(self.MatrixHeight**2 + self.MatrixLength**2)
        self.bee_array = []
        self.flower_array = []
        self.num_threads = num_threads

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
        threads = []
        idx_array = [0]

        if len(self.flower_array) >= len(self.bee_array):
            candidate_array = self.flower_array
            pick_array = self.bee_array
        else:
            candidate_array = self.bee_array
            pick_array = self.flower_array

        paired_idx = [0] * len(candidate_array)

        while len(idx_array) > 0:
            repeats = [None] * (len(pick_array) + 1)
            for idx in range(len(paired_idx)):
                if repeats[paired_idx[idx]] is None:
                    repeats[paired_idx[idx]] = []
                repeats[paired_idx[idx]].append(idx)

            idx_array = []

            for j in range(len(repeats)-1):
                if repeats[j] is not None and len(repeats[j]) > 1:
                    idx_array += repeats[j]

            subarray_size = len(idx_array) // self.num_threads
            subarray_rem = len(idx_array) % self.num_threads

            for k in range(self.num_threads):
                if len(idx_array) < self.num_threads * 3:
                    if k < 1:
                        threads.append(SMThread(0, len(idx_array), idx_array, paired_idx, candidate_array, pick_array))
                elif k < subarray_rem:
                    start = k * (subarray_size+1)
                    end = (k+1) * (subarray_size+1)
                    threads.append(SMThread(start, end, idx_array, paired_idx, candidate_array, pick_array))
                else:
                    start = subarray_rem * (subarray_size+1) + (k-subarray_rem) * subarray_size
                    end = subarray_rem * (subarray_size+1) + (k-subarray_rem+1) * subarray_size
                    threads.append(SMThread(start, end, idx_array, paired_idx, candidate_array, pick_array))

            for thread in threads:
                thread.stable_marriage()

            threads = []

        print(paired_idx)

        if len(self.bee_array) > len(self.flower_array):
            for i in range(len(paired_idx)):
                if paired_idx[i] == len(pick_array):
                    self.move_bee(self.bee_array[i])
                else:
                    self.move_bee(self.bee_array[i], self.flower_array[paired_idx[i]])
        else:
            for i in range(len(paired_idx)):
                bee_moved = [False]*len(self.bee_array)
                if paired_idx[i] != len(pick_array):
                    self.move_bee(self.bee_array[paired_idx[i]], self.flower_array[i])
                    bee_moved[paired_idx[i]] = False
                for idx, moved in enumerate(bee_moved):
                    if not moved:
                        self.move_bee(self.bee_array[idx])

    def move_bee(self, bee: list, flower: list = None):
        if flower is not None:
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
                bee[0] = flower[0]
                bee[1] = flower[1]
        else:
            dist_x = rand.uniform(-1, 1)
            dist_y = rand.uniform(-1, 1)

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

class SMThread:
    def __init__(self, start: int, end: int, idx_array: list, paired_idx: list, candidate_array: list,
                 pick_array: list):
        self.idx_array = idx_array[start:end]
        self.paired_idx = paired_idx
        self.candidate_array = candidate_array
        self.pick_array = pick_array

    def stable_marriage(self):
        if len(self.pick_array) == 0:
            return
        repeats = self.generate_repeats(self.idx_array, self.paired_idx)

        pick = 0
        while pick < len(self.pick_array):
            competitors = repeats[pick]

            if competitors is not None and len(competitors) > 1:
                closest_comp = None
                closest_mag = None

                for competitor in competitors:
                    competitor_coords = self.candidate_array[competitor]
                    pick_coords = self.pick_array[pick]
                    competitor_mag = self.get_square_mag(competitor_coords, pick_coords)

                    if closest_comp is None:
                        closest_comp = competitor
                        closest_mag = competitor_mag
                    elif closest_mag > competitor_mag:
                        self.paired_idx[closest_comp] += 1

                        closest_comp = competitor
                        closest_mag = competitor_mag
                    else:
                        self.paired_idx[competitor] += 1

                for competitor in competitors:
                    if competitor != closest_comp:
                        if repeats[pick + 1] is None:
                            repeats[pick + 1] = []
                        repeats[pick + 1].append(competitor)

                repeats[pick] = [closest_mag]
            pick += 1

    # O(N)
    def generate_repeats(self, idx_array: list, paired_idx: list):
        repeats = [None] * (len(self.pick_array) + 1)
        for idx in idx_array:
            if repeats[paired_idx[idx]] is None:
                repeats[paired_idx[idx]] = []
            repeats[paired_idx[idx]].append(idx)
        return repeats

    @staticmethod
    def get_square_mag(candidate, pick):
        x = candidate[0] - pick[0]
        y = candidate[1] - pick[1]

        return x**2 + y**2


def mk_gif(fps: int = 30):
    gif_name = 'out.gif'
    file_list = glob.glob('Images/*.png')
    #print(file_list)
    #list.sort(file_list, key=lambda x: int(x.split('_')[3]))
    list.sort(file_list, key=lambda x: int(x.split('_')[-1].split('.png')[0]))
    clip = mpy.ImageSequenceClip(file_list, fps=fps)
    clip.write_gif(gif_name, fps=fps)


def quick_demo(MatrixLength: int = 400, MatrixHeight: int = 400, num_bees: int = 800, bee_points: list = None,
               flower_frames: list = None):
    bee_handle = BeeHandle(MatrixHeight, MatrixLength)

    win = GraphWin("Bees", MatrixLength, MatrixHeight)

    if bee_points is None:
        for i in range(num_bees):
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
        if MatrixLength < MatrixHeight:
            flower_idx = list(range(MatrixLength))
        else:
            flower_idx = list(range(MatrixHeight))
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
    quick_demo(700,500,flower_frames=read_contours("video_contours.txt"))

if __name__ == "__main__":
    main()