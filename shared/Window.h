#ifndef WINDOW_H
#define WINDOW_H

namespace Window {
    constexpr int width = 800;
    constexpr int height = 600;
    constexpr int subgridWidth = 30;
    constexpr int subgridHeight = 30;
    constexpr int numGridsX = width / subgridWidth;
    constexpr int numGridsY = height / subgridHeight;
    constexpr int boundaryMargin = 10;
    constexpr float heightMultiplier = 10000.0;
}

#endif