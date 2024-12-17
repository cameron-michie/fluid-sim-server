#include "Triangulate.h"
#include "Delaunator.h"
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <iostream>

Triangulate::Triangulate() {}

std::vector<Coord> Triangulate::triangulate(const std::vector<Coord>& coords) {
    // Logging input coordinates
    std::cout << "Triangulate::triangulate - Input size: " << coords.size() << std::endl;
    for (size_t i = 0; i < 100; ++i) {
        std::cout << "Coord " << i << ": (" << coords[i].x << ", " << coords[i].y << ", " << coords[i].z << ")" << std::endl;
    }

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

    // std::vector<double> coords2222 = {-100, 100, 350, 243, 375, -124, -285, -324};
    // //triangulation happens here
    // delaunator::Delaunator delaunay2(coords2222);


    // for(std::size_t i = 0; i < delaunay2.triangles.size(); i+=3) {
    //     printf(
    //         "Triangle points DELAUNAY 2 DON't MISS: [[%f, %f], [%f, %f], [%f, %f]]\n",
    //         delaunay2.coords[2 * delaunay2.triangles[i]],        //tx0
    //         delaunay2.coords[2 * delaunay2.triangles[i] + 1],    //ty0
    //         delaunay2.coords[2 * delaunay2.triangles[i + 1]],    //tx1
    //         delaunay2.coords[2 * delaunay2.triangles[i + 1] + 1],//ty1
    //         delaunay2.coords[2 * delaunay2.triangles[i + 2]],    //tx2
    //         delaunay2.coords[2 * delaunay2.triangles[i + 2] + 1] //ty2
    //     );
    // }

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

        double height = 0.0; // Set Z-coordinate to 0

        triangulatedCoords.push_back(Coord{ static_cast<float>(x0), static_cast<float>(y0), static_cast<float>(height) });
        triangulatedCoords.push_back(Coord{ static_cast<float>(x1), static_cast<float>(y1), static_cast<float>(height) });
        triangulatedCoords.push_back(Coord{ static_cast<float>(x2), static_cast<float>(y2), static_cast<float>(height) });
    }

    // Logging output triangulated coordinates
    std::cout << "Triangulate::triangulate - Output size: " << triangulatedCoords.size() << std::endl;
    for (size_t i = 0; i < 100; ++i) {
        std::cout << "Triangulated Coord " << i << ": (" << triangulatedCoords[i].x << ", " << triangulatedCoords[i].y << ", " << triangulatedCoords[i].z << ")" << std::endl;
    }

    return triangulatedCoords;
}
