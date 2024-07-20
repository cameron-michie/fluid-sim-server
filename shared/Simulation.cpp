#include "Simulation.h"
#include "Kernel.h"
#include "Window.h"
#include <iostream>
#include <algorithm>
#include <chrono>
#include <thread>
#include "RectangularGrid.h"
#include <unordered_set>
#include <functional>

Simulation::Simulation(std::unique_ptr<Grid> grid) : grid(std::move(grid)) {}

const float Simulation::timeStep = 0.001f;
const float Simulation::maxSpeed = 5000.0f;
const float Simulation::gasConstant = 1000000.0f;
const float Simulation::pressureMultiplier = 1.0f;
const float Simulation::restDensity = 1.0f;
const int Simulation::repelUnitVector = 1; // 1 to make particles repel, -1 to make them for attract

void Simulation::initialiseFluidParticles() {
    int numParticlesWidth = 50;
    int numParticlesHeight = 50;
    for (int i = 0; i < numParticlesWidth; i++) {
        for (int j = 0; j < numParticlesHeight; j++) {
            float x, y;
            std::tie(x, y) = grid->getRandomPosition();
            Vector2f position = Vector2f(x, y);
            Vector2f velocity = Vector2f(0, 0);
            float radius = Particle::fluidParticleRadius;
            float mass = Particle::fluidParticleMass;
            Particle particle(position, velocity, radius, mass);
            particles.push_back(particle);
        }
    }
}

void Simulation::calculatePredictedPositions(){
    for (auto& particle : particles) {
        particle.predictedPosition = particle.position + particle.velocity * Simulation::timeStep;
    }
}

void Simulation::computeDensities() {
    int totalGrids = grid->getTotalGridIndices();
    sumParticlesInGridspaceInMap.assign(totalGrids, 0);
    particlesInGridspaceInMap.assign(totalGrids, std::vector<Particle>());
    for (const auto& particle : particles) {
        int gridIndex = grid->getGridIndex(particle.position.x, particle.position.y);
        sumParticlesInGridspaceInMap[gridIndex]++;
        particlesInGridspaceInMap[gridIndex].push_back(particle);
    }
}

void Simulation::computeDensityPressure() {
    for (auto& particle : particles) {
        particle.density = 0.0f;
        int particleGridIndex = grid->getGridIndex(particle.predictedPosition.x, particle.predictedPosition.y);
        std::vector<int> adjacentIndices = grid->getAdjacentIndices(particleGridIndex);
        adjacentIndices.push_back(particleGridIndex);

        for (const auto& adjacentIndex : adjacentIndices) {
            for (const auto& other : particlesInGridspaceInMap[adjacentIndex]) {
                float dist = std::sqrt(
                    std::pow(particle.predictedPosition.x - other.predictedPosition.x, 2) + 
                    std::pow(particle.predictedPosition.y - other.predictedPosition.y, 2) 
                );
                if (dist == 0 || dist >= particle.radius) continue; 
                particle.density += Kernel::smoothKernel(dist, particle.radius);;
            }
        }
        particle.pressure = (particle.density - restDensity) * gasConstant;
    }
}

Vector2f Simulation::calculatePressureForce(Particle particle){
    Vector2f pressureForce(0.0f, 0.0f);
    int particleGridIndex = grid->getGridIndex(particle.predictedPosition.x, particle.predictedPosition.y);
    std::vector<int> adjacentIndices = grid->getAdjacentIndices(particleGridIndex);
    adjacentIndices.push_back(particleGridIndex);

    for (const auto& adjacentIndex : adjacentIndices) {
        for (const auto& other : particlesInGridspaceInMap[adjacentIndex]) {
            pressureForce += accumulatePressureForce(particle, other, Kernel::smoothKernel, particle.radius);
        }
    }

    for (const auto& bombParticle : bombParticles) {
            pressureForce += accumulatePressureForce(particle, bombParticle, Kernel::smoothKernel, particle.radius*5);
        }

    return pressureForce;
}

Vector2f Simulation::accumulatePressureForce(Particle particle, Particle other, float (*kernelFunction)(float, float), float effectiveRadius){
    float dist = distanceBetweenParticles(particle, other);
    if (dist == 0 || dist >= effectiveRadius) {
        return Vector2f(0,0);
    }
    float kernelContribution = kernelFunction(dist, effectiveRadius) * Simulation::pressureMultiplier;
    Vector2f direction = (particle.predictedPosition - other.predictedPosition) / ( -1* repelUnitVector * dist );
    Vector2f pressureForceContribution = kernelContribution * direction * other.mass * (particle.pressure + other.pressure) / (2.0f);
    return pressureForceContribution;
}

void Simulation::computeForces() {
    for (auto& particle : particles) {
        Vector2f pressureForce = calculatePressureForce(particle);
        Vector2f collisionAcceleration = (particle.density != 0) ? pressureForce / particle.density : Vector2f(0.0f, 0.0f);
        particle.velocity += Simulation::timeStep * collisionAcceleration;
    }
}

void Simulation::integrate() {
    for (auto& particle : particles) {
        particle.position += Simulation::timeStep * particle.velocity;
        grid->applyBoundaryConditions(particle);
        particle.velocity = particle.velocity * 0.995f;
    }
}

void Simulation::addNewParticles(int numParticles) {
    int i_newParticles = numParticles;
    const Vector2f inlet_position = Vector2f(100.0f, 100.0f);
    for (int i = 0; i < i_newParticles; i++) {
        Vector2f position = inlet_position + Vector2f(20.0f * randNum(), 20.0f * randNum());
        Vector2f velocity = Vector2f(0, 0);
        float radius = Particle::fluidParticleRadius;
        float mass = Particle::fluidParticleMass;
        Particle particle(position, velocity, radius, 1.0f);
        particles.push_back(particle);
    }
}

void Simulation::addBombParticle(float x, float y) {
    Vector2f position = Vector2f(x,y);
    Vector2f velocity = Vector2f(0, 0);
    float radius = Particle::bombParticleRadius;
    float mass = Particle::bombParticleRadius;
    Particle particle(position, velocity, radius, mass);
    bombParticles.push_back(particle);
}

void Simulation::removeBombParticle() {
    // bombParticles.erase(bombParticles.begin());
}

float Simulation::distanceBetweenParticles(Particle p, Particle o) {
    float dist = std::sqrt(
        std::pow(p.predictedPosition.x - o.predictedPosition.x, 2) + 
        std::pow(p.predictedPosition.y - o.predictedPosition.y, 2));
    return dist;
}

const std::vector<Particle>& Simulation::getParticles() {
    return particles;
}

std::vector<int> Simulation::getSumParticlesInGridspaceInMap() {
    return sumParticlesInGridspaceInMap;
}

float Simulation::randNum(){
    return std::rand() / static_cast<float>(RAND_MAX);
}

std::pair<int, int>* Simulation::getParticlesPositions() {
    const int numParticles = particles.size();
    std::pair<int, int>* particlePositions = new std::pair<int, int>[numParticles];
    for (int i = 0; i < numParticles; i++) {
        Particle particle = particles[i];
        int x = particle.position.x;
        int y = particle.position.y;
        particlePositions[i] = std::pair<int, int>(x, y);
    }
    return particlePositions;
}
