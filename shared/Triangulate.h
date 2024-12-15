#ifndef TRIANGULATE_H
#define TRIANGULATE_H

#include "Coord.h"
#include <vector>

class Triangulate {
public:
    Triangulate();
    Coord* triangulate(Coord* coords, std::size_t count);

private:
    void calculateBoundaryPoints(const std::vector<std::size_t>& triangles,
                                 std::vector<bool>& isBoundaryPoint,
                                 std::size_t pointCount);

    void computeAverageDensities(const std::vector<float>& densities,
                                 const std::vector<int>& counts,
                                 const std::vector<bool>& isBoundaryPoint,
                                 Coord* coords,
                                 std::size_t count,
                                 float heightNormalization);
};

#endif // TRIANGULATE_H
