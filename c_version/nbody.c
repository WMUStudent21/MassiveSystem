#include "nbody.h"

// gcc -o o main.c -lm -fopenmp -Wall

int main(int argc, char* argv[]) {
    int     thread_count;
    
    Get_args(argc, argv, &thread_count);

    double start, finish, elapsed;

    // Initialize particle size and duration

    int num_particles = 10000;
    int num_steps = 10000;

    // Generate n particles
    Particle* particles = generate_random_particles(&num_particles,x_limit,y_limit,G);

    // start time
    GET_TIME(start);

    // Simulate force interactions
    for (int time_step = 1; time_step < num_steps; time_step++) {

        // Create root node
        Node* root = create_node(x_limit/2, y_limit/2, 1, x_limit/2);
        for (int i = 0; i < num_particles; i++) {
            insert(root, &particles[i]);
        }

        update_forces(particles, root, &num_particles, thread_count);
        update_positions(particles, time_step, &num_particles, thread_count);


        free_tree(root);
        root = NULL;

        // if (time_step = 1) {
        //     print_tree(root, 0);
        // }
    }

    // end time
    GET_TIME(finish);
    elapsed = finish - start;
    printf("Elapsed time = %e seconds\n", elapsed);

    // Free memory for particles
    free(particles);

    particles = NULL;
    return 0;
}