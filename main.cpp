#include <iostream>
#include <vector>
#include <cmath>

class Vector2 {
public:
    float x, y;

    Vector2(float x, float y) : x(x), y(y) {}
};

class Object {
public:
    std::vector<Vector2> vectors;

    Object(const std::vector<Vector2>& vectors) : vectors(vectors) {}
};

class Line {
public:
    Vector2 start, end;
    Line() : start(0, 0), end(0, 0) {}

    Line(const Vector2& start, const Vector2& end) : start(start), end(end) {}
};

class Trapezoid {
public:
    Vector2 v0, v1, v2, v3;
    Line front, back, left, right;
    Vector2 position;
    float angle;

    Trapezoid(const Vector2& position, float angle, float fov, float near_plane, float far_plane)
        : v0(near_plane, std::tan(fov / 2) * near_plane),
          v1(near_plane, -std::tan(fov / 2) * near_plane),
          v2(far_plane, std::tan(fov / 2) * far_plane),
          v3(far_plane, -std::tan(fov / 2) * far_plane),
          position(position),
          angle(angle) {

        Vector2 v0_rotated = rotateAndPosition(v0, position, angle);
        Vector2 v1_rotated = rotateAndPosition(v1, position, angle);
        Vector2 v2_rotated = rotateAndPosition(v2, position, angle);
        Vector2 v3_rotated = rotateAndPosition(v3, position, angle);

        front = Line(v2_rotated, v3_rotated);
        back = Line(v1_rotated, v0_rotated);
        left = Line(v0_rotated, v2_rotated);
        right = Line(v3_rotated, v1_rotated);
    }

    static Vector2 rotateAndPosition(const Vector2& vect, const Vector2& position, float angle) {
        float x1 = std::cos(angle) * vect.x - std::sin(angle) * vect.y;
        float y1 = std::sin(angle) * vect.x + std::cos(angle) * vect.y;
        float x2 = x1 + position.x;
        float y2 = y1 + position.y;
        return Vector2(x2, y2);
    }

    static Vector2 backRotateAndPosition(const Vector2& vect, const Vector2& position, float angle) {
        float x1 = vect.x - position.x;
        float y1 = vect.y - position.y;
        float x2 = std::cos(-angle) * x1 - std::sin(-angle) * y1;
        float y2 = std::sin(-angle) * x1 + std::cos(-angle) * y1;
        return Vector2(x2, y2);
    }

    bool isInsideTrapezoid(const Vector2& point) const {
        Vector2 relativePoint = backRotateAndPosition(point, position, angle);
        bool insideX = v0.x <= relativePoint.x && relativePoint.x <= v2.x;
        float k = (v2.y - v0.y) / (v2.x - v0.x);
        bool insideY = -k * relativePoint.x <= relativePoint.y && relativePoint.y <= k * relativePoint.x;
        return insideX && insideY;
    }
};

class QuadTreeNode {
public:
    float x, y, width, height;
    std::vector<Vector2> vectors;
    int max_size;
    QuadTreeNode* top_left;
    QuadTreeNode* top_right;
    QuadTreeNode* bottom_left;
    QuadTreeNode* bottom_right;

    QuadTreeNode(float x, float y, float width, float height)
        : x(x), y(y), width(width), height(height), max_size(1),
          top_left(nullptr), top_right(nullptr), bottom_left(nullptr), bottom_right(nullptr) {}

    bool isLeaf() const {
        return top_left == nullptr && top_right == nullptr &&
               bottom_left == nullptr && bottom_right == nullptr;
    }
};

class QuadTree {
public:
    QuadTreeNode* root;
    float max_objects_per_node;

    QuadTree(float x, float y, float width, float height, float max_objects_per_node)
        : root(new QuadTreeNode(x, y, width, height)), max_objects_per_node(max_objects_per_node) {}

    ~QuadTree() {
        deleteTree(root);
    }

    void deleteTree(QuadTreeNode* node) {
        if (node) {
            deleteTree(node->top_left);
            deleteTree(node->top_right);
            deleteTree(node->bottom_left);
            deleteTree(node->bottom_right);
            delete node;
        }
    }

    void insert(Object& obj) {
        for (const auto& vector : obj.vectors) {
            insertRecurs(root, vector);
        }
    }

    std::vector<Vector2> search(float x, float y, float width, float height, const Trapezoid& trapezoid) {
        std::vector<Vector2> result;
        search(root, x, y, width, height, result, trapezoid);
        return result;
    }

