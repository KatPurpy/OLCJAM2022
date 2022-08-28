#pragma once
#include "unit.hpp"
#include "cloud.hpp"
#include "stdio.h"
#include "animal.hpp"
Cloud::Cloud()
{
    m_maxspeed = 5;
}

void Cloud::Update() {
    BaseUnitUpdate();
}

void Cloud::Draw() {
    
}

class queryaabbcallback : public b2QueryCallback
{
    float x;
    public:
        queryaabbcallback(float x) : x(x)
        {

        }
        bool ReportFixture(b2Fixture* fixture)
        {

            auto data = (Animal*)fixture->GetBody()->GetUserData();
            printf("data %p\n", data);
            if(data && data->type == Constants::PC_ANIMAL)
            {
                data->GetScared(x);
            }
        }
};

void Cloud::Ability()
{
    printf("Rain :D\n");
    auto pos = body->GetPosition();
    //HACK: MUST IMPLEMENT PROPER OVERLAP TESTS INSTEAD OF THIS MESS.
    b2Vec2 lowerBound, upperBound;
    lowerBound = pos;
    upperBound = pos;
    float rainArea = 67;
    lowerBound.x -= rainArea;
    lowerBound.y = pos.y - 100000;
    upperBound.x += rainArea;

    b2AABB aabb = {lowerBound, upperBound};
    queryaabbcallback cb(pos.x);
    body->GetWorld()->QueryAABB(&cb, aabb);
}

b2Body* Cloud::CreateBody(b2World* world) {
    
    b2BodyDef def;
    def.gravityScale = 0;
    def.type = b2_dynamicBody;
    def.allowSleep = false;
    
    b2Body* body = world->CreateBody(&def);
    b2CircleShape shape;
    shape.m_radius = 4.5f;

    b2FixtureDef fdef;
    fdef.filter.categoryBits = Constants::PC_PLAYERUNIT;
    fdef.filter.maskBits = Constants::PLAYERUNIT_COLLIDESWITH;
    fdef.shape = &shape;

    body->CreateFixture(&fdef);

    return body;    
}

void Cloud::OnCollisionEnter(b2Body*) 
{
    
}
