#pragma once
#include "Box2D.h"
#include "physicsentity.hpp"
#include "constants.hpp"
#include "stb_ds.h"
#include "stdio.h"
template <class T, Constants::PhysicsCategory type, bool allowNullData> 
class OverlapCheck
{
    
        public:
        static inline T* cache = NULL; //prevent per frame allocations;
    template <typename shapeT>
    void GenericOverlap(b2World* world, b2BodyDef* def, shapeT* shape)
    {
        arrsetlen(cache, 0);
        b2FixtureDef fdef;
        fdef.isSensor = true;
        fdef.shape = shape;
        fdef.filter.categoryBits = 0xFFFF;
        fdef.filter.maskBits = type;
        auto body = world->CreateBody(def);
        body->CreateFixture(&fdef);

        world->Step(0, 0, 0, 0);


        for(b2ContactEdge* edge = body->GetContactList(); edge; edge = edge->next)
        {
            auto contact = edge->contact;
            if(contact->IsTouching())
            {
                void* dataA = contact->GetFixtureA()->GetBody()->GetUserData();
                void* dataB = contact->GetFixtureB()->GetBody()->GetUserData();
                //printf("%p %p\n", dataA, dataB);
                if(allowNullData || dataA)
                {
                    arrpush(cache, (T)dataA);
                }
            }
        }
        world->DestroyBody(body);
    }

    T* OverlapCircle(b2World* world, b2Vec2 vec, float radius)
    {
        b2BodyDef def;
        def.awake = true;
        def.type = b2_dynamicBody;
        def.position = vec;
        def.userData = (void*)0;

        b2CircleShape shape;
        shape.m_radius = radius;
        GenericOverlap(world, &def, &shape);
        return cache;
    }
};