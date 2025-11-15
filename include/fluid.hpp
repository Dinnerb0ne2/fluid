#ifndef FLUID_HPP
#define FLUID_HPP

#include <vector>
#include <cmath>
#include <stdexcept>
#include <algorithm>
#include <string>  
#include <nary_tree.hpp>
#include <obstacle.hpp>

// Lattice Boltzmann D2Q9 model parameters (2D, 9 velocity directions)
const int NUM_VELOCITIES = 9;
const float W[NUM_VELOCITIES] = {4.0f/9.0f, 1.0f/9.0f, 1.0f/9.0f, 1.0f/9.0f, 1.0f/9.0f, 
                                 1.0f/36.0f, 1.0f/36.0f, 1.0f/36.0f, 1.0f/36.0f}; // Weights
const int CX[NUM_VELOCITIES] = {0, 1, 0, -1, 0, 1, -1, -1, 1}; // X velocity components
const int CY[NUM_VELOCITIES] = {0, 0, 1, 0, -1, 1, 1, -1, -1}; // Y velocity components

// Structure representing a single fluid cell
struct FluidCell {
    Vec2 pos;                     // Cell position in world space
    float density;                // Macroscopic density
    float velocity[NUM_VELOCITIES]; // Distribution function for each velocity direction
    bool is_obstacle;             // Whether the cell is an obstacle
    
    // Constructor: Initialize default values
    FluidCell() : density(1.0f), is_obstacle(false) {
        // Initialize all velocities to 0.0f
        for (int i = 0; i < NUM_VELOCITIES; ++i) {
            velocity[i] = 0.0f;
        }
    }
};

// BLW (Boltzmann Lattice Weighted) fluid simulation class
class BLWFluid {
private:
    int grid_width;               // Number of cells in X direction
    int grid_height;              // Number of cells in Y direction
    float cell_size;              // Size of each cell in world units
    std::vector<FluidCell> cells; // Grid of fluid cells
    ObstacleManager obstacle_manager; // Obstacle manager
    NaryTree<4> spatial_tree;     // 4-ary tree for neighbor queries
    
    // Fluid physical parameters
    float kinematic_viscosity;    // Viscosity of the fluid
    float dt;                     // Time step (calculated from cell size)
    float gravity;                // Gravitational acceleration (Y direction)
    
    // Calculate equilibrium distribution function
    void compute_equilibrium(FluidCell& cell, float ux, float uy) {
        if (cell.is_obstacle) return;
        
        float u_sq = ux * ux + uy * uy;
        for (int i = 0; i < NUM_VELOCITIES; ++i) {
            float cu = CX[i] * ux + CY[i] * uy; // Dot product of velocity and direction
            cell.velocity[i] = W[i] * cell.density * (1.0f + 3.0f * cu + 4.5f * cu * cu - 1.5f * u_sq);
        }
    }
    
    // Perform collision step (BGK model)
    void collision(FluidCell& cell) {
        if (cell.is_obstacle) return;
        
        // Calculate macroscopic velocity (ux, uy)
        float ux = 0.0f, uy = 0.0f;
        for (int i = 0; i < NUM_VELOCITIES; ++i) {
            ux += CX[i] * cell.velocity[i];
            uy += CY[i] * cell.velocity[i];
        }
        
        // Avoid division by zero (should not happen with valid density)
        if (std::fabs(cell.density) < 1e-6) {
            cell.density = 1.0f;
        }
        ux /= cell.density;
        uy /= cell.density;
        
        // Apply gravitational acceleration
        uy += gravity * dt;
        
        // Calculate equilibrium distribution
        float feq[NUM_VELOCITIES];
        float u_sq = ux * ux + uy * uy;
        for (int i = 0; i < NUM_VELOCITIES; ++i) {
            float cu = CX[i] * ux + CY[i] * uy;
            feq[i] = W[i] * cell.density * (1.0f + 3.0f * cu + 4.5f * cu * cu - 1.5f * u_sq);
        }
        
        // BGK collision: relax towards equilibrium
        float tau = 0.5f + (kinematic_viscosity * dt) / (cell_size * cell_size);
        float omega = 1.0f / tau; // Relaxation parameter
        for (int i = 0; i < NUM_VELOCITIES; ++i) {
            cell.velocity[i] = (1.0f - omega) * cell.velocity[i] + omega * feq[i];
        }
    }
    
