#include <fluid.hpp>
#include <iostream>
#include <cmath>
#include <vector>

BLWFluid::BLWFluid(int width, int height, float cell_size, float viscosity, float gravity)
    : grid_width(width), grid_height(height), cell_size(cell_size),
      kinematic_viscosity(viscosity), gravity(gravity),
      spatial_tree(Vec2(0.0f, 0.0f), Vec2(width * cell_size, height * cell_size), cell_size * 2.0f) {
    
    dt = cell_size / sqrt(2.0f); // Stable time step
    cells.resize(width * height);
    std::cout << "[DEBUG] BLWFluid: Cells resized to " << width * height << " elements" << std::endl;

    // Initialize cell positions and check for obstacles
    std::vector<Vec2> cell_positions;
    cell_positions.reserve(width * height); // Preallocate for safety
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int idx = y * width + x;
            cells[idx].pos = Vec2(x * cell_size, y * cell_size);
            cells[idx].is_obstacle = obstacle_manager.is_point_obstructed(cells[idx].pos);
            cells[idx].density = 1.0f; // Explicit default density (avoids NaNs)
            // Initialize velocities to 0.0f (already done in FluidCell constructor)
            cell_positions.push_back(cells[idx].pos);
        }
    }

    spatial_tree.build(cell_positions);
    std::cout << "[DEBUG] BLWFluid: Spatial tree built with " << cell_positions.size() << " cells" << std::endl;
}