    void insertRecurs(QuadTreeNode* node, const Vector2& vector) {
        if (node->isLeaf()) {
            node->vectors.push_back(vector);
            if (node->vectors.size() > static_cast<size_t>(max_objects_per_node)) {
                splitNode(node);
            }
        } else {
            if (node->x <= vector.x && vector.x <= node->x + node->width &&
                node->y <= vector.y && vector.y <= node->y + node->height) {
                if (vector.x > node->x + node->width / 2) {
                    if (vector.y < node->y + node->height / 2) {
                        insertRecurs(node->top_right, vector);
                    } else {
                        insertRecurs(node->bottom_right, vector);
                    }
                } else {
                    if (vector.y < node->y + node->height / 2) {
                        insertRecurs(node->top_left, vector);
                    } else {
                        insertRecurs(node->bottom_left, vector);
                    }
                }
            } else {
                // std::cout << "Point outside the node: "
                //           << node->x << ", " << node->y << ", " << node->x + node->width << ", " << node->y + node->height
                //           << ", point: " << vector.x << ", " << vector.y << std::endl;
            }
        }
    }

    void splitNode(QuadTreeNode* node) {
        float newWidth = node->width / 2;
        float newHeight = node->height / 2;

        node->top_left = new QuadTreeNode(node->x, node->y, newWidth, newHeight);
        node->top_right = new QuadTreeNode(node->x + newWidth, node->y, newWidth, newHeight);
        node->bottom_left = new QuadTreeNode(node->x, node->y + newHeight, newWidth, newHeight);
        node->bottom_right = new QuadTreeNode(node->x + newWidth, node->y + newHeight, newWidth, newHeight);

        for (const auto& vector : node->vectors) {
            insertRecurs(node, vector);
        }

        node->vectors.clear();
    }

    bool isOutsideOrInsideTrapezoidNode(const QuadTreeNode* node, float width, float height, const Trapezoid& trapezoid) {
        bool insideTrap =
            trapezoid.isInsideTrapezoid(Vector2(node->x, node->y)) &&
            trapezoid.isInsideTrapezoid(Vector2(node->x + width, node->y)) &&
            trapezoid.isInsideTrapezoid(Vector2(node->x, node->y + height)) &&
            trapezoid.isInsideTrapezoid(Vector2(node->x + width, node->y + height));

        bool outsideTrap = false;
        const Line sides[] = {trapezoid.right, trapezoid.back, trapezoid.left, trapezoid.front};

        for (const auto& line : sides) {
            if ((node->x <= line.start.x && line.start.x <= node->x + width && node->y <= line.start.y && line.start.y <= node->y + height) ||
                (node->x <= line.end.x && line.end.x <= node->x + width && node->y <= line.end.y && line.end.y <= node->y + height)) {
                outsideTrap = true;
            }
        }

        return insideTrap || outsideTrap;
    }

    void search(QuadTreeNode* node, float x, float y, float width, float height, std::vector<Vector2>& result, const Trapezoid& trapezoid) {
        if (node->isLeaf()) {
            for (const auto& vector : node->vectors) {
                if (x <= vector.x && vector.x <= x + width && y <= vector.y && vector.y <= y + height) {
                    if (trapezoid.isInsideTrapezoid(vector)) {
                        result.push_back(vector);
                    }
                }
            }
        } else {
            if (x <= node->x + node->width && x + width >= node->x &&
                y <= node->y + node->height && y + height >= node->y &&
                isOutsideOrInsideTrapezoidNode(node, width, height, trapezoid)) {
                search(node->top_left, x, y, width, height, result, trapezoid);
                search(node->top_right, x, y, width, height, result, trapezoid);
                search(node->bottom_left, x, y, width, height, result, trapezoid);
                search(node->bottom_right, x, y, width, height, result, trapezoid);
            }
        }
    }
};

class App {
public:
    QuadTree* quadTree;
    Trapezoid trapezoid;
    std::vector<Vector2> result;

    App(float width, float height)
        : quadTree(new QuadTree(0, 0, width, height, 1)),
          trapezoid(Vector2(100, 100), -180 * M_PI / 180, 45 * M_PI / 180, 10, 90) {
        Object obj1({Vector2(90, 80), Vector2(120, 80), Vector2(80, 80)});
        Object obj2({Vector2(70, 80), Vector2(30, 40), Vector2(20, 40)});
        Object obj3({Vector2(40, 60), Vector2(40, 51), Vector2(40, 51)});
        Object obj4({Vector2(200 - 10, 100 - 15), Vector2(170 - 10, 120 - 15), Vector2(50 - 10, 100 - 15)});

        quadTree->insert(obj1);
        quadTree->insert(obj2);
        quadTree->insert(obj3);
        quadTree->insert(obj4);
        result = quadTree->search(0, 0, width, height, trapezoid);
        for (const auto& vec : result) {
            std::cout << vec.x << " " << vec.y << std::endl;
        }
    }

    ~App() {
        delete quadTree;
    }
};

int main() {
    App app(200, 200);
    return 0;
}
