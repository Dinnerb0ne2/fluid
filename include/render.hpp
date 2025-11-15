#ifndef RENDER_HPP
#define RENDER_HPP

#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <cmath>
#include <cstring>
#include <algorithm>
#include <fluid.hpp>

// Render class for OpenGL-based visualization
class Render {
private:
    GLFWwindow* window;           // GLFW window handle
    int window_width;             // Window width in pixels
    int window_height;            // Window height in pixels
    BLWFluid& fluid;              // Reference to fluid simulation
    double last_time;             // Time of last FPS update
    int frame_count;              // Frame counter for FPS calculation
    float fps;                    // Current FPS value
    
    // Initialize OpenGL settings
    void setup_opengl() {
        // Set orthographic projection (2D rendering)
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0f, window_width, window_height, 0.0f, -1.0f, 1.0f);
        glMatrixMode(GL_MODELVIEW);
        
        // Enable blending for transparency
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        // Enable anti-aliasing
        glEnable(GL_POINT_SMOOTH);
        glEnable(GL_LINE_SMOOTH);
        
        // Set clear color (black)
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    }
    
    // Update FPS counter
    void update_fps() {
        double current_time = glfwGetTime();
        frame_count++;
        
        // Update FPS every 1 second
        if (current_time - last_time >= 1.0) {
            fps = static_cast<float>(frame_count) / static_cast<float>(current_time - last_time);
            frame_count = 0;
            last_time = current_time;
        }
    }
    
    // Draw a single ASCII character using OpenGL quads (no external dependencies)
    void draw_char(float x, float y, char c, float scale = 1.0f) {
        // 5x7 pixel font matrix (ASCII 32-126)
        const bool font[128][7][5] = {
            // ASCII 32 (space)
            {{0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}},
            // ASCII 48 (0)
            {{0,1,1,1,0}, {0,1,0,1,0}, {0,1,0,1,0}, {0,1,0,1,0}, {0,1,1,1,0}, {0,0,0,0,0}, {0,0,0,0,0}},
            // ASCII 49 (1)
            {{0,0,1,0,0}, {0,1,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0}, {0,1,1,1,0}, {0,0,0,0,0}, {0,0,0,0,0}},
            // ASCII 50 (2)
            {{0,1,1,1,0}, {0,0,0,1,0}, {0,1,1,1,0}, {0,1,0,0,0}, {0,1,1,1,0}, {0,0,0,0,0}, {0,0,0,0,0}},
            // ASCII 51 (3)
            {{0,1,1,1,0}, {0,0,0,1,0}, {0,1,1,1,0}, {0,0,0,1,0}, {0,1,1,1,0}, {0,0,0,0,0}, {0,0,0,0,0}},
            // ASCII 52 (4)
            {{0,1,0,1,0}, {0,1,0,1,0}, {0,1,1,1,0}, {0,0,0,1,0}, {0,0,0,1,0}, {0,0,0,0,0}, {0,0,0,0,0}},
            // ASCII 53 (5)
            {{0,1,1,1,0}, {0,1,0,0,0}, {0,1,1,1,0}, {0,0,0,1,0}, {0,1,1,1,0}, {0,0,0,0,0}, {0,0,0,0,0}},
            // ASCII 54 (6)
            {{0,1,1,1,0}, {0,1,0,0,0}, {0,1,1,1,0}, {0,1,0,1,0}, {0,1,1,1,0}, {0,0,0,0,0}, {0,0,0,0,0}},
            // ASCII 55 (7)
            {{0,1,1,1,0}, {0,0,0,1,0}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,0,0,0}, {0,0,0,0,0}},
            // ASCII 56 (8)
            {{0,1,1,1,0}, {0,1,0,1,0}, {0,1,1,1,0}, {0,1,0,1,0}, {0,1,1,1,0}, {0,0,0,0,0}, {0,0,0,0,0}},
            // ASCII 57 (9)
            {{0,1,1,1,0}, {0,1,0,1,0}, {0,1,1,1,0}, {0,0,0,1,0}, {0,1,1,1,0}, {0,0,0,0,0}, {0,0,0,0,0}},
            // ASCII 65 (A) to 126 (~) - Basic set for UI
            {{0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}}
        };
        
        // Clamp to printable ASCII characters
        if (c < 32 || c > 126) c = 32;
        
        glPushMatrix();
        glTranslatef(x, y, 0.0f);
        glScalef(scale, scale, 1.0f);
        
        // Draw each pixel of the character as a small quad
        glColor3f(1.0f, 1.0f, 1.0f); // White text
        for (int row = 0; row < 7; ++row) {
            for (int col = 0; col < 5; ++col) {
                if (font[static_cast<unsigned char>(c)][row][col]) {
                    float px = col * 2.0f;
                    float py = row * 2.0f;
                    glBegin(GL_QUADS);
                        glVertex2f(px, py);
                        glVertex2f(px + 2.0f, py);
                        glVertex2f(px + 2.0f, py + 2.0f);
                        glVertex2f(px, py + 2.0f);
                    glEnd();
                }
            }
        }
        
        glPopMatrix();
    }
    
    // Render fluid cells with density-based coloring
    void render_fluid() {
        const auto& cells = fluid.get_cells();
        int grid_w = fluid.get_grid_width();
        int grid_h = fluid.get_grid_height();
        float cell_size = fluid.get_cell_size();
        
        // Validate cell grid size (prevent out-of-bounds access)
        if (cells.size() != static_cast<size_t>(grid_w * grid_h)) {
            std::cerr << "[ERROR] Render: Cell grid size mismatch - Expected " << grid_w * grid_h << ", Got " << cells.size() << std::endl;
            return;
        }
        
        for (int y = 0; y < grid_h; ++y) {
            for (int x = 0; x < grid_w; ++x) {
                int idx = y * grid_w + x;
                // Skip invalid indices (safety check)
                if (idx < 0 || idx >= static_cast<int>(cells.size())) {
                    std::cerr << "[WARNING] Render: Invalid cell index " << idx << std::endl;
                    continue;
                }
                
                const auto& cell = cells[idx];
                if (cell.is_obstacle) continue;
                
                // Clamp density to valid range (0.8-1.2) for stable coloring
                float density = std::clamp(cell.density, 0.8f, 1.2f);
                // Calculate color gradient: Blue (low density) → Green → Red (high density)
                float r = std::min(1.0f, (density - 0.8f) * 5.0f);
                float b = std::min(1.0f, (1.2f - density) * 5.0f);
                float g = 0.2f + (0.6f * (1.0f - std::fabs(density - 1.0f) * 5.0f));
                
                // Set color and draw cell as quad
                glColor3f(r, g, b);
                glBegin(GL_QUADS);
                    float x_pos = x * cell_size;
                    float y_pos = y * cell_size;
                    glVertex2f(x_pos, y_pos);
                    glVertex2f(x_pos + cell_size, y_pos);
                    glVertex2f(x_pos + cell_size, y_pos + cell_size);
                    glVertex2f(x_pos, y_pos + cell_size);
                glEnd();
            }
        }
    }
    
    // Render obstacles as black quads
    void render_obstacles() {
        const auto& cells = fluid.get_cells();
        int grid_w = fluid.get_grid_width();
        int grid_h = fluid.get_grid_height();
        float cell_size = fluid.get_cell_size();
        
        if (cells.size() != static_cast<size_t>(grid_w * grid_h)) {
            std::cerr << "[ERROR] Render: Obstacle grid size mismatch" << std::endl;
            return;
        }
        
        glColor3f(0.0f, 0.0f, 0.0f); // Black color for obstacles
        for (size_t idx = 0; idx < cells.size(); ++idx) {
            const auto& cell = cells[idx];
            if (cell.is_obstacle) {
                // Calculate cell position from index
                int x = static_cast<int>(idx % grid_w);
                int y = static_cast<int>(idx / grid_w);
                float x_pos = x * cell_size;
                float y_pos = y * cell_size;
                
                glBegin(GL_QUADS);
                    glVertex2f(x_pos, y_pos);
                    glVertex2f(x_pos + cell_size, y_pos);
                    glVertex2f(x_pos + cell_size, y_pos + cell_size);
                    glVertex2f(x_pos, y_pos + cell_size);
                glEnd();
            }
        }
    }
    
    // Render UI (FPS, grid size, viscosity, obstacle count)
    void render_ui() {
        update_fps();
        
        // Format UI text
        char text[256];
        snprintf(text, sizeof(text), "FPS: %.1f | Grid: %dx%d | Viscosity: %.4f | Obstacles: %u",
                 fps,
                 fluid.get_grid_width(),
                 fluid.get_grid_height(),
                 fluid.get_viscosity(),
                 static_cast<unsigned int>(fluid.get_obstacle_count()));
        
        // Draw text at bottom-left corner (10px padding)
        float x = 10.0f;
        float y = 20.0f;
        float scale = 1.5f;
        
        for (size_t i = 0; i < std::strlen(text); ++i) {
            draw_char(x, y, text[i], scale);
            x += 12.0f * scale; // Advance X position for next character
        }
    }

