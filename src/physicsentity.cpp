#include "physicsentity.hpp"

void PhysicsEntity::Instantiate(b2World* world)
{
    this->body = CreateBody(world);
    this->body->SetUserData((void*)this);
}