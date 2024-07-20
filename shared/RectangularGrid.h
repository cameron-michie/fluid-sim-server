#ifndef RECTANGULAR_GRID_H
#define RECTANGULAR_GRID_H

#include "Grid.h"
#include <vector>
#include "Particle.h"

class RectangularGrid : public Grid {
    public:
        const int width, height, subgridWidth, subgridHeight;
        int numGridsX, numGridsY;
        RectangularGrid(int width, int height, int subgridWidth, int subgridHeight);
        std::vector<int> getAdjacentIndices(int index) const override;
        int getGridIndex(float x, float y) const override;
        int getTotalGridIndices() const override;
        void applyBoundaryConditions(Particle& particle) const override;
        std::tuple<float, float> getRandomPosition() const override;
    };

#endif // RECTANGULAR_GRID_H