public:
    // Constructor: Initialize GLFW window and rendering context
    Render(BLWFluid& fluid_ref, int win_width, int win_height, const char* title)
        : window(nullptr),
          window_width(win_width),
          window_height(win_height),
          fluid(fluid_ref),
          last_time(0.0),
          frame_count(0),
          fps(0.0) {
        
        // Initialize GLFW
        if (!glfwInit()) {
            std::cerr << "[ERROR] Render: Failed to initialize GLFW" << std::endl;
            exit(EXIT_FAILURE);
        }
        
        // Configure GLFW window (compatible with legacy OpenGL functions)
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
        
        // Create GLFW window
        window = glfwCreateWindow(win_width, win_height, title, nullptr, nullptr);
        if (!window) {
            std::cerr << "[ERROR] Render: Failed to create GLFW window" << std::endl;
            glfwTerminate();
            exit(EXIT_FAILURE);
        }
        
        // Set OpenGL context as current
        glfwMakeContextCurrent(window);
        if (!glfwGetCurrentContext()) {
            std::cerr << "[ERROR] Render: Failed to set OpenGL context" << std::endl;
            glfwDestroyWindow(window);
            glfwTerminate();
            exit(EXIT_FAILURE);
        }
        
        // Initialize OpenGL settings
        setup_opengl();
        std::cout << "[DEBUG] Render: Initialized window " << win_width << "x" << win_height << std::endl;
    }
    
    // Destructor: Clean up GLFW resources
    ~Render() {
        glfwDestroyWindow(window);
        glfwTerminate();
        std::cout << "[DEBUG] Render: Cleaned up GLFW resources" << std::endl;
    }
    
    // Check if window is still open
    bool is_running() const {
        return !glfwWindowShouldClose(window);
    }
    
    // Handle user input (ESC to quit)
    void process_input() {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    }
    
    // Main render loop
    void render() {
        // Clear color buffer
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Render in correct order: Obstacles → Fluid → UI
        render_obstacles();
        render_fluid();
        render_ui();
    }
    
    // Swap front/back buffers (double buffering)
    void swap_buffers() {
        glfwSwapBuffers(window);
    }
    
    // Poll for events (keyboard, mouse, etc.)
    void poll_events() {
        glfwPollEvents();
    }
    
    // Getter for the current FPS value
    float get_current_fps() const {
        return fps;
    }
};

#endif // RENDER_HPP