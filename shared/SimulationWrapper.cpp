#include "SimulationWrapper.h"
#include "Window.h"

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

const Coord* SimulationWrapper::getParticleCoords(int* size) {
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

const Coord* SimulationWrapper::triangulate(Coord* coords) {
    // Determine the count of coordinates
    std::size_t count = 0;
    while (coords[count].x != 0 || coords[count].y != 0 || coords[count].z != 0) {
        ++count;
    }
    return triangulateInstance.triangulate(coords, count);
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

    const Coord* SimulationWrapper_getParticleCoords(SimulationWrapper* wrapper, int* size) {
        return wrapper->getParticleCoords(size);
    }

    const Coord* SimulationWrapper_triangulate(SimulationWrapper* wrapper, Coord* coords) {
        return wrapper->triangulate(coords);
    }

} // extern "C"
