#ifndef TRIANGULATE_H
#define TRIANGULATE_H

#include "Coord.h"
#include <vector>

struct Vertex {
    float x, y, z; 
    float density;
};

struct Triangle {
    std::size_t v0, v1, v2; 
};

class Triangulate {
public:
    Triangulate();
    std::vector<Coord> triangulate(Coord* coords, size_t input_size);

private:
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;
    std::vector<Coord> triangulatedCoords;
};

#endif // TRIANGULATE_H
