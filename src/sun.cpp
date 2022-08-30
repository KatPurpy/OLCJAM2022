#include "sun.hpp"
#include "stdio.h"
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

void Sun::Destroy(bool silent)
{
    
}

void Sun::Ability()
{
    printf("BZZZZZ\n");

}

b2Body* Sun::CreateBody(b2World* world)
{
    b2BodyDef def;
    def.gravityScale = 0;
    def.type = b2_dynamicBody;
    def.allowSleep = false;
    
    b2Body* body = world->CreateBody(&def);
    b2CircleShape shape;
    shape.m_radius = 3;
    b2FixtureDef fdef;
    fdef.filter.categoryBits = Constants::PC_PLAYERUNIT;
    fdef.filter.maskBits = Constants::PLAYERUNIT_COLLIDESWITH;
    fdef.shape = &shape;
    body->CreateFixture(&fdef);
    return body;
}
void Sun::OnCollisionEnter(b2Body*) {
    
}

void Sun::OnParticleColisionEnter(b2ParticleSystem* particleSystem,
                               b2ParticleBodyContact* particleBodyContact)
                               {
                                particleSystem->SetParticleFlags(particleBodyContact->index, b2ParticleFlag::b2_zombieParticle);
                                b2ParticleDef def;
                                
                                
                               }