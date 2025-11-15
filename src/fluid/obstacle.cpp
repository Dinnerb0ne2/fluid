
#include <obstacle.hpp>
#include <GLFW/glfw3.h>

#include <fstream>
#include <sstream>
#include <cmath>

bool PolygonObstacle::load_from_file(const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    vertices.clear();
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        float x, y;
        if (iss >> x >> y) {
            vertices.push_back(Vec2(x, y));
        }
    }

    file.close();
    return vertices.size() >= 3; // Polygon needs at least 3 vertices
}

bool PolygonObstacle::point_inside(const Vec2& p) const {
    bool inside = false;
    size_t n = vertices.size();
    for (size_t i = 0, j = n-1; i < n; j = i++) {
        if (((vertices[i].y > p.y) != (vertices[j].y > p.y)) &&
            (p.x < (vertices[j].x - vertices[i].x) * (p.y - vertices[i].y) / (vertices[j].y - vertices[i].y) + vertices[i].x)) {
            inside = !inside;
        }
    }
    return inside;
}

bool ObstacleManager::add_obstacle_from_file(const char* filename) {
    PolygonObstacle obs;
    if (obs.load_from_file(filename)) {
        obstacles.push_back(obs);
        return true;
    }
    return false;
}

bool ObstacleManager::add_obstacle_from_vertices(const std::vector<Vec2>& vertices) {
    if (vertices.size() < 3) return false;
    PolygonObstacle obs;
    obs.vertices = vertices;
    obstacles.push_back(obs);
    return true;
}

bool ObstacleManager::is_point_obstructed(const Vec2& p) const {
    for (const auto& obs : obstacles) {
        if (obs.point_inside(p)) return true;
    }
    return false;
}