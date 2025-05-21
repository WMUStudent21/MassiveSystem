# N-Body Simulation with Barnes-Hut Algorithm: A Performance Study

## Overview

This project implements the Barnes-Hut algorithm for simulating N-body gravitational interactions. The primary goal is to explore and demonstrate performance improvements by transitioning the simulation from a high-level prototype in Python to optimized versions in C with OpenMP for multi-core parallelism, and finally to a massively parallel implementation using CUDA for GPU acceleration.

The N-body problem is a classical problem in physics and astrophysics that predicts the individual motions of a group of celestial objects interacting gravitationally. The Barnes-Hut algorithm is an approximation algorithm that reduces the computational complexity from O(N^2) to O(N log N) by treating distant groups of particles as single, larger particles.

## Features

* **Barnes-Hut Algorithm:** Core N-body simulation logic using a quadtree (or octree in 3D, though this project is 2D) to approximate gravitational forces.
* **Multiple Implementations:**
    * **Python:** A prototype version for algorithm validation and ease of development.
    * **C with OpenMP:** A C language refactor focused on performance, utilizing OpenMP for shared-memory parallelism to leverage multi-core CPUs.
    * **CUDA:** A high-performance version leveraging NVIDIA's CUDA platform for massively parallel computation on GPUs.
* **Visualization:** The C version includes an SDL2-based visualizer to observe the particle simulation. The Python version uses Matplotlib for animation.

## Project Structure

The repository is organized as follows:

├── python_version/         # Python implementation
│   ├── nbody.py            # Main simulation script with visualization
│   └── utils.py            # Particle, Node classes, and utility functions
├── c_version/              # C and OpenMP implementation
│   ├── main.c              # Main program with SDL2 visualization
│   ├── nbody.c             # Core simulation logic, can be compiled for non-visual runs
│   ├── nbody.h             # Header file for C structs and function prototypes
│   └── timer.h             # Timer utility (borrowed from OpenMP resources)
├── cuda_version/           # CUDA C++ implementation
│   ├── nbody.cu            # Main CUDA kernel and host code
│   ├── nbody.h             # Header file for CUDA structs and function prototypes
│   └── timer.h             # (Assumed similar timer or CUDA events for timing)
├── README.md               # This file
└── requirements.txt        # Python dependencies (consider making this specific to the project)


## Implementations

### 1. Python Version

* **Description:** A clear, high-level implementation primarily for algorithm correctness testing and rapid prototyping.
* **Files:** `python_version/nbody.py`, `python_version/utils.py`
* **Characteristics:** Handles up to approximately N=1000 particles.
* **Dependencies:**
    * NumPy
    * Matplotlib
    * (Update `requirements.txt` for specific project needs)
* **To Run:**
    ```bash
    cd python_version
    python nbody.py
    # (Follow prompts for number of particles)
    ```

### 2. C Version with OpenMP

* **Description:** A refactor of the Python prototype into C for improved performance, with OpenMP directives added to parallelize computationally intensive loops (force calculation, position updates) across CPU cores.
* **Files:** `c_version/main.c` (SDL visualization), `c_version/nbody.c`, `c_version/nbody.h`
* **Characteristics:** Handles up to approximately N=100,000 particles.
* **Dependencies:**
    * A C compiler (e.g., GCC)
    * OpenMP library (usually included with modern GCC)
    * SDL2 library (for `main.c` visualization)
* **To Compile & Run (Example with GCC for `main.c`):**
    ```bash
    cd c_version
    # For SDL visualization version:
    gcc -o nbody_vis main.c nbody.c -lSDL2 -lm -fopenmp -Wall
    ./nbody_vis <num_threads> <num_particles>

    # For non-visual benchmark version (if nbody.c has its own main):
    # gcc -o nbody_benchmark nbody.c -lm -fopenmp -Wall
    # ./nbody_benchmark <num_threads> <num_particles>
    ```
    *(Please verify and update these compile commands based on your exact setup and how `nbody.c`'s main is structured)*

### 3. CUDA Version

* **Description:** The C implementation further optimized by porting the core computational tasks (primarily force calculation and particle updates) to NVIDIA's CUDA platform, enabling execution on a GPU for massive parallelism.
* **Files:** `cuda_version/nbody.cu`, `cuda_version/nbody.h`
* **Characteristics:** Designed for significant performance gains with large N, leveraging GPU architecture.
* **Dependencies:**
    * NVIDIA CUDA Toolkit (nvcc compiler)
    * NVIDIA GPU with CUDA support
* **To Compile & Run (Example with nvcc):**
    ```bash
    cd cuda_version
    nvcc -o nbody_cuda nbody.cu # Add any necessary library links or architecture flags
    ./nbody_cuda # Add any command line arguments
    ```
    *(Please verify and update these compile commands and arguments)*

## Performance

This project demonstrates a clear progression in performance capabilities:

* **Python:** Baseline, suitable for smaller N.
* **C/OpenMP:** Significant speedup over Python, capable of handling much larger N by utilizing CPU cores.
* **CUDA:** Aims for the highest performance by offloading parallel computations to the GPU, ideal for very large N.

*(Consider adding a small table or graph here with benchmark results: N particles vs. time_per_step for each version, if available.)*

## Acknowledgements

* The `timer.h` utility used in the C and potentially CUDA versions was adapted from OpenMP resources.
