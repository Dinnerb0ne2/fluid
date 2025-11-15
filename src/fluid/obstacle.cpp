#include <obstacle.hpp>
#include <GLFW/glfw3.h>

#include <fstream>
#include <sstream>
#include <cmath>
#include <iostream>

// PolygonObstacle constructor
PolygonObstacle::PolygonObstacle()
    : is_solid(true) {
}

// Load obstacle vertices from file
bool PolygonObstacle::load_from_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[ERROR] Obstacle: Failed to open file - " << filename << std::endl;
        return false;
    }

    vertices.clear();
    std::string line;
    int line_num = 0;
    while (std::getline(file, line)) {
        line_num++;
        if (line.empty() || line.front() == '#') continue;
        std::istringstream iss(line);
        float x, y;
        if (!(iss >> x >> y)) {
            std::cerr << "[WARNING] Obstacle: Invalid format in " << filename << " (line " << line_num << ")" << std::endl;
            continue;
        }
        vertices.emplace_back(x, y);
    }

    file.close();

    if (vertices.size() < 3) {
        std::cerr << "[ERROR] Obstacle: Insufficient vertices (" << vertices.size() << " < 3) - " << filename << std::endl;
        return false;
    }

    // Close polygon if needed
    if (vertices.front().x != vertices.back().x || vertices.front().y != vertices.back().y) {
        vertices.push_back(vertices.front());
    }

    return true;
}

// Point-in-polygon (ray-casting)
bool PolygonObstacle::point_inside(const Vec2& p) const {
    bool inside = false;
    size_t n = (vertices.size() >= 1) ? vertices.size() - 1 : 0;
    if (n == 0) return false;

    for (size_t i = 0, j = n - 1; i < n; j = i++) {
        if (((vertices[i].y > p.y) != (vertices[j].y > p.y)) &&
            (p.x < (vertices[j].x - vertices[i].x) * (p.y - vertices[i].y) / (vertices[j].y - vertices[i].y) + vertices[i].x)) {
            inside = !inside;
        }
    }
    return inside;
}

// ObstacleManager constructor
ObstacleManager::ObstacleManager() {}

bool ObstacleManager::add_obstacle_from_file(const std::string& filename) {
    PolygonObstacle obs;
    if (obs.load_from_file(filename)) {
        obstacles.push_back(std::move(obs));
        std::cout << "[DEBUG] ObstacleManager: Loaded - " << filename << std::endl;
        return true;
    }
    return false;
}

bool ObstacleManager::add_obstacle_from_vertices(const std::vector<Vec2>& vertices) {
    if (vertices.size() < 3) {
        std::cerr << "[ERROR] ObstacleManager: At least 3 vertices required" << std::endl;
        return false;
    }
    PolygonObstacle obs;
    obs.vertices = vertices;
    if (obs.vertices.front().x != obs.vertices.back().x || obs.vertices.front().y != obs.vertices.back().y) {
        obs.vertices.push_back(obs.vertices.front());
    }
    obstacles.push_back(std::move(obs));
    return true;
}

bool ObstacleManager::is_point_obstructed(const Vec2& p) const {
    for (const auto& obs : obstacles) {
        if (obs.point_inside(p)) return true;
    }
    return false;
}

size_t ObstacleManager::get_obstacle_count() const {
    return obstacles.size();
}