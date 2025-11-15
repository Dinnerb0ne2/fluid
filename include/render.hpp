#ifndef RENDER_HPP
#define RENDER_HPP

#include <GLFW/glfw3.h>
#include <string>

// Forward declaration to avoid including heavy headers in this public header
class BLWFluid;

// Render class for OpenGL-based visualization (declarations only)
class Render {
private:
    GLFWwindow* window;           // GLFW window handle
    int window_width;             // Window width in pixels
    int window_height;            // Window height in pixels
    BLWFluid& fluid;              // Reference to fluid simulation
    double last_time;             // Time of last FPS update
    int frame_count;              // Frame counter for FPS calculation
    float fps;                    // Current FPS value

    // Internal helpers (definitions in src/fluid/render.cpp)
    void setup_opengl();
    void update_fps();
    void draw_char(float x, float y, char c, float scale = 1.0f);
    void render_fluid();
    void render_obstacles();
    void render_ui();

public:
    // Constructor: Initialize GLFW window and rendering context
    Render(BLWFluid& fluid_ref, int win_width, int win_height, const char* title);

    // Destructor: Clean up GLFW resources
    ~Render();

    // Check if window is still open
    bool is_running() const;

    // Handle user input (ESC to quit)
    void process_input();

    // Main render routine (render obstacles, fluid, UI)
    void render();

    // Swap front/back buffers (double buffering)
    void swap_buffers();

    // Poll for events (keyboard, mouse, etc.)
    void poll_events();

    // Getter for the current FPS value
    float get_current_fps() const;
};

#endif // RENDER_HPP