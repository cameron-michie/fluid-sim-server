#include <memory>
#include "Simulation.h"
#include "Window.h"
#include "RectangularGrid.h"
#include "Grid.h"
#include <fstream>


void iterate(Simulation* simulation) {
    simulation->calculatePredictedPositions();
    simulation->computeDensities();
    simulation->computeDensityPressure();
    simulation->computeForces();
    simulation->integrate();
}

 
int main() {
    std::unique_ptr<RectangularGrid> grid = std::make_unique<RectangularGrid>(Window::width, Window::height, Window::subgridWidth, Window::subgridHeight);
    Simulation simulation(std::move(grid));
    simulation.initialiseFluidParticles();

    int ticker = 0;

    while (true) {
        iterate(&simulation);
        if (ticker == 20) simulation.addBombParticle(350.0f, 350.0f);
        if (ticker == 40) simulation.removeBombParticle();
        ticker++;
    }

    return 0;
}


