# Project Details and Usage

## Obstacle Configuration
Obstacles are polygon shapes defined by vertices. You can add obstacles in two ways:

### 1. File Import
Create text files in the `obstacles/` directory (e.g., `obstacle1.txt`) with one vertex per line (x/y coordinates, space-separated):
```txt
50.0 50.0    # Vertex 1
200.0 50.0   # Vertex 2
200.0 200.0  # Vertex 3
50.0 200.0   # Vertex 4 (closes the square)
```
- Vertices must be ordered clockwise or counter-clockwise.
- Minimum 3 vertices required for a valid polygon.

### 2. Manual Vertex Input
In `src/main.cpp`, uncomment and modify the custom polygon example:
```cpp
std::vector<Vec2> triangle_vertices = {
    Vec2(300.0f, 150.0f),
    Vec2(350.0f, 250.0f),
    Vec2(250.0f, 250.0f)
};
fluid.add_obstacle_from_vertices(triangle_vertices);
```

## Build & Run
### Windows (Dev-C++/MinGW)
1. **Build the Project**:
   - Double-click `build.cmd` or run it via the command prompt:
     ```bash
     build.cmd
     ```
   - The compiled executable (`blw_fluid.exe`) will be created in the `bin/` directory.

2. **Run the Simulation**:
   - Double-click `run.cmd` or run it via the command prompt:
     ```bash
     run.cmd
     ```

### Troubleshooting Build Errors
- Ensure `MINGW_PATH` in `build.cmd` matches your Dev-C++ MinGW installation path (e.g., `C:\Program Files (x86)\Dev-Cpp\MinGW32\bin` for 32-bit).
- Verify GLFW libraries in `lib/` match your compiler architecture (32/64-bit).
- Check that all source files are present in the `src/` directory.

## Simulation Parameters
Modify these parameters in `src/main.cpp` to adjust the simulation:
| Parameter          | Description                                  | Default Value |
|---------------------|----------------------------------------------|---------------|
| `GRID_WIDTH`        | Number of fluid cells along the x-axis        | 128           |
| `GRID_HEIGHT`       | Number of fluid cells along the y-axis        | 128           |
| `CELL_SIZE`         | Size of each fluid cell (pixels)              | 4.0f          |
| `VISCOSITY`         | Fluid viscosity (lower = more turbulent)      | 0.01f         |
| `GRAVITY`           | Vertical gravity force (negative = downward)  | -0.001f       |

### N-ary Tree Configuration
To change the n-ary tree order (n≥4), modify the template parameter in `include/fluid.h`:
```cpp
// Change from 4 to 5, 6, etc. (n≥4)
NaryTree<4> spatial_tree;
```
Remember to add an explicit instantiation in `src/fluid/nary_tree.cpp` for the new order:
```cpp
template class NaryTree<5>;
template class NaryTreeNode<5>;
```

## Usage
- **Start Simulation**: Run `run.cmd` after building.
- **Exit Simulation**: Press the `ESC` key.
- **Visualization**:
  - Fluid density is color-coded: blue (low density) → green (medium) → red (high density).
  - Obstacles are rendered as black polygons.
  - UI at the bottom displays: FPS, grid size, viscosity, and obstacle count.