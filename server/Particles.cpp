#include "Particle.h"

Particle::Particle( 
    Vector2f position, Vector2f velocity, float radius, float mass) : 
    position(position), velocity(velocity), density(0.0f), pressure(0.0f), mass(mass), radius(radius), predictedPosition(position) {}

const float Particle::fluidParticleRadius = 30.0f;
const float Particle::bombParticleRadius = 2000.0f;
const float Particle::fluidParticleMass= 1.0f;
const float Particle::bombParticleMass = 1.0f;
