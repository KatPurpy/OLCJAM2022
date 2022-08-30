#include "physicsentity.hpp"

PhysicsEntity::~PhysicsEntity()
{
    body->GetWorld()->DestroyBody(body);
}

void PhysicsEntity::Instantiate(b2World* world)
{
    this->body = CreateBody(world);
    this->body->SetUserData((void*)this);
}
void PhysicsEntity::OnParticleColisionEnter(b2ParticleSystem* particleSystem,
                               b2ParticleBodyContact* particleBodyContact) 
{
}
#include "stdio.h"
void PhysicsEntity::Destroy(bool)
{
    dead = true;
    printf("WARNING, A PHYSICS ENTITY DOES NOT HAVE Destroy() defined!\n");
}