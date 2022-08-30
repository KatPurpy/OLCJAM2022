#include "animal.hpp"
#include "stdio.h"
Animal::Animal()
{
    type = Constants::PC_ANIMAL;
}

void Animal::GetScared(float scare_x)
{
    direction = sgn(body->GetPosition().x - scare_x);;
    body->ApplyLinearImpulse({direction * 5 * body->GetMass(),6 * body->GetMass()},  body->GetPosition(), true);
}

void Animal::OnParticleColisionEnter(b2ParticleSystem* particleSystem,
                               b2ParticleBodyContact* particleBodyContact) 
{
    printf("yey worter.\n");
}

b2Body* Animal::CreateBody(b2World* world)
{
    b2PolygonShape shape;
    shape.SetAsBox(2, 1);

    b2BodyDef bdef;
    bdef.type = b2_dynamicBody;
    bdef.fixedRotation = true;
    auto body = world->CreateBody(&bdef);
    
    b2FixtureDef def;
    def.restitution = 0.25f;
    def.density = 1.2f;
    def.friction = 0.15f;
    def.shape = &shape;
    def.filter.categoryBits = Constants::PC_ANIMAL;
    def.filter.maskBits = Constants::ANIMAL_COLLIDESWITH;
    body->CreateFixture(&def);
    return body;            
}
#include "overlapcheck.hpp"
void Animal::Update()
{
    b2Vec2 f = {direction * 20,0};
    
    OverlapCheck<void*, Constants::PC_GROUND, true> check;
    void** discard = check.OverlapCircle(body->GetWorld(), body->GetWorldCenter(), 2);
    if(arrlen(discard) != 0)
    {
        body->ApplyForceToCenter(f * body->GetMass(), true);
    }
}
void Animal::Draw()
{

}

void Animal::OnCollisionEnter(b2Body* otherBody)
{

}
