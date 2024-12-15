#include "Triangulate.h"
#include "delaunator.hpp"
#include <unordered_map>
#include <cmath>

Triangulate::Triangulate() {}

Coord* Triangulate::triangulate(Coord* coords, std::size_t count) {
    // Flatten Coord array into a vector of doubles
    std::vector<double> flatCoords;
    flatCoords.reserve(count * 2); // Only x and y are needed
    for (std::size_t i = 0; i < count; ++i) {
        flatCoords.push_back(coords[i].x);
        flatCoords.push_back(coords[i].y);
    }

    // Perform Delaunay triangulation
    delaunator::Delaunator d(flatCoords);

    // Initialize boundary status, densities, and counts
    std::vector<bool> isBoundaryPoint(count, false);
    std::vector<float> densities(count, 0.0f);
    std::vector<int> counts(count, 0);

    // Calculate densities and counts from triangles
    for (std::size_t i = 0; i < d.triangles.size(); i += 3) {
        std::array<std::size_t, 3> triangle = {
            d.triangles[i], d.triangles[i + 1], d.triangles[i + 2]
        };

        for (auto vertex : triangle) {
            densities[vertex] += 1.0f; // Example density increment
            counts[vertex] += 1;
        }
    }

    // Mark boundary points
    calculateBoundaryPoints(d.triangles, isBoundaryPoint, count);

    // Compute average densities and assign them to Coord.z
    computeAverageDensities(densities, counts, isBoundaryPoint, coords, count, 1.0f); // Assume heightNormalization = 1.0f

    return coords;
}

void Triangulate::calculateBoundaryPoints(const std::vector<std::size_t>& triangles,
                                          std::vector<bool>& isBoundaryPoint,
                                          std::size_t pointCount) {
    std::unordered_map<std::pair<std::size_t, std::size_t>, int, 
        std::hash<std::size_t>, std::equal_to<std::pair<std::size_t, std::size_t>>> edgeCounts;

    for (std::size_t i = 0; i < triangles.size(); i += 3) {
        std::array<std::pair<std::size_t, std::size_t>, 3> edges = {
            std::make_pair(triangles[i], triangles[i + 1]),
            std::make_pair(triangles[i + 1], triangles[i + 2]),
            std::make_pair(triangles[i + 2], triangles[i])
        };

        for (auto& edge : edges) {
            if (edge.first > edge.second) std::swap(edge.first, edge.second);

            edgeCounts[edge]++;
        }
    }

    for (const auto& [edge, count] : edgeCounts) {
        if (count == 1) { // Edge appears only once, marking it as a boundary edge
            isBoundaryPoint[edge.first] = true;
            isBoundaryPoint[edge.second] = true;
        }
    }
}

void Triangulate::computeAverageDensities(const std::vector<float>& densities,
                                          const std::vector<int>& counts,
                                          const std::vector<bool>& isBoundaryPoint,
                                          Coord* coords,
                                          std::size_t count,
                                          float heightNormalization) {
    for (std::size_t i = 0; i < count; ++i) {
        float averageDensity = (counts[i] > 0) ? heightNormalization * densities[i] / counts[i] : 0.0f;

        if (isBoundaryPoint[i]) {
            float totalDensity = 0.0f;
            int totalCount = 0;

            // Collect second-degree neighbors' data for boundary adjustment
            for (std::size_t j = 0; j < count; ++j) {
                if (i != j) { // Avoid self
                    totalDensity += densities[j];
                    totalCount += counts[j];
                }
            }

            averageDensity = (totalCount > 0) ? heightNormalization * totalDensity / totalCount : 0.0f;
        }

        coords[i].z = averageDensity; // Update z with computed density
    }
}
