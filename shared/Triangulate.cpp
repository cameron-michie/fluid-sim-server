#include "delaunator.hpp"
#include <vector>
#include <cmath>
#include "Vector2f.h"
#include "Particle.h"
#include "Triangulate.h"


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

Triangulate::Triangulate(const std::vector<Particle>& particles)
    : particles(particles) {
    coords.reserve(particles.size() * 2);
    for (const auto& particle : particles) {
        coords.push_back(particle.position.x);
        coords.push_back(particle.position.y);
    }
}

std::vector<std::tuple<float, float, float>> Triangulate::triangulate() {
    delaunator::Delaunator d(coords);

    // Initialize boundary status, densities, and counts
    isBoundaryPoint.resize(particles.size(), false);
    densities.resize(particles.size(), 0.0f);
    counts.resize(particles.size(), 0);

    // Populate densities and counts based on triangles
    for (std::size_t i = 0; i < d.triangles.size(); i += 3) {
        std::array<std::size_t, 3> triangleIndices = {
            d.triangles[i], d.triangles[i + 1], d.triangles[i + 2]
        };

        for (auto vertex : triangleIndices) {
            densities[vertex] += 1.0f; // Placeholder: density increment per triangle
            counts[vertex] += 1;
        }
    }

    // Mark boundary points
    calculateBoundaryPoints(d);

    // Prepare result vector
    std::vector<std::tuple<float, float, float>> results;
    computeAverageDensities(results, 1.0f); // Assuming height_normalisation = 1.0

    return results;
}

void Triangulate::calculateBoundaryPoints(const delaunator::Delaunator& d) {
    std::unordered_map<std::size_t, std::vector<std::size_t>> edges;

    // Extract edges from triangles
    for (std::size_t i = 0; i < d.triangles.size(); i += 3) {
        std::array<std::pair<std::size_t, std::size_t>, 3> triangleEdges = {
            std::make_pair(d.triangles[i], d.triangles[i + 1]),
            std::make_pair(d.triangles[i + 1], d.triangles[i + 2]),
            std::make_pair(d.triangles[i + 2], d.triangles[i])
        };

        for (auto& edge : triangleEdges) {
            if (edge.first > edge.second) std::swap(edge.first, edge.second);

            edges[edge.first].push_back(edge.second);
        }
    }

    // Mark boundary edges (edges that occur only once)
    for (const auto& edge : edges) {
        if (edge.second.size() == 1) {
            isBoundaryPoint[edge.first] = true;
            isBoundaryPoint[edge.second.front()] = true;
        }
    }
}

void Triangulate::computeAverageDensities(std::vector<std::tuple<float, float, float>>& results, float heightNormalization) {
    results.reserve(particles.size());

    // Compute densities for all points
    for (std::size_t i = 0; i < particles.size(); ++i) {
        float averageDensity = (counts[i] > 0)
                                   ? heightNormalization * densities[i] / counts[i]
                                   : 0;

        // Adjust for boundary points with second-degree neighbors
        if (isBoundaryPoint[i]) {
            float totalDensity = 0;
            int totalCount = 0;

            // Find neighbors of this point
            for (std::size_t j = 0; j < particles.size(); ++j) {
                if (i == j) continue; // Skip self
                totalDensity += densities[j];
                totalCount += counts[j];
            }

            averageDensity = (totalCount > 0)
                                 ? heightNormalization * totalDensity / totalCount
                                 : 0;
        }

        results.emplace_back(particles[i].position.x, particles[i].position.y, averageDensity);
    }
}
