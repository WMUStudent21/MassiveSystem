#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include "nbody.h"

// gcc -o o main.c -lSDL2 -lm -fopenmp -Wall && ./o 8 %% rm ./o

int main(int argc, char* argv[]) {
    int     thread_count;
    int     num_particles;

    Get_args(argc, argv, &thread_count, &num_particles);

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Create a window
    SDL_Window* window = SDL_CreateWindow("N-Body Simulation", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, x_limit, y_limit, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Create a renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Generate particles
    Particle* particles = generate_random_particles(&num_particles,x_limit,y_limit,G);

    // printf("center of root node: (%f, %f)\n", root->center_x, root->center_y);  
    // print_tree(root, 0);

    // Simulation parameters
    int num_steps = 100000;

    // Simulation loop
    for (int step = 0; step < num_steps; step++) {

        Node* root = create_node(x_limit/2, y_limit/2, 1, x_limit/2);
        for (int i = 0; i < num_particles; i++) {
            insert(root, &particles[i]);
        }


        // Update forces and positions
        update_forces(particles, root, &num_particles, thread_count);
        update_positions(particles, 10*step, &num_particles, thread_count);

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Render particles
        for (int i = 0; i < num_particles; i++) {
            if (particles[i].mass >= 800) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            } else {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                // SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            }
            SDL_Rect rect = {particles[i].position_x, particles[i].position_y, 2, 2};
            SDL_RenderFillRect(renderer, &rect);
        }

        // Update the screen
        SDL_RenderPresent(renderer);

        free_tree(root);
        root = NULL;

    }

    // Free memory for particles
    free(particles);

    particles = NULL;

    // Clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}