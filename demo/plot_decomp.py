
"""
This script reads JSON input from stdin and creates a matplotlib figure
showing input and output of decomp program.
"""
import json
import matplotlib.pyplot as plt
import numpy as np
import random
import sys
random.seed(666)

input_data = sys.stdin.read()
data = json.loads(input_data)

def get_polygon(vertices, indices):
    if indices[0] != indices[-1]:
        indices += [indices[0]]
    return vertices[indices, :]


def plot_polygon(vertices, indices):
    polygon = get_polygon(vertices, indices)
    plt.plot(polygon[:,0], polygon[:,1])


plt.subplot(2, 1, 1)
vertices = np.array(data["vertices"])
plot_polygon(vertices, data["input"]["outer"])
for hole_indices in data["input"]["holes"]:
    plot_polygon(vertices, hole_indices)
plt.grid()


sp = plt.subplot(2, 1, 2)
for convex_polygon_indices in data["output"]:
    color = tuple(random.random() for _ in range(3))
    convex_polygon = get_polygon(vertices, convex_polygon_indices)
    plt.fill(convex_polygon[:, 0], convex_polygon[:, 1], fc=color, alpha=0.8)
plt.grid()

plt.show()
