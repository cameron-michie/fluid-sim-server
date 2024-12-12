#ifndef TRIANGULATE_H
#define TRIANGULATE_H

#include "Vector2f.h"
#include "Particle.h"

class Triangulate {
public:
    Triangulate(const std::vector<Particle>& particles);
    std::vector<std::tuple<float, float, float>> triangulate();

private:
    const std::vector<Particle>& particles;
    std::vector<double> coords;
    std::vector<bool> isBoundaryPoint;
    std::vector<float> densities;
    std::vector<int> counts;

    void calculateBoundaryPoints(const delaunator::Delaunator& d);
    void computeAverageDensities(std::vector<std::tuple<float, float, float>>& results, float heightNormalization);
};

#enddef TRIANGULATE_H
