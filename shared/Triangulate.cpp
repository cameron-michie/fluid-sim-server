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
#include "Coord.h"

Triangulate::Triangulate() {}

Coord* Triangulate::triangulate(Coord* particle_coords, size_t input_size) {

    // Flatten coordinates for Delaunator
    std::vector<double> flat_coords;
    for (int i=0; i < input_size; ++i) {
        Coord coord = *(particle_coords+i); // Access ith element in vector starting at coord*
        flat_coords.push_back(static_cast<double>(coord.x));
        flat_coords.push_back(static_cast<double>(coord.y));
    }

    // Perform Delaunay triangulation
    delaunator::Delaunator delaunay(flat_coords);
    const auto& triangles = delaunay.triangles;

    // Compute area of each triangle
    std::vector<double> triangle_areas(triangles.size() / 3);
    for (size_t i = 0; i < triangles.size(); i += 3) {
        size_t t0 = triangles[i];
        size_t t1 = triangles[i + 1];
        size_t t2 = triangles[i + 2];

        const Coord& v0 = particle_coords[t0];
        const Coord& v1 = particle_coords[t1];
        const Coord& v2 = particle_coords[t2];

        double area = 0.5 * std::abs(
            (v1.x - v0.x) * (v2.y - v0.y) -
            (v2.x - v0.x) * (v1.y - v0.y)
        );

        triangle_areas[i / 3] = area;
    }

    // Accumulate areas for each vertex
    std::vector<double> vertex_area_sum(input_size, 0.0);
    for (size_t i = 0; i < triangles.size(); i++) {
        size_t vertex_index = triangles[i];
        size_t triangle_index = i / 3;
        vertex_area_sum[vertex_index] += triangle_areas[triangle_index];
    }

    // Compute density and set z for each vertex
    std::vector<float> vertex_densities(input_size, 0.0f);
    for (size_t i = 0; i < input_size; ++i) {
        double total_area = vertex_area_sum[i];
        float density = (total_area > 0.0) ? static_cast<float>(Window::heightMultiplier / total_area) : 0.0f;
        vertex_densities[i] = density;
    }

    // Build triangulatedCoords
    for (size_t i = 0; i < triangles.size(); i += 3) {
        size_t t0 = triangles[i];
        size_t t1 = triangles[i + 1];
        size_t t2 = triangles[i + 2];

        Coord v0 = particle_coords[t0];
        v0.z = vertex_densities[t0]; // Set z to density
        Coord v1 = particle_coords[t1];
        v1.z = vertex_densities[t1];
        Coord v2 = particle_coords[t2];
        v2.z = vertex_densities[t2];

        triangulatedCoords.push_back(v0);
        triangulatedCoords.push_back(v1);
        triangulatedCoords.push_back(v2);
    }

    return triangulatedCoords.data();
}