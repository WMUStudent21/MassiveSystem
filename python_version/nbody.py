import numpy as np
import matplotlib.pyplot as plt
from utils import *

## main ################################

def visualizer(num_particles):
    import matplotlib.pyplot as plt
    import matplotlib.animation as animation
    from matplotlib.animation import FuncAnimation

    particles = generate_random_particles(num_particles)
    root = Node(np.array([x_limit/2, y_limit/2]), 1, x_limit/2)

    # insert the particles into the tree
    for particle in particles:
        root.insert(particle)
    num_steps = 100000

    fig, ax = plt.subplots()
    ax.set_xlim(-x_limit*10, x_limit*10)
    ax.set_ylim(-y_limit*10, y_limit*10)

    colors = ['r' if p.mass >= 1000 else 'b' for p in particles]

    scat = ax.scatter([p.position[0] for p in particles], [p.position[1] for p in particles], c=colors)

    def animate(i):
        update_forces(particles, root)
        update_positions(particles, i)

        colors = ['r' if p.mass >= 1000 else 'b' for p in particles]    
        scat.set_offsets([[p.position[0], p.position[1]] for p in particles])
        scat.set_color(colors)

        return scat,

    # show center of mass
    ax.scatter([root.center[0]], [root.center[1]], c='g', s=100)
    anim = FuncAnimation(fig, animate, frames=num_steps, interval=10, blit=True)
    plt.show()

def dry():

    particles = generate_random_particles(num_particles)
    root = Node(np.array([x_limit/2, y_limit/2]), 1, x_limit/2)

    # insert the particles into the tree
    for particle in particles:
        root.insert(particle)
    num_steps = 100

    for timestep in range(num_steps):
        update_forces(particles, root)
        update_positions(particles, timestep)

    print_tree(root, particles)
    [print(p.position) for p in particles]

if __name__ == '__main__':
    # dry()
    num_particles = int(input("Enter number of particles: "))

    visualizer(num_particles)







