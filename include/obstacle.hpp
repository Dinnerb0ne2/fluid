#ifndef OBSTACLE_HPP
#define OBSTACLE_HPP

#include <vector>
#include <string>
#include <iostream>
#include <nary_tree.hpp> // For Vec2 definition

// Polygon obstacle structure (declarations only)
struct PolygonObstacle {
    std::vector<Vec2> vertices; // Vertices (clockwise order)
    bool is_solid;              // Collidable flag

    PolygonObstacle();

    // Load from file
    bool load_from_file(const std::string& filename);

    // Point-in-polygon check
    bool point_inside(const Vec2& p) const;
};

// Obstacle manager class (declarations only)
class ObstacleManager {
private:
    std::vector<PolygonObstacle> obstacles;

public:
    ObstacleManager();

    // Add obstacle from file
    bool add_obstacle_from_file(const std::string& filename);

    // Add obstacle from vertices
    bool add_obstacle_from_vertices(const std::vector<Vec2>& vertices);

    // Check if point is obstructed
    bool is_point_obstructed(const Vec2& p) const;

    // Get obstacle count
    size_t get_obstacle_count() const;
};

#endif // OBSTACLE_HPP