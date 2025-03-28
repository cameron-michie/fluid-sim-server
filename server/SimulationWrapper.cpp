#include "SimulationWrapper.h"
#include "Window.h"
#include <iostream>

SimulationWrapper::SimulationWrapper()
    :
      grid(std::make_unique<RectangularGrid>(Window::width, Window::height, Window::subgridWidth, Window::subgridHeight)),
      simulation(std::move(grid)),
      ticker(0),
      triangulateInstance()
{
    simulation.initialiseFluidParticles();
}

SimulationWrapper::~SimulationWrapper() {}

void SimulationWrapper::iterate() {
    simulation.calculatePredictedPositions();
    simulation.computeDensities();
    simulation.computeDensityPressure();
    simulation.computeForces();
    simulation.integrate();
    ticker++;
}

void SimulationWrapper::addBombParticle(float x, float y) {
    simulation.addBombParticle(x, y);
}

void SimulationWrapper::removeBombParticle() {
    simulation.removeBombParticle();
}

const Coord* SimulationWrapper::getParticleCoords(size_t* size) {
    const auto& particles = simulation.getParticles();
    coords.clear();
    coords.reserve(particles.size());
    for (const auto& particle : particles) {
        // Check if the particle is within the boundary margin
        if (particle.position.x > Window::boundaryMargin &&
            particle.position.x < Window::width - Window::boundaryMargin &&
            particle.position.y > Window::boundaryMargin &&
            particle.position.y < Window::height - Window::boundaryMargin) {
            coords.push_back({particle.position.x, particle.position.y, 0});
        }
    }
    *size = coords.size();
    return coords.data();
}

const Coord* SimulationWrapper::triangulate(Coord* coords_array, size_t input_size, size_t* output_size) {
    std::vector<Coord> inputCoords(coords_array, coords_array + input_size);
    triangulatedCoords = triangulateInstance.triangulate(inputCoords.data(), input_size);
    *output_size = triangulatedCoords.size();
    return triangulatedCoords.data();
}


extern "C" {

    SimulationWrapper* SimulationWrapper_new() {
        return new SimulationWrapper();
    }

    void SimulationWrapper_delete(SimulationWrapper* wrapper) {
        delete wrapper;
    }

    void SimulationWrapper_iterate(SimulationWrapper* wrapper) {
        wrapper->iterate();
    }

    void SimulationWrapper_addBombParticle(SimulationWrapper* wrapper, float x, float y) {
        wrapper->addBombParticle(x, y);
    }

    void SimulationWrapper_removeBombParticle(SimulationWrapper* wrapper) {
        wrapper->removeBombParticle();
    }

    const Coord* SimulationWrapper_getParticleCoords(SimulationWrapper* wrapper, size_t* size) {
        return wrapper->getParticleCoords(size);
    }

    const Coord* SimulationWrapper_triangulate(SimulationWrapper* wrapper, Coord* coords, size_t input_size, size_t* output_size) {
        return wrapper->triangulate(coords, input_size, output_size);
    }

} // extern "C"