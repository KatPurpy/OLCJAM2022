#include "physicsentity.hpp"
#include "overlapcheck.hpp"

void SaveAnimal(Animal*);

class AnimalSafeZone : public PhysicsEntity
{
    b2Body* CreateBody(b2World* world)
    {
        b2PolygonShape shape;
        shape.SetAsBox(2, 100);
        
        b2BodyDef def;
        def.type = b2_staticBody;

        auto body = world->CreateBody(&def);
        
        b2FixtureDef fdef;
        fdef.isSensor = true;
        fdef.shape = &shape;
        body->CreateFixture(&fdef);
        return body;
    }
    void Update()
    {
        
        OverlapCheck<Animal*, Constants::PC_ANIMAL, false> check;
        
        b2PolygonShape shape;
        shape.SetAsBox(2, 100);
        
        b2BodyDef def;
        def.type = b2_staticBody;
        def.position = body->GetPosition();

        check.GenericOverlap(body->GetWorld(), &def, &shape);
        
        Animal** animalList = OverlapCheck<Animal*, Constants::PC_ANIMAL, false> ::cache;
        if(arrlen(animalList))
        {
            for(int i = 0; i < arrlen(animalList); i++)
            {
                SaveAnimal(animalList[i]);
            }
        }
    }
    void Draw()
    {

    }
    void OnCollisionEnter(b2Body* otherBody)
    {
        //printf("IT WORKS\n");
    }
};