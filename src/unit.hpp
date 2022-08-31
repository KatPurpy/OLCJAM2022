#pragma once
#include "physicsentity.hpp"
#include "BZZRE/subsystems/input.hpp"
#include "camera.hpp"

class Unit : public PhysicsEntity
{
    protected:
        static inline Unit* current;

        float m_maxspeed;
        bool lock = false; 
        bool destroyed;
        void BaseUnitUpdate();
        void IsClickOnMe();
        virtual void Ability() = 0;
    public:
        Unit();
        ~Unit();
                b2Vec2 targetpos;
        void Follow();
        void SetPosition(b2Vec2 vec);
};