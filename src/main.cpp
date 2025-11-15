// -*- coding: utf-8 -*-
// Copyright 2025(C) CryptoChat Dinnerb0ne<tomma_2022@outlook.com>
//
//    Copyright 2025 [Dinnberb0ne]
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0 
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
//
// date: 2025-11-15
// version: 0.1
// description: 2D BLW Fluid Simulation (N-ary Tree Spatial Partitioning)
// LICENSE: Apache-2.0

#include <iostream>
#include <fluid.hpp>
#include <render.hpp>

int main() {
    std::cout << "[DEBUG] Main: Starting 2D BLW Fluid Simulation" << std::endl;
    
    // Simulation configuration (adjustable)
    const int GRID_WIDTH = 128;    // Number of cells in X direction
    const int GRID_HEIGHT = 128;   // Number of cells in Y direction
    const float CELL_SIZE = 4.0f;  // Size of each cell (pixels)
    const float VISCOSITY = 0.01f; // Fluid viscosity (lower = more turbulent)
    const float GRAVITY = -0.001f; // Gravitational acceleration (negative = downward)
    const int WINDOW_WIDTH = GRID_WIDTH * CELL_SIZE;  // Window width (pixels)
    const int WINDOW_HEIGHT = GRID_HEIGHT * CELL_SIZE;// Window height (pixels)
    
    try {
        // Initialize fluid simulation
        std::cout << "[DEBUG] Main: Initializing fluid simulation..." << std::endl;
        BLWFluid fluid(GRID_WIDTH, GRID_HEIGHT, CELL_SIZE, VISCOSITY, GRAVITY);
        
        // Load obstacles (place these files in "obstacles/" folder)
        std::cout << "[DEBUG] Main: Loading obstacles..." << std::endl;
        fluid.add_obstacle_from_file("obstacles/obstacle1.txt");
        fluid.add_obstacle_from_file("obstacles/obstacle2.txt");
        fluid.add_obstacle_from_file("obstacles/obstacle3.txt");
        
        // Initialize renderer
        std::cout << "[DEBUG] Main: Initializing renderer..." << std::endl;
        Render render(fluid, WINDOW_WIDTH, WINDOW_HEIGHT, "2D BLW Fluid Simulation (N-ary Tree)");
        
        // Main simulation loop
        std::cout << "[DEBUG] Main: Starting main loop (ESC to quit)" << std::endl;
        int frame = 0;
        while (render.is_running()) {
            render.process_input();  // Handle ESC key
            fluid.update();          // Update fluid simulation
            render.render();         // Render fluid + obstacles + UI
            render.swap_buffers();   // Swap double buffers
            render.poll_events();    // Poll input events
            
            if (frame % 100 == 0) {
                std::cout << "[DEBUG] Main: Frame " << frame << " - Simulation running..." << std::endl;
            }
            frame++;
        }
        
        std::cout << "[DEBUG] Main: Simulation exited successfully" << std::endl;
        return EXIT_SUCCESS;
        
    } catch (const std::exception& e) {
        std::cerr << "[FATAL] Main: Exception - " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}