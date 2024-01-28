#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <omp.h>
#include <time.h>
// #include "timer.h"

const double G = 6.673e-11;
const double THRESHOLD = 0.7;
const double x_limit = 1000;
const double y_limit = 1000;
const double pi = 3.14159265358979323846;
const double k = 5e-4;

//////////////////////////////////////////////////
//
//     QUAD   TREE    IMPLEMENTATION           ///
//
//////////////////////////////////////////////////

typedef struct Node {
    double center_x;
    double center_y;
    double size;
    double mass;
    double x_min;
    double x_max;
    double y_min;
    double y_max;
    double length;
    bool div;
    struct Particle* external;
    struct Node* nw;
    struct Node* ne;
    struct Node* sw;
    struct Node* se;
} Node;

typedef struct Particle {
    double mass;
    double position_x;
    double position_y;
    double force_x;
    double force_y;
    double velocity_x;
    double velocity_y;
} Particle;

void insert(Node* node, Particle* particle);
void subdivide(Node* node);
bool contains(Node* node, Particle* particle);
void print_tree(Node* node, int depth);
void free_tree(Node* node);
Node* create_node(double x, double y, double size, double length);


Particle* generate_random_particles(int* num_particles, double x_limit, double y_limit, double uniGravConst) {
    Particle* particles = (Particle*)malloc(*num_particles * sizeof(Particle));
    double external_mass = 0;
    srand(time(NULL));

    for (int i = 0; i < *num_particles; i++) {
        // if (i == 0) {
        //     particles[i].mass = 1500;
        //     particles[i].position_x = 2*x_limit/3;
        //     particles[i].position_y = 2*y_limit/3;
        //     particles[i].force_x = 0;
        //     particles[i].force_y = 0;
        //     particles[i].velocity_x = 0;
        //     particles[i].velocity_y = 0;
        //     external_mass += particles[i].mass;

        //     // printf("Particle %d\n", i);
        //     // printf("Mass: %0.1f\n", particles[i].mass);
        //     // printf("Position: %0.1f %0.1f\n", particles[i].position_x, particles[i].position_y);
        //     // printf("--------------------\n");
        //     continue;
        // }
        double angle = (double)rand() / RAND_MAX * ((double)2 * pi);
        double radius = (double)rand() / RAND_MAX * ((double)2.5 * x_limit/6);
        double mass = (double)rand() / RAND_MAX * (500 - 180) + 180;
        particles[i].position_x = radius * cos(angle);
        particles[i].position_y = radius * sin(angle);
        double r = sqrt(pow(particles[i].position_x,2) + pow(particles[i].position_y,2));
        external_mass += mass;
        double angular_velocity = sqrt(uniGravConst * external_mass / pow(r,3)) * 7e-2;
        particles[i].mass = mass;
        particles[i].force_x = 0;
        particles[i].force_y = 0;
        particles[i].velocity_x = (double)rand() / RAND_MAX * (angular_velocity * particles[i].position_x + angular_velocity * particles[i].position_y) - angular_velocity * particles[i].position_y;
        particles[i].velocity_y = (double)rand() / RAND_MAX * (angular_velocity * particles[i].position_x + angular_velocity * particles[i].position_y) - angular_velocity * particles[i].position_y;
        particles[i].position_x += x_limit/2;
        particles[i].position_y += y_limit/2;

        // printf("Particle %d\n", i);
        // printf("Mass: %0.1f\n", particles[i].mass);
        // printf("Position: %0.1f %0.1f\n", particles[i].position_x, particles[i].position_y);
        // printf("Velocity: %0.15f %0.15f\n", particles[i].velocity_x, particles[i].velocity_y);
        // printf("--------------------\n");
    }

    return particles;
}

Node* create_node(double x, double y, double size, double length) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->center_x = x;
    node->center_y = y;
    node->size = size;
    node->length = length/2;
    node->x_min = x - length;
    node->x_max = x + length;
    node->y_min = y - length;
    node->y_max = y + length;
    node->mass = 0;
    node->external = NULL;
    node->nw = NULL;
    node->ne = NULL;
    node->sw = NULL;
    node->se = NULL;
    node->div = false;
    return node;
}

void insert(Node* node, Particle* particle) {
if (node->external == NULL && node->nw == NULL) {
    node->external = particle;
    node->mass = particle->mass;
    } else {
        if (node->div == false) {
            subdivide(node);
        }
        if (node->external != NULL) {
            if (contains(node->nw, node->external)) {
                insert(node->nw, node->external);
                // node->external = NULL;
            }
            else if (contains(node->ne, node->external)) {
                insert(node->ne, node->external);
                // node->external = NULL;
            }
            else if (contains(node->sw, node->external)) {
                insert(node->sw, node->external);
                // node->external = NULL;
            }
            else if (contains(node->se, node->external)) {
                insert(node->se, node->external);
                // node->external = NULL;
            }
        }

        if (contains(node->nw, particle)) {
            insert(node->nw, particle);
        }
        else if (contains(node->ne, particle)) {
            insert(node->ne, particle);
        }
        else if (contains(node->sw, particle)) {
            insert(node->sw, particle);
        }
        else if (contains(node->se, particle)) {
            insert(node->se, particle);
        }
        node->mass += particle->mass;
        node->external = NULL;
    }
}

