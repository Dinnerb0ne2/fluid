#ifndef NARY_TREE_HPP
#define NARY_TREE_HPP

#include <vector>
#include <cmath>
#include <stdexcept>

// Custom 2D vector struct for position calculations
struct Vec2 {
    float x, y;
    
    // Default constructor
    Vec2() : x(0.0f), y(0.0f) {}
    
    // Parameterized constructor
    Vec2(float x_val, float y_val) : x(x_val), y(y_val) {}
    
    // Vector addition
    Vec2 operator+(const Vec2& other) const {
        return Vec2(x + other.x, y + other.y);
    }
    
    // Vector subtraction
    Vec2 operator-(const Vec2& other) const {
        return Vec2(x - other.x, y - other.y);
    }
    
    // Scalar multiplication
    Vec2 operator*(float scalar) const {
        return Vec2(x * scalar, y * scalar);
    }
};

// Node structure for N-ary tree spatial partitioning
template <int N>
class NaryTreeNode {
public:
    Vec2 min_bounds;       // Minimum bounds of the node (x1, y1)
    Vec2 max_bounds;       // Maximum bounds of the node (x2, y2)
    std::vector<size_t> cell_indices; // Indices of fluid cells in this node
    std::vector<NaryTreeNode<N>*> children; // Child nodes (if subdivided)
    bool is_leaf;          // Flag indicating if node is a leaf (no children)
    
    // Constructor: Initialize node with bounds
    NaryTreeNode(const Vec2& min, const Vec2& max) 
        : min_bounds(min), max_bounds(max), is_leaf(true) {}
    
    // Destructor: Recursively delete child nodes to avoid memory leaks
    ~NaryTreeNode() {
        for (auto child : children) {
            delete child;
        }
    }
};

// N-ary tree class for efficient neighbor queries
template <int N>
class NaryTree {
private:
    NaryTreeNode<N>* root;          // Root node of the tree（修复：先声明 root）
    float cell_size_threshold;      // Threshold to stop subdividing nodes（后声明 threshold）
    const float MIN_NODE_SIZE = 4.0f; // Minimum node size (prevents infinite subdivision)
    
    // Recursively subdivide a node into NxN child nodes
    void subdivide(NaryTreeNode<N>* node) {
        if (!node->is_leaf) return;
        
        // Calculate child node dimensions
        float dx = (node->max_bounds.x - node->min_bounds.x) / N;
        float dy = (node->max_bounds.y - node->min_bounds.y) / N;
        
        // Prevent subdivision below minimum size
        if (dx < MIN_NODE_SIZE || dy < MIN_NODE_SIZE) {
            return;
        }
        
        // Create NxN child nodes
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                Vec2 child_min(
                    node->min_bounds.x + i * dx,
                    node->min_bounds.y + j * dy
                );
                Vec2 child_max(
                    node->min_bounds.x + (i + 1) * dx,
                    node->min_bounds.y + (j + 1) * dy
                );
                node->children.push_back(new NaryTreeNode<N>(child_min, child_max));
            }
        }
        
        node->is_leaf = false;
    }
    
    // Recursively insert a cell into the appropriate node
    bool insert_cell(NaryTreeNode<N>* node, const Vec2& cell_pos, size_t cell_idx) {
        // Check if cell is outside current node bounds
        if (cell_pos.x < node->min_bounds.x || cell_pos.x > node->max_bounds.x ||
            cell_pos.y < node->min_bounds.y || cell_pos.y > node->max_bounds.y) {
            return false;
        }
        
        // If leaf node: add cell or subdivide
        if (node->is_leaf) {
            float node_width = node->max_bounds.x - node->min_bounds.x;
            float node_height = node->max_bounds.y - node->min_bounds.y;
            
            // Add cell to leaf if below threshold, else subdivide
            if (node_width <= cell_size_threshold && node_height <= cell_size_threshold) {
                node->cell_indices.push_back(cell_idx);
                return true;
            } else {
                subdivide(node);
            }
        }
        
        // Insert into child nodes (recursive)
        for (auto child : node->children) {
            if (insert_cell(child, cell_pos, cell_idx)) {
                return true;
            }
        }
        
        return false;
    }

public:
    // 修复：初始化顺序与声明顺序一致（root 后于 cell_size_threshold）
    NaryTree(const Vec2& world_min, const Vec2& world_max, float threshold) 
        : cell_size_threshold(threshold), root(nullptr) {
        // Validate world bounds
        if (world_min.x >= world_max.x || world_min.y >= world_max.y) {
            throw std::invalid_argument("Invalid world bounds: min >= max");
        }
        root = new NaryTreeNode<N>(world_min, world_max);
    }
    
    // Destructor: Delete root node (recursively cleans up children)
    ~NaryTree() {
        delete root;
    }
    
    // Build tree from list of cell positions
    void build(const std::vector<Vec2>& cell_positions) {
        if (!root) {
            throw std::runtime_error("Tree root not initialized");
        }
        
        // Insert each cell into the tree
        for (size_t i = 0; i < cell_positions.size(); ++i) {
            if (!insert_cell(root, cell_positions[i], i)) {
                throw std::runtime_error("Failed to insert cell into tree");
            }
        }
    }
    
    // Query all cells within a rectangular range
    std::vector<size_t> query_range(const Vec2& min, const Vec2& max) const {
        std::vector<size_t> result;
        if (!root) return result;
        
        std::vector<NaryTreeNode<N>*> stack;
        stack.push_back(root);
        
        // Iterative DFS to avoid stack overflow with deep trees
        while (!stack.empty()) {
            NaryTreeNode<N>* node = stack.back();
            stack.pop_back();
            
            // Skip nodes that don't overlap with query range
            if (node->max_bounds.x < min.x || node->min_bounds.x > max.x ||
                node->max_bounds.y < min.y || node->min_bounds.y > max.y) {
                continue;
            }
            
            // Add cells from leaf nodes
            if (node->is_leaf) {
                result.insert(result.end(), node->cell_indices.begin(), node->cell_indices.end());
            } else {
                // Add child nodes to stack for further checking
                for (auto child : node->children) {
                    stack.push_back(child);
                }
            }
        }
        
        return result;
    }
};

// Explicit instantiation for 4-ary tree (used in fluid simulation)
template class NaryTree<4>;
template class NaryTreeNode<4>;

#endif // NARY_TREE_HPP