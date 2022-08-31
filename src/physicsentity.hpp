#pragma once
#include "Box2D.h"
#include "constants.hpp"
#include "HandmadeMath.h"
void MakeFireParticle(b2Vec2);

class PhysicsEntity
{
    public:
        struct BurnData
{
    PhysicsEntity* body{};
    bool onFire = false;
    float wetTime{};
    float health = 5; //in seconds
    
    const float fireEmitInterval = 0.15f;
    float fireTime{};
    
    inline void SetOnFire(PhysicsEntity* body)
    {
        if(onFire) return;
        this->body = body;
        body->type = (Constants::PhysicsCategory) ((uint16_t)body->type | (uint16_t)Constants::PC_BURNING);
        onFire = true;
    }

    inline void PutoutFire()
    {

    }

    inline void Update()
    {
        if(!onFire || !body) return;
        fireTime -= 1./60.;
        health -= 1./60.;
        wetTime -= 1./60.; 
        if(fireTime < 0)
        {
            EmitFire(body->body);
            fireTime = fireEmitInterval;
        }
        if(health < 0)
        {
            body->dead = true;
        }
    }

    inline void EmitFire(b2Body* body)
    {
        b2AABB aabb;
        aabb.lowerBound = b2Vec2(FLT_MAX,FLT_MAX);
        aabb.upperBound = b2Vec2(-FLT_MAX,-FLT_MAX); 
        for (b2Fixture* fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext())
        {
            aabb.Combine(aabb, fixture->GetAABB(0));
        }
        for(int i = 0; i < 5; i++)
        {
            float x = rand() / (float)(RAND_MAX);
            float y = rand() / (float)(RAND_MAX);
            MakeFireParticle({HMM_Lerp(aabb.lowerBound.x, x, aabb.upperBound.x), 
            HMM_Lerp(aabb.lowerBound.y, y, aabb.upperBound.y)});        
        }
        
        //fireEmitInterval
    }
};


        bool canBeSetOnFire{};
        BurnData burnData;
        bool dead{};
        Constants::PhysicsCategory type;
        b2Body* body;
        void Instantiate(b2World*);
        virtual b2Body* CreateBody(b2World* world) = 0;
        virtual void Update();
        virtual void Draw() = 0;
        virtual void OnCollisionEnter(PhysicsEntity* otherBody);
        virtual void OnParticleColisionEnter(b2ParticleSystem* particleSystem,
                                b2ParticleBodyContact* particleBodyContact);
        virtual void Destroy(bool silent);
        ~PhysicsEntity();



};


