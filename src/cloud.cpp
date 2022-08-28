#pragma once
#include "unit.hpp"
#include "cloud.hpp"
#include "stdio.h"

Cloud::Cloud()
{
    m_maxspeed = 5;
}

void Cloud::Update() {
    BaseUnitUpdate();
}

void Cloud::Draw() {
    
}

void Cloud::Ability()
{
    printf("Rain :D\n");
}

b2Body* Cloud::CreateBody(b2World* world) {
    b2BodyDef def;
    def.gravityScale = 0;
    def.type = b2_dynamicBody;
    def.allowSleep = false;
    
    b2Body* body = world->CreateBody(&def);
    b2CircleShape shape;
    
    shape.m_radius = 4.5f;
    body->CreateFixture(&shape, 1);
    return body;    
}

void Cloud::OnCollisionEnter(b2Body*) 
{
    
}
