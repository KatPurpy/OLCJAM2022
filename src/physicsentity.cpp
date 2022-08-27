#include "physicsentity.hpp"

void PhysicsEntity::Instantiate(b2World* world)
{
    this->body = CreateBody(world);
    this->body->GetUserData().pointer = (uintptr_t)(void*)this;
}