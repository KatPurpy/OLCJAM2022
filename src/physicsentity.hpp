#pragma once
#include "box2d/b2_body.h"
#include "box2d/box2d.h"

class PhysicsEntity
{
    public:
        b2Body* body;
        void Instantiate(b2World*);
        virtual b2Body* CreateBody(b2World* world) = 0;
        virtual void Update() = 0;
        virtual void Draw() = 0;
        virtual void OnCollisionEnter(b2Body* otherBody) = 0;
};