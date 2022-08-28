#include "animal.hpp"
Animal::Animal()
{
    type = Constants::PC_ANIMAL;
}

void Animal::GetScared(float scare_x)
{
    direction = sgn(body->GetPosition().x - scare_x);;
    body->ApplyLinearImpulse({direction * 80,100},  body->GetPosition(), true);
}

b2Body* Animal::CreateBody(b2World* world)
{
    b2PolygonShape shape;
    shape.SetAsBox(2, 1);

    b2BodyDef bdef;
    bdef.type = b2_dynamicBody;
    
    auto body = world->CreateBody(&bdef);

    b2FixtureDef def;
    def.restitution = 0.25f;
    def.density = 2;
    def.friction = 0.25f;
    def.shape = &shape;
    def.filter.categoryBits = Constants::PC_ANIMAL;
    def.filter.maskBits = Constants::ANIMAL_COLLIDESWITH;
    body->CreateFixture(&def);
    return body;            
}

void Animal::Update()
{
    b2Vec2 f = {direction * 150,0};
    body->ApplyForceToCenter(f, true);
}

void Animal::Draw()
{

}

void Animal::OnCollisionEnter(b2Body* otherBody)
{

}
