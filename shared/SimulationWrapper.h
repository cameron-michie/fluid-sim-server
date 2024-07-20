#ifndef SIMULATION_WRAPPER_H
#define SIMULATION_WRAPPER_H

#include <memory>
#include <vector>
#include "Simulation.h"
#include "Window.h"
#include "RectangularGrid.h"
#include "Grid.h"
#include <fstream>

extern "C" {

    typedef struct {
        float x;
        float y;
    } Coord;

    class SimulationWrapper {
        public:
            SimulationWrapper();
            ~SimulationWrapper();
            void iterate();
            void addBombParticle(float x, float y);
            void removeBombParticle();
            const Coord* getParticleCoords(int* size);

        private:
            std::unique_ptr<RectangularGrid> grid;
            Simulation simulation;
            int ticker;
            std::vector<Coord> coords;
    };

    SimulationWrapper* SimulationWrapper_new();
    void SimulationWrapper_delete(SimulationWrapper* wrapper);
    void SimulationWrapper_iterate(SimulationWrapper* wrapper);
    void SimulationWrapper_addBombParticle(SimulationWrapper* wrapper, float x, float y);
    void SimulationWrapper_removeBombParticle(SimulationWrapper* wrapper);
    const Coord* SimulationWrapper_getParticleCoords(SimulationWrapper* wrapper, int* size);

}

#endif // SIMULATION_WRAPPER_H
