import numpy as np
import matplotlib.pyplot as plt

## constants ###########################

uniGravConst = 6.67408e-11 # m^3 kg^-1 s^-2
θ_threshold = 0.5   # how much approximation is allowed
F_g = lambda m1, m2, r: (uniGravConst * m1 * m2) / r**2
eucl = lambda x1, x2: np.linalg.norm(x1 - x2)
x_limit, y_limit = 100, 100

## quadtree ############################

class Particle:
    def __init__(self, mass, position, angular_velocity):
        self.mass = mass
        self.position = position
        self.force = np.zeros(2)
        self.velocity = np.random.uniform(angular_velocity * position[0], -angular_velocity * position[1], 2)

# reset mass if particle interaction instead of center of mass
class Node:
    def __init__(self, center, size, length=0):
        self.center = center
        self.size = size
        self.length = length/2
        self.x_bb = np.array([center[0] - length, center[0] + length])
        self.y_bb = np.array([center[1] - length, center[1] + length])
        self.mass = 0
        self.external = None
        self.children = [None] * 4
        

    def insert(self, particle):
        if self.external is None and self.children[0] is None:
            # print("inserting", particle.position, "within", self.center, self.x_bb, self.y_bb)
            self.external = particle
            self.mass = particle.mass
        else:
            if self.children[0] is None:
                self.subdivide()
            for i in range(4):
                if self.children[i].contains(self.external):
                    self.children[i].insert(self.external)
                    self.external = None
                    # self.mass = 0   # for no mass
                    
                if self.children[i].contains(particle):
                    self.children[i].insert(particle)
            self.mass += particle.mass
            self.external = None

    def subdivide(self):
        x, y = self.center
        # print("dividing", self.center, self.length)
        self.children[0] = Node(np.array([x - self.length, y - self.length]), self.size/4, self.length)   # bottom left
        self.children[1] = Node(np.array([x + self.length, y - self.length]), self.size/4, self.length)   # bottom right
        self.children[2] = Node(np.array([x - self.length, y + self.length]), self.size/4, self.length)   # top left
        self.children[3] = Node(np.array([x + self.length, y + self.length]), self.size/4, self.length)   # top right

    def contains(self, particle):
        if particle is None:
            return False
        x, y = particle.position
        return (self.x_bb[0] <= x <= self.x_bb[1] and
                self.y_bb[0] <= y <= self.y_bb[1])
    
def print_tree(node, particles, depth=0):
    indent = "        " * depth
    if node.external is not None:
        particles.remove(node.external)
        print(f"{indent}Leaf: {node.external.mass} @ {node.external.position}")
    else:
        print(f"{indent}Node: {node.mass} @ {node.center}")
        for child in node.children:
            if child is not None:
                print_tree(child, particles, depth+1)

def generate_random_particles(num_particles):
    particles = []
    external_mass = 0
    for i in range(num_particles):
        if i == 0:
            position = np.array([x_limit/2, y_limit/2])
            mass = 1e6
            particles.append(Particle(mass, position, 0))
            external_mass += mass
            continue

        angle = np.random.uniform(0, 2*np.pi)
        radius = np.random.uniform(0, 3*x_limit/2)
        position = np.array([radius * np.cos(angle), radius * np.sin(angle)])
        r = np.linalg.norm(position)
        mass = np.random.uniform(18, 50)
        external_mass += mass
        angular_velocity = np.sqrt(uniGravConst * external_mass / r**3)
        particles.append(Particle(mass, position, angular_velocity))
    return particles



## barnes-hut interaction ##############

def update_forces(particles, root):
    for particle in particles:
        particle.force = 0
        calculate_force(particle, root)

def calculate_force(particle, node):
    if node.external is None:
        d = eucl(node.center, particle.position)
        if node.size / d < θ_threshold:
            f = F_g(node.mass, particle.mass, d)
            particle.force += f * (node.center - particle.position) / d + 1e-10
        else:
            for child in node.children:
                if child is not None:
                    calculate_force(particle, child)
    else:
        if node.external is not particle:
            d = eucl(node.external.position, particle.position)
            f = F_g(node.external.mass, particle.mass, d)
            particle.force += f * (node.external.position - particle.position) / d + 1e-10

def update_positions(particles, time_step):
    for particle in particles:
        acceleration = particle.force / particle.mass # F=ma
        particle.velocity += acceleration * time_step
        particle.position += particle.velocity * time_step

def calculate_theta(particle, node):
    d = eucl(node.center, particle.position)
    return node.size / d

