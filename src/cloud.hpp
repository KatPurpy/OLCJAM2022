#pragma once
#include "unit.hpp"

class Cloud : public Unit
{
    public:
        Cloud();
        void Update();
        void Draw();
        b2Body* CreateBody(b2World*);
        void OnCollisionEnter(b2Body*);
};