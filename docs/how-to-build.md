# How to Build

## Dependencies
1. **GLFW 3.x**: Window management and OpenGL context creation
   - Download: [GLFW Official Website](https://www.glfw.org/download.html)
2. **GLM (Optional)**: For vector math (included via custom `Vec2` struct if not used)
   - Download: [GLM Official Website](https://glm.g-truc.net/)

## Installation & Setup
1. **Install Dependencies**:
   - Install MinGW
   - Download GLFW and extract the following files:
     - Copy `include/GLFW/` to the project's `include/GLFW/` directory.
     - Copy `lib-mingw-w64/libglfw3.a` and `libglfw3dll.a` (for 64-bit) or `lib-mingw/libglfw3.a` (for 32-bit) to the project's `lib/` directory.

2. **Optional: GLM Setup**:
   - If using GLM, extract the `glm/` directory to the project's `include/` folder and replace the custom `Vec2` struct with `glm::vec2` in source files.