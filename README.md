Parallelisation implementation of the Barnes-Hut algorithm

Started with prototype implementation in Python3.11 as {nbody.py, utils.py} | up to n=1000 particles Refactored into C code as {main.c, nbody.c, nbody.h} | up to n=100_000 particles Parallelised with OpenMP pragma parameters
Further optimised with CUDA program.

P.S. Borrowed timer.h from the OpenMP resources.
