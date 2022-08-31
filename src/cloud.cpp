#pragma once
#include "unit.hpp"
#include "cloud.hpp"
#include "stdio.h"
#include "animal.hpp"
#include "overlapcheck.hpp"
Cloud::Cloud()
{
    m_maxspeed = 5;
    type = Constants::PC_CLOUD;
}
void PutWater(b2Vec2 position, b2Shape* shape, float stride);

void Cloud::Update() {
    BaseUnitUpdate();
}

void Cloud::Draw() {
    
}

void Cloud::Ability()
{
    printf("Rain :D\n");
    auto pos = body->GetPosition();

    OverlapCheck<Animal*, Constants::PC_ANIMAL, false> queue;
    Animal** animals = queue.OverlapCircle(body->GetWorld(), pos, 50);
    for(int i = 0; i < arrlen(animals); i++)
    {
        animals[i]->GetScared(pos.x);
    }
    b2CircleShape shape;
    shape.m_radius =  body->GetFixtureList()->GetShape()->m_radius * 0.75f;
    PutWater(body->GetPosition(), &shape, 0);
    dead = true;
    /*//HACK: MUST IMPLEMENT PROPER OVERLAP TESTS INSTEAD OF THIS MESS.
    b2Vec2 lowerBound, upperBound;
    lowerBound = pos;
    upperBound = pos;
    float rainArea = 67;
    lowerBound.x -= rainArea;
    lowerBound.y = pos.y - 100000;
    upperBound.x += rainArea;

    b2AABB aabb = {lowerBound, upperBound};
    queryaabbcallback cb(pos.x);
    body->GetWorld()->QueryAABB(&cb, aabb);*/
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
