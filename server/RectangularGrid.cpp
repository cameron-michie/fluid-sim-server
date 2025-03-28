#include "RectangularGrid.h"
#include <vector>
#include "Particle.h"
#include <iostream>
#include <cstdlib>

RectangularGrid::RectangularGrid(int width, int height, int subgridWidth, int subgridHeight)
: width(width), height(height), subgridWidth(subgridWidth), subgridHeight(subgridHeight) {
    numGridsX = width / subgridWidth;
    numGridsY = height / subgridHeight;
}

std::vector<int> RectangularGrid::getAdjacentIndices(int index) const {
    std::vector<int> adjacentIndices;
    int gridX = index % numGridsX;
    int gridY = index / numGridsX;

    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            if (dx == 0 && dy == 0) continue;
            int neighborX = gridX + dx;
            int neighborY = gridY + dy;

            if (neighborX >= 0 && neighborX < numGridsX && neighborY >= 0 && neighborY < numGridsY) {
                int neighborIndex = neighborY * numGridsX + neighborX;
                adjacentIndices.push_back(neighborIndex);
            }
        }
    }
    return adjacentIndices;
}

int RectangularGrid::getGridIndex(float x, float y) const {
    int gridX = static_cast<int>(x / subgridWidth);
    int gridY = static_cast<int>(y / subgridHeight);
    if (gridX >= numGridsX) gridX = numGridsX - 1;
    if (gridY >= numGridsY) gridY = numGridsY - 1;
    if (gridX < 0) gridX = 0;
    if (gridY < 0) gridY = 0;
    return gridY * numGridsX + gridX;
}

int RectangularGrid::getTotalGridIndices() const {
    return numGridsX * numGridsY;
}

void RectangularGrid::applyBoundaryConditions(Particle& particle) const {
    if (particle.position.x <= 0) {
        particle.velocity.x *= -1.0f;
        particle.position.x = 0;
    }
    if (particle.position.x >= width) {
        particle.velocity.x *= -1.0f;
        particle.position.x = width;
    }
    if (particle.position.y <= 0) {
        particle.velocity.y *= -1.0f;
        particle.position.y = 0;
    }
    if (particle.position.y >= height) {
        particle.velocity.y *= -1.0f;
        particle.position.y = height;
    }
}

std::tuple<float, float> RectangularGrid::getRandomPosition() const {
    const float x = static_cast<float>(width) * std::rand() / static_cast<float>(RAND_MAX);
    const float y = static_cast<float>(height) * std::rand() / static_cast<float>(RAND_MAX);
    return std::make_tuple(x,y);
}