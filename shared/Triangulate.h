#ifndef TRIANGULATE_H
#define TRIANGULATE_H

#include "Coord.h"
#include <vector>

struct Vertex {
    float x, y, z;    // Coordinates
    float density;    // Density at the vertex
};

struct Triangle {
    std::size_t v0, v1, v2; // Indices into the vertex list
};

class Triangulate {
public:
    Triangulate();

    // Perform triangulation and compute densities
    Coord* triangulate(Coord* coords, size_t input_size);

    // Getters for vertices and triangles
    const std::vector<Vertex>& getVertices() const;
    const std::vector<Triangle>& getTriangles() const;

private:
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;
    std::vector<Coord> triangulatedCoords;
};

#endif // TRIANGULATE_H
