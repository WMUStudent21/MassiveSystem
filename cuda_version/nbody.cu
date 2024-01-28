#include "nbody.h"

// nvcc -o o nbody.cu

int main() {
    // Initialize particle size and duration
    int num_particles = 10000;
    const int num_steps = 10000;
    const int block_size = 256;
    const int num_blocks = (num_particles + block_size - 1) / block_size;
    printf("block size: %d and num blocks: %d\n", block_size, num_blocks);
    
    // Generate n particles
    Particle* particles = generate_random_particles(&num_particles,x_limit,y_limit,G);

    float start_cpu, finish_cpu, elapsed_cpu, elapsedTime;
    cudaEvent_t start, stop;

    // start time
    GET_TIME(start_cpu);
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start, 0);

    // Simulate force interactions
    for (int time_step = 1; time_step < num_steps; time_step++) {

        // Create root node
        Node* root = create_node(x_limit/2, y_limit/2, 1, x_limit/2);
        for (int i = 0; i < num_particles; i++) {
            insert(root, &particles[i]);
        }

        Particle* d_particles;
        cudaMalloc(&d_particles, num_particles*sizeof(Particle));
        cudaMemcpy(d_particles, particles, num_particles*sizeof(Particle), cudaMemcpyHostToDevice);
        
        Node* d_root;
        cudaMalloc(&d_root, sizeof(Node));
        cudaMemcpy(d_root, root, sizeof(Node), cudaMemcpyHostToDevice);

        // calculate forces in GPU
        calculate_force_kernel<<<num_blocks, block_size>>>(d_particles, d_root, num_particles);
        update_positions_kernel<<<num_blocks, block_size>>>(d_particles, time_step, num_particles);
        
        // particle_print_gpu<<<num_blocks, block_size>>>(d_particles, num_particles);
        // printf("----\n");

        cudaMemcpy(root, d_root, sizeof(Node), cudaMemcpyDeviceToHost);
        cudaMemcpy(particles, d_particles, num_particles * sizeof(Particle), cudaMemcpyDeviceToHost);

        cudaFree(d_particles);
        cudaFree(d_root);
        free_tree(root);
        root = NULL;
    }

    // end time
    cudaDeviceSynchronize();
    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&elapsedTime, start, stop);
    printf("Time to generate:  %e s\n", elapsedTime/1000);
    GET_TIME(finish_cpu);
    elapsed_cpu = finish_cpu - start_cpu;
    printf("Elapsed time = %e seconds\n", elapsed_cpu);
    
    // Free memory for particles
    free(particles);

    particles = NULL;
    return 0;
}
