#pragma once
#include "unit.hpp"

class Sun : public Unit
{
    public:
        Sun();
        b2BodyDef BodyDef();
        void Update();
        void Draw();
        b2Body* CreateBody(b2World*);
        void OnCollisionEnter(b2Body*);
};