#include "Kernel.h"
#include "Particle.h"
#include <cmath>

Kernel::Kernel() {}

float Kernel::poly6Kernel(float r, float h) {
    if (r < 0 || r > h) return 0.0f;
    float factor = -315.0f / (64.0f * M_PI * std::pow(h, 9));
    return factor * std::pow(h * h - r * r, 3);
}

float Kernel::smoothKernel(float r, float h) {
    if (r <= 0) return 0.0f;
    float factor = -45.0f / (M_PI * std::pow(h, 6));
    return factor * std::pow(h - r, 2);
}

float Kernel::gaussianKernel(float r, float h) {
    if (r <= 0 || r >= h) return 0.0f;
    float factor = 1 / 5 * std::sqrt(2 * M_PI);
    return factor * std::exp(std::pow(r / (h), 2));
}