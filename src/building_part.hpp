#pragma once
#include "HandmadeMath.h"
#include "physicsentity.hpp"

void RegisterBuildingDestruction();
void DrawParamToScreen(BZZRE::Graphics::SpriteDrawParams& params);
struct BuildingPartDef
{
    int* buildingCountPtr;
    BZZRE::Graphics::SpriteDrawParams sprite;
    b2Transform transform;
    hmm_v2 size;
};

class BuildingPart : public PhysicsEntity
{
    protected:

    public:
        int* partCounter;
        hmm_v2 boxSize;
        b2Transform transform;
        BZZRE::Graphics::SpriteDrawParams sprite;
        BuildingPart(BuildingPartDef& def)
        {
            assert(def.buildingCountPtr);
            partCounter = def.buildingCountPtr;
            boxSize = def.size;
            transform = def.transform;
            sprite = def.sprite;
            burnData.health = rand()%5 / 5. * 3 + 5;
            canBeSetOnFire = true;
            (*partCounter)++;
        }

        b2Body* CreateBody(b2World* world)
        {
            b2PolygonShape shape;
            shape.SetAsBox(boxSize.Width, boxSize.Height);

            b2FixtureDef fdef;
            fdef.shape = &shape; 
            fdef.filter.categoryBits = Constants::PC_BUILDING;
            fdef.filter.maskBits = Constants::BUILDING_COLLIDESSWITH;
            fdef.density = 100;
            b2BodyDef bdef;
            bdef.position = transform.p;
            bdef.angle = transform.q.GetAngle();
            bdef.type = b2BodyType::b2_dynamicBody;
            
            auto body = world->CreateBody(&bdef);
            body->CreateFixture(&fdef);
            return body;
        }

        void Draw()
        {
            auto spr = sprite;
            auto pos = Camera::Box2DToScreen(body->GetPosition());
            auto inf = sg_query_image_info(spr.image);
            spr.xywh = {pos.X, pos.Y,
            boxSize.Width * 2, boxSize.Height * 2};
            spr.xywh.ZW *= Camera::ppm;
            spr.r = -body->GetAngle();
            spr.origin = {inf.width/2.f,inf.height/2.f};
            //spr.origin = {boxSize.Width * Camera::ppm, boxSize.Height * Camera::ppm };
            //spr.origin *= Camera::ppm;
            AddSprite(spr);

            //TODO: MAKE THIS SHIT DRAW
        }

        void Kill(bool silent) override
        {
            assert(partCounter);
            (*partCounter)--;
            printf("%i\n",*partCounter);
            if(!(*partCounter))
            {
                RegisterBuildingDestruction();
            }
            dead = true;
            //PhysicsEntity::Kill(silent);
        }
};