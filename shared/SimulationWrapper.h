#ifndef SIMULATION_WRAPPER_H
#define SIMULATION_WRAPPER_H

#include <memory>
#include <vector>
#include "Simulation.h"
#include "Window.h"
#include "RectangularGrid.h"
#include "Triangulate.h"
#include "Grid.h"
#include "Coord.h"
#include <fstream>

extern "C" {

    class SimulationWrapper {
        public:
            SimulationWrapper();
            ~SimulationWrapper();
            void iterate();
            void addBombParticle(float x, float y);
            void removeBombParticle();
            const Coord* getParticleCoords(size_t* size);
            const Coord* triangulate(const Coord* coords, size_t input_size, size_t* output_size);
        private:
            std::unique_ptr<RectangularGrid> grid;
            Simulation simulation;
            Triangulate triangulateInstance;
            int ticker;
            std::vector<Coord> coords;
            std::vector<Coord> triangulatedCoords;
    };

    SimulationWrapper* SimulationWrapper_new();
    void SimulationWrapper_delete(SimulationWrapper* wrapper);
    void SimulationWrapper_iterate(SimulationWrapper* wrapper);
    void SimulationWrapper_addBombParticle(SimulationWrapper* wrapper, float x, float y);
    void SimulationWrapper_removeBombParticle(SimulationWrapper* wrapper);
    const Coord* SimulationWrapper_getParticleCoords(SimulationWrapper* wrapper, size_t* size);
    const Coord* SimulationWrapper_triangulate(SimulationWrapper* wrapper, const Coord* coords, size_t input_size, size_t* output_size);
}

#endif // SIMULATION_WRAPPER_H
