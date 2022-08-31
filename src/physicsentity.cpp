#include "physicsentity.hpp"

PhysicsEntity::~PhysicsEntity()
{
    body->GetWorld()->DestroyBody(body);
}

void PhysicsEntity::Update()
{
    burnData.Update();
}

void PhysicsEntity::Instantiate(b2World* world)
{
    this->body = CreateBody(world);
    this->body->SetUserData((void*)this);
}
void PhysicsEntity::OnParticleColisionEnter(b2ParticleSystem* particleSystem,
                               b2ParticleBodyContact* particleBodyContact) 
{
    if(particleSystem->GetUserDataBuffer()[particleBodyContact->index] == Constants::particleFireTag
    && particleSystem->GetParticleLifetime(particleBodyContact->index) >= Constants::fireSmokeThreshold
    && canBeSetOnFire)
    {
        burnData.SetOnFire(this);
    }
}

void PhysicsEntity::OnCollisionEnter(PhysicsEntity* other)
{

    if(canBeSetOnFire && other->type & Constants::PC_BURNING)
    {
        burnData.SetOnFire(this);
    }
}

#include "stdio.h"
void PhysicsEntity::Destroy(bool)
{
    dead = true;
    printf("WARNING, A PHYSICS ENTITY DOES NOT HAVE Destroy() defined!\n");
}