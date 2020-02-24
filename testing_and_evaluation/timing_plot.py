import matplotlib.pyplot as plt
import numpy as np

x = [100, 200, 300, 400, 500, 600, 700, 800, 1000, 1200, 1500]
y = [83.76, 65.52, 46.99, 34.99, 26.44, 19.63, 15.78, 13.04, 8.39, 6.13, 5.38]

fig, ax = plt.subplots(1, 1)
ax.plot(x, y)
ax.set_xlabel("Number of Bees")
ax.set_ylabel("Frames Per Second")