void subdivide(Node* node) {
    double x = node->center_x;
    double y = node->center_y;
    node->div = true;
    node->sw = create_node(x - node->length, y - node->length, (node->size)/4, node->length);
    node->se = create_node(x + node->length, y - node->length, (node->size)/4, node->length);
    node->nw = create_node(x - node->length, y + node->length, (node->size)/4, node->length);
    node->ne = create_node(x + node->length, y + node->length, (node->size)/4, node->length);

}

bool contains(Node* node, Particle* particle) {
    if (particle == NULL) {
        return false;
    }
    double x = particle->position_x;
    double y = particle->position_y;
    return (x >= node->x_min && x <= node->x_max && y >= node->y_min && y <= node->y_max);
}

void free_tree(Node* node) {
    if (node == NULL) {
        return;
    } else {
        free_tree(node->nw);
        free_tree(node->ne);
        free_tree(node->sw);
        free_tree(node->se);
    }

    free(node);
    node = NULL;
}

void print_tree(Node* node, int depth) {
    if (node->external != NULL) {
        printf("%*s%s%0.1f @ %0.1f %0.1f\n", 10 * depth, "", "Leaf ", node->mass, node->external->position_x, node->external->position_y);
    } else {
        printf("%*s%s%0.1f @ %0.1f %0.1f\n", 10 * depth, "", "Node ", node->mass, node->center_x, node->center_y);
        if (node->nw != NULL) {
            print_tree(node->nw, depth + 1);
            print_tree(node->ne, depth + 1);
            print_tree(node->sw, depth + 1);
            print_tree(node->se, depth + 1);
        }
    }
}

//////////////////////////////////////////////////////////////
//
//         BARNES    HUT    ALGORITHM                    /////
//
//////////////////////////////////////////////////////////////

void update_forces(Particle* particles, Node* root, int* num_particles, int thread_count);
void calculate_force(Particle* particle, Node* node);
void update_positions(Particle* particles, double time_step, int* num_particles, int thread_count);

void update_forces(Particle* particles, Node* root, int* num_particles, int thread_count) {

    // Parallelise each particle calculation to a thread, sharing load dynamically.
    int i;
#   pragma omp parallel for schedule(guided, 10) num_threads(thread_count)\
        default(none) shared(particles, root, num_particles) private(i)

    for (i = 0; i < *num_particles; i++) {
        particles[i].force_x = 0;
        particles[i].force_y = 0;
        calculate_force(&particles[i], root);
    }
}

void update_positions(Particle* particles, double time_step, int* num_particles, int thread_count) {

    // Parallelise each particle update to a thread, sharing load dynamically.
    int i;
#   pragma omp parallel for schedule(guided, 10) num_threads(thread_count)\
        default(none) shared(time_step, particles, num_particles) private(i)

    for (i = 0; i < *num_particles; i++) {
        double acceleration_x = particles[i].force_x / particles[i].mass;
        double acceleration_y = particles[i].force_y / particles[i].mass;
        particles[i].velocity_x += acceleration_x * time_step;
        particles[i].velocity_y += acceleration_y * time_step;
        particles[i].position_x += particles[i].velocity_x * time_step;
        particles[i].position_y += particles[i].velocity_y * time_step;
    }
}

void calculate_force(Particle* particle, Node* node) {
    if (node->external == NULL) {
        double d = sqrt(pow(node->center_x - particle->position_x, 2) + pow(node->center_y - particle->position_y, 2));
        if (node->size / d < THRESHOLD) {
            double f = G * node->mass * particle->mass / pow(d, 2);
            particle->force_x = f * (node->center_x - particle->position_x) / d * k;
            particle->force_y = f * (node->center_y - particle->position_y) / d * k;
        } else {
            if (node->div == true) {
                calculate_force(particle, node->nw);
                calculate_force(particle, node->ne);
                calculate_force(particle, node->sw);
                calculate_force(particle, node->se);
            }
        }
    } else {
        if (node->external != particle) {
            double d = sqrt(pow(node->external->position_x - particle->position_x, 2) + pow(node->external->position_y - particle->position_y, 2));
            double f = G * node->external->mass * particle->mass / pow(d, 2);            
            particle->force_x = f * (node->external->position_x - particle->position_x) / d * k;
            particle->force_y = f * (node->external->position_y - particle->position_y) / d * k;
        }
    }
}

//////////////////////////////////////////////////////////////
//
//          MISC    FUNCTIONS                             /////
//
//////////////////////////////////////////////////////////////

void Get_args(int argc, char* argv[], int* thread_count_p, int* num_particles_p);
void Usage(char* prog_name);

// referenced from OpenMP resources
/*------------------------------------------------------------------
 * Function:  Get_args
 * Purpose:   Get command line args
 * In args:   argc, argv
 * Out args:  thread_count_p, m_p, n_p
 */
void Get_args(int argc, char* argv[], int* thread_count_p, int* num_particles_p) 
{
   if (argc != 3) Usage(argv[0]);
   *thread_count_p = strtol(argv[1], NULL, 16);
    *num_particles_p = strtol(argv[2], NULL, 16);
   if (*thread_count_p <= 0) Usage(argv[0]);
   if (*num_particles_p <= 0) Usage(argv[0]);

}  /* Get_args */

/*------------------------------------------------------------------
 * Function:  Usage
 * Purpose:   print a message showing what the command line should
 *            be, and terminate
 * In arg :   prog_name
 */
void Usage (char* prog_name) {
   fprintf(stderr, "usage: %s <thread_count>\n", prog_name);
   exit(0);
}  /* Usage */

