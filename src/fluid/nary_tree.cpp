#include <nary_tree.hpp>
#include <GLFW/glfw3.h>

#include <cmath>

template <int N>
void NaryTree<N>::subdivide(NaryTreeNode<N>* node) {
    if (!node->is_leaf) return;

    float dx = (node->max_bounds.x - node->min_bounds.x) / N;
    float dy = (node->max_bounds.y - node->min_bounds.y) / N;

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            Vec2 child_min(
                node->min_bounds.x + i * dx,
                node->min_bounds.y + j * dy
            );
            Vec2 child_max(
                node->min_bounds.x + (i+1) * dx,
                node->min_bounds.y + (j+1) * dy
            );
            node->children.push_back(new NaryTreeNode<N>(child_min, child_max));
        }
    }

    node->is_leaf = false;
}

template <int N>
bool NaryTree<N>::insert_cell(NaryTreeNode<N>* node, const Vec2& cell_pos, size_t index) {
    if (cell_pos.x < node->min_bounds.x || cell_pos.x > node->max_bounds.x ||
        cell_pos.y < node->min_bounds.y || cell_pos.y > node->max_bounds.y) {
        return false;
    }

    if (node->is_leaf) {
        float cell_size_x = node->max_bounds.x - node->min_bounds.x;
        float cell_size_y = node->max_bounds.y - node->min_bounds.y;

        if (cell_size_x <= cell_size_threshold && cell_size_y <= cell_size_threshold) {
            node->fluid_cell_indices.push_back(index);
            return true;
        } else {
            subdivide(node);
        }
    }

    for (auto child : node->children) {
        if (insert_cell(child, cell_pos, index)) {
            return true;
        }
    }

    return false;
}

template <int N>
NaryTree<N>::NaryTree(const Vec2& world_min, const Vec2& world_max, float threshold) 
    : cell_size_threshold(threshold) {
    root = new NaryTreeNode<N>(world_min, world_max);
}

template <int N>
void NaryTree<N>::build(const std::vector<Vec2>& cell_positions) {
    for (size_t i = 0; i < cell_positions.size(); ++i) {
        insert_cell(root, cell_positions[i], i);
    }
}

template <int N>
std::vector<size_t> NaryTree<N>::query_range(const Vec2& min, const Vec2& max) {
    std::vector<size_t> result;
    std::vector<NaryTreeNode<N>*> stack;
    stack.push_back(root);

    while (!stack.empty()) {
        NaryTreeNode<N>* node = stack.back();
        stack.pop_back();

        if (node->max_bounds.x < min.x || node->min_bounds.x > max.x ||
            node->max_bounds.y < min.y || node->min_bounds.y > max.y) {
            continue;
        }

        if (node->is_leaf) {
            result.insert(result.end(), node->fluid_cell_indices.begin(), node->fluid_cell_indices.end());
        } else {
            for (auto child : node->children) {
                stack.push_back(child);
            }
        }
    }

    return result;
}

// Explicit instantiation for N=4 (can add N=5,6 etc. as needed)
template class NaryTree<4>;
template class NaryTreeNode<4>;