    // Perform streaming step (move distribution functions between cells)
    void streaming() {
        std::vector<FluidCell> new_cells = cells; // Copy current state
        
        for (int y = 0; y < grid_height; ++y) {
            for (int x = 0; x < grid_width; ++x) {
                int idx = y * grid_width + x;
                if (cells[idx].is_obstacle) continue;
                
                // Stream to neighboring cells for each velocity direction
                for (int i = 0; i < NUM_VELOCITIES; ++i) {
                    int nx = x + CX[i];
                    int ny = y + CY[i];
                    
                    // Boundary check: apply bounce-back if out of bounds
                    if (nx < 0 || nx >= grid_width || ny < 0 || ny >= grid_height) {
                        int bounce_i = (i % 4 == 0) ? i : (i + 2) % 4; // Reverse direction
                        new_cells[idx].velocity[bounce_i] += cells[idx].velocity[i];
                    } else {
                        int nidx = ny * grid_width + nx;
                        // Bounce-back if neighboring cell is an obstacle
                        if (new_cells[nidx].is_obstacle) {
                            int bounce_i = (i % 4 == 0) ? i : (i + 2) % 4;
                            new_cells[idx].velocity[bounce_i] += cells[idx].velocity[i];
                        } else {
                            new_cells[nidx].velocity[i] += cells[idx].velocity[i];
                        }
                    }
                }
            }
        }
        
        cells = new_cells; // Update to new state
    }
    
    // Update obstacle status for all cells (call after adding obstacles)
    void update_obstacle_cells() {
        for (int y = 0; y < grid_height; ++y) {
            for (int x = 0; x < grid_width; ++x) {
                int idx = y * grid_width + x;
                cells[idx].is_obstacle = obstacle_manager.is_point_obstructed(cells[idx].pos);
            }
        }
    }
    
    // Update macroscopic density from distribution functions
    void update_density() {
        for (auto& cell : cells) {
            if (cell.is_obstacle) continue;
            
            cell.density = 0.0f;
            for (int i = 0; i < NUM_VELOCITIES; ++i) {
                cell.density += cell.velocity[i];
            }
            cell.density = std::clamp(cell.density, 0.5f, 1.5f);
        }
    }

public:
    // Ensure initialization order matches declaration order (obstacle_manager before spatial_tree)
    // Constructor is declared here; implementation is in src/fluid/fluid.cpp to avoid duplicate definition
    BLWFluid(int width, int height, float cell_size, float viscosity, float gravity);
    
    // Destructor: Default (vector and tree handle memory automatically)
    ~BLWFluid() = default;

    bool add_obstacle_from_file(const std::string& filename) {
        // Ensure obstacle_manager is correctly declared
        bool success = obstacle_manager.add_obstacle_from_file(filename);
        if (success) {
            update_obstacle_cells(); // Update cell obstacle status
        }
        return success;
    }
    
    // Add obstacle from vertex list
    bool add_obstacle_from_vertices(const std::vector<Vec2>& vertices) {
        // Ensure obstacle_manager is correctly declared
        bool success = obstacle_manager.add_obstacle_from_vertices(vertices);
        if (success) {
            update_obstacle_cells();
        }
        return success;
    }
    
    // Update fluid simulation by one time step
    void update() {
        // 1. Collision step
        for (auto& cell : cells) {
            collision(cell);
        }
        
        // 2. Streaming step
        streaming();
        
        // 3. Update macroscopic properties
        update_density();
    }
    
    // Getters for rendering
    const std::vector<FluidCell>& get_cells() const { return cells; }
    int get_grid_width() const { return grid_width; }
    int get_grid_height() const { return grid_height; }
    float get_cell_size() const { return cell_size; }
    float get_viscosity() const { return kinematic_viscosity; }
    size_t get_obstacle_count() const { 
        // 修复：obstacle_manager 已正确声明
        return obstacle_manager.get_obstacle_count(); 
    }
};

#endif // FLUID_HPP