#include "physicsentity.hpp"
#include "stdio.h"
PhysicsEntity::~PhysicsEntity() 
{ 
    printf("DESTROY ENT\n");
    body->GetWorld()->DestroyBody(body); 
}

void
PhysicsEntity::Update()
{
	burnData.Update();
}

void
PhysicsEntity::Instantiate(b2World* world)
{
	this->body = CreateBody(world);
	this->body->SetUserData((void*)this);
}
void
PhysicsEntity::OnParticleColisionEnter(b2ParticleSystem* particleSystem, b2ParticleBodyContact* particleBodyContact)
{
    if(canBeSetOnFire)
    {
        auto tag = particleSystem->GetUserDataBuffer()[particleBodyContact->index];
        if(tag == Constants::particleFireTag
        && particleSystem->GetParticleLifetime(particleBodyContact->index) >= Constants::fireSmokeThreshold)
        {
            burnData.SetOnFire(this);
        }

        if(tag == Constants::particleWaterTag)
        {
            burnData.PutoutFire();
        }
    }
}

void
PhysicsEntity::OnCollisionEnter(PhysicsEntity* other)
{

	if(canBeSetOnFire && other->type & Constants::PC_BURNING)
	{
		burnData.SetOnFire(this);
	}
}

#include "stdio.h"
void
PhysicsEntity::Kill(bool)
{
	dead = true;
    //__debugbreak();
	printf("WARNING, A PHYSICS ENTITY DOES NOT HAVE Destroy() defined!\n");
}