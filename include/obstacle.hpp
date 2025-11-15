#ifndef OBSTACLE_HPP
#define OBSTACLE_HPP

#include <vector>
#include <fstream>
#include <sstream>
#include <cmath>
#include <string>
#include <stdexcept>
#include <iostream>
#include <nary_tree.hpp> // For Vec2 definition

// Polygon obstacle structure
struct PolygonObstacle {
    std::vector<Vec2> vertices; // Vertices (clockwise order)
    bool is_solid;              // Collidable flag
    
    PolygonObstacle() : is_solid(true) {}
    
    // Load from file
    bool load_from_file(const std::string& filename) {
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
            std::cerr << "[ERROR] Obstacle: Insufficient vertices (" << vertices.size() << " < 3)" << std::endl;
            return false;
        }
        
        // Close polygon
        if (vertices.front().x != vertices.back().x || vertices.front().y != vertices.back().y) {
            vertices.push_back(vertices.front());
        }
        
        return true;
    }
    
    // Point-in-polygon check (ray-casting algorithm)
    bool point_inside(const Vec2& p) const {
        bool inside = false;
        size_t n = vertices.size() - 1;
        
        for (size_t i = 0, j = n - 1; i < n; j = i++) {
            if (((vertices[i].y > p.y) != (vertices[j].y > p.y)) &&
                (p.x < (vertices[j].x - vertices[i].x) * (p.y - vertices[i].y) / (vertices[j].y - vertices[i].y) + vertices[i].x)) {
                inside = !inside;
            }
        }
        
        return inside;
    }
};

// Obstacle manager class
class ObstacleManager {
private:
    std::vector<PolygonObstacle> obstacles;

public:
    // Add obstacle from file
    bool add_obstacle_from_file(const std::string& filename) {
        PolygonObstacle obs;
        if (obs.load_from_file(filename)) {
            obstacles.push_back(obs);
            std::cout << "[DEBUG] ObstacleManager: Loaded - " << filename << std::endl;
            return true;
        }
        return false;
    }
    
    // Add obstacle from vertices
    bool add_obstacle_from_vertices(const std::vector<Vec2>& vertices) {
        if (vertices.size() < 3) {
            std::cerr << "[ERROR] ObstacleManager: At least 3 vertices required" << std::endl;
            return false;
        }
        
        PolygonObstacle obs;
        obs.vertices = vertices;
        if (obs.vertices.front().x != obs.vertices.back().x || obs.vertices.front().y != obs.vertices.back().y) {
            obs.vertices.push_back(obs.vertices.front());
        }
        obstacles.push_back(obs);
        return true;
    }
    
    // Check if point is obstructed
    bool is_point_obstructed(const Vec2& p) const {
        for (const auto& obs : obstacles) {
            if (obs.point_inside(p)) return true;
        }
        return false;
    }
    
    // Get obstacle count
    size_t get_obstacle_count() const {
        return obstacles.size();
    }
};

#endif // OBSTACLE_HPP