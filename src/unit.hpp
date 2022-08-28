#pragma once
#include "physicsentity.hpp"
#include "BZZRE/subsystems/input.hpp"
#include "camera.hpp"

class Unit : public PhysicsEntity
{
    protected:
        static inline Unit* current;
        b2Vec2 targetpos;
        float m_maxspeed;
        bool lock;
        void BaseUnitUpdate();
        void IsClickOnMe();
        virtual void Ability() = 0;
    public:
        void Follow();
        
};