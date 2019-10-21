"""
measurements taken to test the conversion of
libfreenect's scaled (by 255/2048) 11bit distance  metric

cutoff = 46.7 cm
70 - 62.2 cm
80 - 73.9 cm
90 - 91.6 cm
95 - 103.1 cm
100 - 117.8 cm
105 - 137.5 cm
110 - 165.5 cm
112 - 175.3 cm
"""

import numpy as np
import matplotlib.pyplot as plt

cutoffs = [70, 80, 90, 95, 100, 105, 110, 112]
distances = [62.2, 73.9, 91.6, 103.1, 117.8, 137.5, 165.5, 175.3]

plt.scatter(cutoffs, distances)
plt.show()
