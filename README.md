# 2D N-ary Tree BLW Fluid Simulation with Obstacles (GLFW)

A real-time 2D fluid simulation implementation using the Lattice Boltzmann Method (BLW model) with GLFW for rendering. This project supports custom polygon obstacles (file-imported or manually defined), n-ary tree spatial optimization (n≥4), color-coded density rendering, and real-time performance metrics.

## Language
- README in [Chinese](README_zh-cn.md)

## Table of Contents
- [Features](#features)
- [Project Structure](#project-structure)
- [Technical Details](#technical-details)

## Docs
- see document files for more project details: [Document](docs/)

## Features
- **2D BLW Fluid Simulation**: Implements the D2Q9 Lattice Boltzmann model for stable fluid dynamics
- **N-ary Tree Optimization**: Spatial partitioning (n≥4) to accelerate neighbor queries
- **Custom Obstacles**: Support for multiple polygon obstacles (file import or manual vertex input)
- **Color Rendering**: Density-based gradient (blue → green → red) for visualizing fluid motion
- **Real-Time Metrics**: FPS, grid size, viscosity, and obstacle count displayed in the UI
- **Configurable Boundaries**: Bounce-back boundary conditions for obstacles and window edges
- **Cross-Platform Compatibility**: Dev-C++/MinGW compatible build scripts (easily adaptable to other compilers)

## Project Structure
```
BLW_Fluid_Simulation/
├── lib/                  # GLFW static/import libraries
│   ├── libglfw3.a
│   └── libglfw3dll.a
├── include/              # Header files
│   ├── GLFW/             # GLFW core headers
│   │   ├── glfw3.h
│   │   └── glfw3native.h
|   ├── fluid.hpp
│   ├── obstacle.hpp
│   ├── nary_tree.hpp
│   └── render.chpp
|
├── src/                  # Source code
│   ├── main.cpp          # Entry point & initialization
│   └──fluid
|       ├── fluid.cpp       # BLW fluid core logic
│       ├── obstacle.cpp    # Obstacle management (polygons/files)
│       ├── nary_tree.cpp   # N-ary tree spatial optimization
│       └── render.cpp    # GLFW rendering & UI
├── obstacles/            # Obstacle definition files
│   ├── obstacle1.txt
│   ├── obstacle2.txt
│   └── obstacle3.txt
├── build.cmd             # Compilation script
├── run.cmd               # Execution script
└── README.md             # Project documentation
```

## Technical Details
### Fluid Simulation
- **Model**: D2Q9 Lattice Boltzmann Method (BLW)
- **Collision**: BGK (Bhatnagar-Gross-Krook) collision model
- **Boundary Conditions**: Bounce-back for obstacles and window edges
- **Time Step**: Stable time step calculated as `cell_size / sqrt(2.0f)`

### Spatial Optimization
- **N-ary Tree**: Partitions the simulation space into n×n child nodes to reduce neighbor query complexity from O(n²) to O(logₙn²).
- **Threshold**: Nodes are subdivided until their size is ≤ `cell_size * 2.0f` (configurable in `BLWFluid` constructor).

### Rendering
- **OpenGL**: Uses immediate mode (GL_QUADS) for simple, efficient rendering.
- **Blending**: Enabled to handle overlapping fluid cells and UI elements.
- **Color Gradient**: Density values are clamped to [0.8, 1.2] and mapped to RGB for consistent visualization.

## License
This project is licensed under the Apache License 2.0 - see the LICENSE file for details: [Apache License 2.0](LICENSE)

## Acknowledgments
- GLFW Team for the cross-platform windowing library.
- Lattice Boltzmann Method references and tutorials for fluid dynamics implementation.
- MinGW community for compiler toolchain support.