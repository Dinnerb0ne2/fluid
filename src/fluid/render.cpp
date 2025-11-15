#include <render.hpp>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cmath>
#include <cstring>

// Constructor: Initialize members in SAME ORDER as declared in render.hpp (fixes reorder warning)
Render::Render(BLWFluid& fluid, int win_width, int win_height, const char* title)
    : window(nullptr),          // 1. GLFWwindow* window
      window_width(win_width),  // 2. int window_width
      window_height(win_height),// 3. int window_height
      fluid(fluid),             // 4. BLWFluid& fluid
      last_time(0.0),
      frame_count(0),
      fps(0.0) {
    
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }

    window = glfwCreateWindow(win_width, win_height, title, NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    setup_opengl();
}

Render::~Render() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Render::setup_opengl() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, window_width, window_height, 0.0f, -1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
}

void Render::update_fps() {
    double current_time = glfwGetTime();
    frame_count++;
    if (current_time - last_time >= 1.0) {
        fps = frame_count / (current_time - last_time);
        frame_count = 0;
        last_time = current_time;
    }
}

// Custom text rendering: Draw single ASCII character (5x7 font, no external dependencies)
void Render::draw_char(float x, float y, char c, float scale) {
    // 5x7 pixel font for ASCII 32 (space) to 126 (~)
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
        // ASCII 65 (A)
        {{0,1,1,1,0}, {0,1,0,1,0}, {0,1,1,1,0}, {0,1,0,1,0}, {0,1,0,1,0}, {0,0,0,0,0}, {0,0,0,0,0}},
        // ASCII 66 (B)
        {{0,1,1,1,0}, {0,1,0,1,0}, {0,1,1,1,0}, {0,1,0,1,0}, {0,1,1,1,0}, {0,0,0,0,0}, {0,0,0,0,0}},
        // ASCII 67 (C)
        {{0,1,1,1,0}, {0,1,0,0,0}, {0,1,0,0,0}, {0,1,0,0,0}, {0,1,1,1,0}, {0,0,0,0,0}, {0,0,0,0,0}},
        // ASCII 68 (D)
        {{0,1,1,0,0}, {0,1,0,1,0}, {0,1,0,1,0}, {0,1,0,1,0}, {0,1,1,0,0}, {0,0,0,0,0}, {0,0,0,0,0}},
        // ASCII 69 (E)
        {{0,1,1,1,0}, {0,1,0,0,0}, {0,1,1,1,0}, {0,1,0,0,0}, {0,1,1,1,0}, {0,0,0,0,0}, {0,0,0,0,0}},
        // ASCII 70 (F)
        {{0,1,1,1,0}, {0,1,0,0,0}, {0,1,1,1,0}, {0,1,0,0,0}, {0,1,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}},
        // ASCII 71 (G)
        {{0,1,1,1,0}, {0,1,0,0,0}, {0,1,0,1,0}, {0,1,0,1,0}, {0,1,1,1,0}, {0,0,0,0,0}, {0,0,0,0,0}},
        // ASCII 72 (H)
        {{0,1,0,1,0}, {0,1,0,1,0}, {0,1,1,1,0}, {0,1,0,1,0}, {0,1,0,1,0}, {0,0,0,0,0}, {0,0,0,0,0}},
        // ASCII 73 (I)
        {{0,1,1,1,0}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0}, {0,1,1,1,0}, {0,0,0,0,0}, {0,0,0,0,0}},
        // ASCII 74 (J)
        {{0,1,1,1,0}, {0,0,0,1,0}, {0,0,0,1,0}, {0,1,0,1,0}, {0,1,1,0,0}, {0,0,0,0,0}, {0,0,0,0,0}},
        // ASCII 75 (K)
        {{0,1,0,1,0}, {0,1,1,0,0}, {0,1,0,0,0}, {0,1,1,0,0}, {0,1,0,1,0}, {0,0,0,0,0}, {0,0,0,0,0}},
        // ASCII 76 (L)
        {{0,1,0,0,0}, {0,1,0,0,0}, {0,1,0,0,0}, {0,1,0,0,0}, {0,1,1,1,0}, {0,0,0,0,0}, {0,0,0,0,0}},
        // ASCII 77 (M)
        {{0,1,0,1,0}, {0,1,1,1,1}, {0,1,0,1,0}, {0,1,0,1,0}, {0,1,0,1,0}, {0,0,0,0,0}, {0,0,0,0,0}},
        // ASCII 78 (N)
        {{0,1,0,1,0}, {0,1,1,1,0}, {0,1,0,1,0}, {0,1,0,1,0}, {0,1,0,1,0}, {0,0,0,0,0}, {0,0,0,0,0}},
        // ASCII 79 (O)
        {{0,1,1,1,0}, {0,1,0,1,0}, {0,1,0,1,0}, {0,1,0,1,0}, {0,1,1,1,0}, {0,0,0,0,0}, {0,0,0,0,0}},
        // ASCII 80 (P)
        {{0,1,1,1,0}, {0,1,0,1,0}, {0,1,1,1,0}, {0,1,0,0,0}, {0,1,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}},
        // ASCII 81 (Q)
        {{0,1,1,1,0}, {0,1,0,1,0}, {0,1,0,1,0}, {0,0,1,1,0}, {0,1,1,0,1}, {0,0,0,0,0}, {0,0,0,0,0}},
        // ASCII 82 (R)
        {{0,1,1,1,0}, {0,1,0,1,0}, {0,1,1,1,0}, {0,1,1,0,0}, {0,1,0,1,0}, {0,0,0,0,0}, {0,0,0,0,0}},
        // ASCII 83 (S)
        {{0,1,1,1,0}, {0,1,0,0,0}, {0,1,1,1,0}, {0,0,0,1,0}, {0,1,1,1,0}, {0,0,0,0,0}, {0,0,0,0,0}},
        // ASCII 84 (T)
        {{0,1,1,1,0}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,0,0,0}, {0,0,0,0,0}},
        // ASCII 85 (U)
        {{0,1,0,1,0}, {0,1,0,1,0}, {0,1,0,1,0}, {0,1,0,1,0}, {0,1,1,1,0}, {0,0,0,0,0}, {0,0,0,0,0}},
        // ASCII 86 (V)
        {{0,1,0,1,0}, {0,1,0,1,0}, {0,1,0,1,0}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,0,0,0}, {0,0,0,0,0}},
        // ASCII 87 (W)
        {{0,1,0,1,0}, {0,1,0,1,0}, {0,1,0,1,0}, {0,1,1,1,0}, {0,1,0,1,0}, {0,0,0,0,0}, {0,0,0,0,0}},
        // ASCII 88 (X)
        {{0,1,0,1,0}, {0,1,0,1,0}, {0,0,1,0,0}, {0,1,0,1,0}, {0,1,0,1,0}, {0,0,0,0,0}, {0,0,0,0,0}},
        // ASCII 89 (Y)
        {{0,1,0,1,0}, {0,1,0,1,0}, {0,1,1,1,0}, {0,0,1,0,0}, {0,0,1,0,0}, {0,0,0,0,0}, {0,0,0,0,0}},
        // ASCII 90 (Z)
        {{0,1,1,1,0}, {0,0,0,1,0}, {0,0,1,0,0}, {0,1,0,0,0}, {0,1,1,1,0}, {0,0,0,0,0}, {0,0,0,0,0}},
        // ASCII 97 (a) to 126 (~) - Add if needed, but basic UI only uses numbers/symbols
        {{0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}}
    };

    // Only render printable ASCII characters (32-126)
    if (c < 32 || c > 126) c = 32;

    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, 1.0f);

    // Draw each pixel of the character as a small quad
    glColor3f(1.0f, 1.0f, 1.0f); // White text
    for (int row = 0; row < 7; ++row) {
        for (int col = 0; col < 5; ++col) {
            if (font[(unsigned char)c][row][col]) {
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

void Render::render_fluid() {
    int grid_w = fluid.get_grid_width();
    int grid_h = fluid.get_grid_height();
    float cell_size = fluid.get_cell_size();
    const auto& cells = fluid.get_cells();

    for (int y = 0; y < grid_h; ++y) {
        for (int x = 0; x < grid_w; ++x) {
            int idx = y * grid_w + x;
            const auto& cell = cells[idx];

            if (cell.is_obstacle) continue;

            // Color based on density (blue → green → red gradient)
            float density_norm = std::clamp(cell.density, 0.8f, 1.2f);
            float r = std::min(1.0f, (density_norm - 0.8f) * 5.0f);
            float b = std::min(1.0f, (1.2f - density_norm) * 5.0f);
            float g = 0.2f + (0.6f * (1.0f - fabs(density_norm - 1.0f) * 5.0f));

            glColor3f(r, g, b);
            glBegin(GL_QUADS);
            glVertex2f(x * cell_size, y * cell_size);
            glVertex2f((x+1) * cell_size, y * cell_size);
            glVertex2f((x+1) * cell_size, (y+1) * cell_size);
            glVertex2f(x * cell_size, (y+1) * cell_size);
            glEnd();
        }
    }
}

void Render::render_obstacles() {
    // Render obstacles as black polygons
    glColor3f(0.0f, 0.0f, 0.0f);
    const auto& cells = fluid.get_cells();
    float cell_size = fluid.get_cell_size();

    for (const auto& cell : cells) {
        if (cell.is_obstacle) {
            glBegin(GL_QUADS);
            glVertex2f(cell.pos.x, cell.pos.y);
            glVertex2f(cell.pos.x + cell_size, cell.pos.y);
            glVertex2f(cell.pos.x + cell_size, cell.pos.y + cell_size);
            glVertex2f(cell.pos.x, cell.pos.y + cell_size);
            glEnd();
        }
    }
}

void Render::render_ui() {
    update_fps();

    // Render FPS and parameters (fix: %u for size_t obstacle count)
    char text[256];
    snprintf(text, sizeof(text), "FPS: %.1f | Grid: %dx%d | Viscosity: %.4f | Obstacles: %u",
             fps,
             fluid.get_grid_width(),
             fluid.get_grid_height(),
             fluid.get_viscosity(),
             static_cast<unsigned int>(fluid.get_obstacle_count())); // Cast to match %u

    // Draw text at bottom-left (y = 20 to avoid clipping)
    float x = 10.0f;
    float y = 20.0f;
    float scale = 1.5f;

    for (size_t i = 0; i < strlen(text); ++i) {
        draw_char(x, y, text[i], scale);
        x += 12.0f * scale; // Advance x position for next character
    }
}

void Render::process_input() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void Render::render() {
    glClear(GL_COLOR_BUFFER_BIT);
    render_obstacles();
    render_fluid();
    render_ui();
}