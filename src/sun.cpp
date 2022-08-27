#include "sun.hpp"

Sun::Sun()
{
    m_maxspeed = 20;
}

void Sun::Update() 
{
    BaseUnitUpdate();
}

void Sun::Draw()
{

}

b2Body* Sun::CreateBody(b2World* world)
{
    b2BodyDef def;
    def.gravityScale = 0;
    def.type = b2_dynamicBody;
    def.allowSleep = false;
    
    b2Body* body = world->CreateBody(&def);
    b2CircleShape shape;
    shape.m_radius = 6;
    body->CreateFixture(&shape, 1);
    return body;
}
void Sun::OnCollisionEnter(b2Body*) {
    
}
