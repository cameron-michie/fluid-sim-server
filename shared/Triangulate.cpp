#include "Triangulate.h"
#include "Delaunator.h"
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <iostream>
#include "Window.h"

Triangulate::Triangulate() {}

std::vector<Coord> Triangulate::triangulate(const std::vector<Coord>& coords) {
    // Logging input coordinates
    std::cout << "Triangulate::triangulate - Input size: " << coords.size() << std::endl;
    for (size_t i = 0; i < 100; ++i) {
        std::cout << "Coord " << i << ": (" << coords[i].x << ", " << coords[i].y << ", " << coords[i].z << ")" << std::endl;
    }

    // Compute min and max of input coordinates
    float min_x = std::numeric_limits<float>::max();
    float max_x = std::numeric_limits<float>::lowest();
    float min_y = std::numeric_limits<float>::max();
    float max_y = std::numeric_limits<float>::lowest();

    for (const auto& coord : coords) {
        if (coord.x < min_x) min_x = coord.x;
        if (coord.x > max_x) max_x = coord.x;
        if (coord.y < min_y) min_y = coord.y;
        if (coord.y > max_y) max_y = coord.y;
    }

    // Avoid division by zero
    float range_x = max_x - min_x;
    float range_y = max_y - min_y;
    if (range_x == 0) range_x = 1.0f;
    if (range_y == 0) range_y = 1.0f;

    // Compute scaling factors
    float scale_x = static_cast<float>(Window::width) / range_x;
    float scale_y = static_cast<float>(Window::height) / range_y;

    // Flatten coordinates for Delaunator
    std::vector<double> flat_coords;
    for (const auto& coord : coords) {
        flat_coords.push_back(static_cast<double>(coord.x));
        flat_coords.push_back(static_cast<double>(coord.y));
    }

    // Compute Delaunay triangulation
    delaunator::Delaunator delaunay(flat_coords);
    const auto& triangles = delaunay.triangles;

    // Prepare triangulated coordinates
    std::vector<Coord> triangulatedCoords;

    for (size_t i = 0; i < triangles.size(); i += 3) {
        size_t t0 = triangles[i];
        size_t t1 = triangles[i + 1];
        size_t t2 = triangles[i + 2];

        double x0 = delaunay.coords[2 * t0];
        double y0 = delaunay.coords[2 * t0 + 1];

        double x1 = delaunay.coords[2 * t1];
        double y1 = delaunay.coords[2 * t1 + 1];

        double x2 = delaunay.coords[2 * t2];
        double y2 = delaunay.coords[2 * t2 + 1];

        // Rescale coordinates to [0, Window::width] and [0, Window::height]
        float rescaled_x0 = (static_cast<float>(x0) - min_x) * scale_x;
        float rescaled_y0 = (static_cast<float>(y0) - min_y) * scale_y;
        float rescaled_x1 = (static_cast<float>(x1) - min_x) * scale_x;
        float rescaled_y1 = (static_cast<float>(y1) - min_y) * scale_y;
        float rescaled_x2 = (static_cast<float>(x2) - min_x) * scale_x;
        float rescaled_y2 = (static_cast<float>(y2) - min_y) * scale_y;

        float height = 0.0f; // Set Z-coordinate to 0

        triangulatedCoords.push_back(Coord{ rescaled_x0, rescaled_y0, height });
        triangulatedCoords.push_back(Coord{ rescaled_x1, rescaled_y1, height });
        triangulatedCoords.push_back(Coord{ rescaled_x2, rescaled_y2, height });
    }

    // Logging output triangulated coordinates
    std::cout << "Triangulate::triangulate - Output size: " << triangulatedCoords.size() << std::endl;
    for (size_t i = 0; i < 100; ++i) {
        std::cout << "Triangulated Coord " << i << ": (" << triangulatedCoords[i].x << ", " << triangulatedCoords[i].y << ", " << triangulatedCoords[i].z << ")" << std::endl;
    }

    return triangulatedCoords;
}
