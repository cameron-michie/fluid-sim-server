#ifndef TRIANGULATE_H
#define TRIANGULATE_H

#include "Coord.h"
#include <vector>

class Triangulate {
public:
    Triangulate();
    std::vector<Coord> triangulate(const std::vector<Coord>& coords);
};

#endif // TRIANGULATE_H
