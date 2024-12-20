#ifndef SIMULATION_H
#define SIMULATION_H

#include <vector>
#include <functional>
#include <utility>
#include "Particle.h"
#include "Grid.h"
#include <fstream>
#include <string>
#include "Vector2f.h"

class Simulation {
private:
    std::vector<Particle> particles;
    std::vector<Particle> bombParticles;
    std::vector<int> sumParticlesInGridspaceInMap;
    std::vector<std::vector<Particle>> particlesInGridspaceInMap;
    std::unique_ptr<Grid> grid;

    static const float timeStep;
    static const float maxSpeed;
    static const float gasConstant;
    static const float restDensity;
    static const int repelUnitVector;
    static const float pressureMultiplier;

    float distanceBetweenParticles(Particle p, Particle o);
    Vector2f calculatePressureForce(Particle particle);
    Vector2f accumulatePressureForce(Particle p, Particle o, float (*kernelFunction)(float, float), float effectiveRadius);
    float randNum();

public:
    Simulation(std::unique_ptr<Grid> grid);
    void initialiseFluidParticles();
    void calculatePredictedPositions();
    void computeDensityPressure();
    void computeForces();
    void computeDensities();
    void integrate();
    void addNewParticles(int numParticles);
    void addBombParticle(float x, float y);
    void removeBombParticle();
    const std::vector<Particle>& getParticles();
    std::pair<int,int>* getParticlesPositions();
    std::vector<int> getSumParticlesInGridspaceInMap();
};

#endif // SIMULATION_H
