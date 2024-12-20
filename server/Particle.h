#ifndef PARTICLE_H
#define PARTICLE_H

#include "Vector2f.h"

class Particle {
    public:
        Vector2f position;
        Vector2f predictedPosition;        
        Vector2f velocity;
        float density;
        float pressure;
        float radius;
        float mass;
        static const float fluidParticleRadius;
        static const float bombParticleRadius;
        static const float fluidParticleMass;
        static const float bombParticleMass;
        Particle( 
            Vector2f position,
            Vector2f velocity,
            float radius,
            float mass);
};

#endif
