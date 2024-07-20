#ifndef GRID_H
#define GRID_H

#include <vector>
#include "Particle.h"

class Grid {
    public:
        virtual ~Grid();
        virtual std::vector<int> getAdjacentIndices(int index) const = 0;
        virtual int getGridIndex(float x, float y) const = 0;
        virtual int getTotalGridIndices() const = 0;
        virtual void applyBoundaryConditions(Particle& particle) const = 0;
        virtual std::tuple<float, float> getRandomPosition() const = 0;;
};

#endif