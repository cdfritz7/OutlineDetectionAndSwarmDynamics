import glob
import moviepy.editor as mpy
import random
import math
import matplotlib.pyplot as plt
import time

#gen_particles = [[(x*10, y*10) for x in range(-19,20)] for y in range(-19,20)]
gen_particles = [[(x*50, y*50) for x in range(-200//50,200//50+1)] for y in range(-200//50,200//50+1)]

particles = []
for l in gen_particles:
    for p in l:
        particles.append(p)

attractors = list((x, x) for x in range(-200,201))

repellent_constant = 2000.0
attraction_constant = 1000.0


def potential(x, y, particle_num):
    res = 0
    for i in range(len(particles)):
        if particle_num != i:
            p_x = particles[i][0]
            p_y = particles[i][1]
            dist_squared = (x-p_x)**2 + (y-p_y)**2
            dist = math.sqrt(dist_squared)
            if dist != 0 :
                res += repellent_constant/dist#dist_squared
            else:
                res += repellent_constant/0.0001
    for i in range(len(attractors)):
        a_x = attractors[i][0]
        a_y = attractors[i][1]
        dist_squared = (x-a_x)**2 + (y-a_y)**2
        dist = math.sqrt(dist_squared)
        if(dist != 0):
            res -= attraction_constant/dist#/dist_squared
        else:
            res -= attraction_constant/0.00001
    return res


def step_particle(x, y, num_particle):
    x_incr = random.randint(-1, 1)
    y_incr = random.randint(-1, 1)
    #print("old loc: {}, {}".format(x, y))
    #print("new loc: {}, {}".format(x+x_incr, y+y_incr))
    if x_incr != 0 or y_incr != 0:
        old_potential = potential(x, y, num_particle)
        new_potential = potential(x+x_incr, y+y_incr, num_particle)
        #print("Old potential: {}".format(old_potential))
        #print("New potential: {}".format(new_potential))
        #prob_of_move = 1 - 1.0/(1.0 + math.exp(-(old_potential - new_potential)))
        #print("prob_of_move: {}".format(prob_of_move))
        #print()
        #print()
        if new_potential < old_potential: #random.uniform(0,1) < prob_of_move:
            #print("took move\n")
            return x + x_incr, y + y_incr
    return x, y

def plot_sim():
    plt.clf()
    plt.scatter(list(attractors[i][0] for i in range(len(attractors))),
                list(attractors[i][1] for i in range(len(attractors))), color='b')
    plt.scatter(list(particles[i][0] for i in range(len(particles))),
                list(particles[i][1] for i in range(len(particles))), color='r')
    plt.xlim(right=200, left=-200)
    plt.ylim(top=200,bottom=-200)
    #plt.show()

def step_all_particles():
    for i in range(len(particles)):
        (x,y) = step_particle(particles[i][0], particles[i][1], i)
        particles[i] = (x,y)

file_names = []
num_frames = 30*25
for i in range(num_frames):
    plot_sim()
    step_all_particles()
    file_name = "Images/particle_sim_img_{0:03d}.png".format(i)
    file_names.append(file_name)
    plt.savefig(file_name, dpi=96)
    print("Saved img {} of {}".format(i,num_frames))

def mk_gif():
    gif_name = 'out.gif'
    fps = 60
    file_list = glob.glob('Images/*.png')
    #print(file_list)
    #list.sort(file_list, key=lambda x: int(x.split('_')[3]))
    list.sort(file_list, key=lambda x: int(x.split('_')[-1].split('.png')[0]))
    clip = mpy.ImageSequenceClip(file_list, fps=fps)
    clip.write_gif(gif_name, fps=fps)

mk_gif()
