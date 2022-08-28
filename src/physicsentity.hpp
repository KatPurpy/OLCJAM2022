#pragma once
#include "Box2D.h"
#include "constants.hpp"

class PhysicsEntity
{
    public:
        Constants::PhysicsCategory type;
        b2Body* body;
        void Instantiate(b2World*);
        virtual b2Body* CreateBody(b2World* world) = 0;
        virtual void Update() = 0;
        virtual void Draw() = 0;
        virtual void OnCollisionEnter(b2Body* otherBody) = 0;
};