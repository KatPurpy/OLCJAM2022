#pragma once
#include "stdio.h"
#include "BZZRE/subsystems/input.hpp"
#include "sun.hpp"
#include "cloud.hpp"
#include "terrain.hpp"
#include "animal.hpp"
#include "Box2D.h"
#include "imguidebugdraw.cpp"
#include "time.h"
#include "BZZRE/resources/image.hpp"
#include "gradient_fire.h"
using namespace BZZRE;
static const void* particleFireTag = (const void*)(0xBABEBABE);
struct SceneLevel
{
    static constexpr const float fireLifetime = 10;
    static constexpr const float fireSmokeThreshold = 7;
    static inline b2ParticleColor fireColorGradient[fire_gradient_width];
    
    
    class _contactListener : public b2ContactListener
    {
        /// Called when a fixture and particle start touching if the
        /// b2_fixtureContactFilterParticle flag is set on the particle.
        void BeginContact(b2ParticleSystem* particleSystem,
                                b2ParticleBodyContact* particleBodyContact)
        {
            auto bodyData = (PhysicsEntity*)particleBodyContact->body->GetUserData();
            if(bodyData)
            {
                bodyData->OnParticleColisionEnter(particleSystem, particleBodyContact);
            }
        }

        /// Called when a fixture and particle stop touching if the
        /// b2_fixtureContactFilterParticle flag is set on the particle.
        void EndContact(b2Fixture* fixture,
                                b2ParticleSystem* particleSystem, int32 index)
        {
            B2_NOT_USED(fixture);
            B2_NOT_USED(particleSystem);
            B2_NOT_USED(index);
        }
        
    };

    class _presolver : public b2ContactFilter
    {
        public:
	     bool ShouldCollide(b2ParticleSystem* particleSystem,
							   int32 particleIndexA, int32 particleIndexB)
                               {
                                     auto buffer = particleSystem->GetUserDataBuffer();
                                     auto userdataA = buffer[particleIndexA];
                                     auto userdataB = buffer[particleIndexB];
                                     return !(userdataA == particleFireTag && userdataB == particleFireTag);
                               }
    };

    static inline b2World* world;
    static inline Terrain* terrain;
    static inline FooDraw* debugDraw;
    static inline _presolver* particleFilter;
    static inline _contactListener* contactListener;

    static inline b2ParticleSystem* system;
    static inline b2ParticleGroup* pwaterGroup;

    static inline b2ParticleSystem* sfireSystem;
    static inline b2ParticleGroup* pfireGroup;

    static void IterateEntities(void (*iterate)(PhysicsEntity*))
    {
        b2Body* node = world->GetBodyList();
        while (node)
        {
            b2Body* b = node;
            node = node->GetNext();
            PhysicsEntity* myActor = (PhysicsEntity*)b->GetUserData();
            if (myActor){
                if(myActor->dead)
                {
                    delete myActor;
                } else {
                    iterate(myActor);
                }
            }
        }
    }

    static void PutWater(b2Vec2 position, b2Shape* shape)
    {        
        b2ParticleSystemDef psystemdef;
        b2ParticleGroupDef pgroupdef;
        pgroupdef.position = position;
        pgroupdef.shape = shape;
        pgroupdef.group = pwaterGroup;

        system->CreateParticleGroup(pgroupdef);
        int numparticles = system->GetParticleCount();
        auto colors = system->GetColorBuffer();
        for(int i = 0; i < numparticles; i++)
        {
            colors[i] = {0,0,255,255};
            system->SetParticleFlags(i,  b2ParticleFlag::b2_fixtureContactListenerParticle);
        }
    }

    static void Enter()
    {
        {
            char* data = fire_gradient_data;
            for(int i = 0; i < fire_gradient_width; i++)
            {
                uint8_t pixel[3];
                HEADER_PIXEL(data, pixel);
                int index = fire_gradient_width - i - 1;
                assert(index >= 0);
                fireColorGradient[index] = b2ParticleColor(pixel[0],pixel[1],pixel[2], (index >> 2) << 2);
            }
        }
            // Define the gravity vector.
        b2Vec2 gravity(0.0f, -10.0f);

        // Construct a world object, which will hold and simulate the rigid bodies.
        world = new b2World(gravity);
        world->SetDebugDraw(debugDraw = new FooDraw());
        debugDraw->SetFlags(b2Draw::e_shapeBit | b2Draw::e_particleBit);
        
        world->SetContactFilter(particleFilter = new _presolver());
        world->SetContactListener(contactListener = new _contactListener());

        Camera::x = 0;
        Camera::y = 0;
        Camera::screen_margin_x = 0.35f;
        Camera::screen_margin_y = 0.35f;
        Camera::speed = 1;
        Camera::ppm = 10;

        

        PhysicsEntity* e = (new Sun());
        e->Instantiate(world);
        ((Sun*)e)->SetPosition({20,20});

        /*cloud.Instantiate(world);*/
        terrain = new Terrain();
        terrain->Initialize(world, 0, 0, 100, 128, 128, 32);
        terrain->Generate(world, [](int,float x){return (float)(((int)(x/32)) * 1);});
        
        e = new Animal();
        e->Instantiate(world);
        e->body->SetTransform({60, 10}, 0);

        b2ParticleSystemDef psystemdef;
        psystemdef.radius = 0.75f;
        system = world->CreateParticleSystem(&psystemdef);
        
        psystemdef.gravityScale = -0;
        psystemdef.colorMixingStrength = 0.5f;
        psystemdef.maxCount = 2048;
        sfireSystem = world->CreateParticleSystem(&psystemdef);

        b2ParticleGroupDef groupdef;
        groupdef.groupFlags = b2ParticleGroupFlag::b2_particleGroupCanBeEmpty;
        
        pwaterGroup = system->CreateParticleGroup(groupdef);
        
        pfireGroup = sfireSystem->CreateParticleGroup(groupdef);

        b2CircleShape shape;
        shape.m_radius = 5;



        srand(time(NULL));   // Initialization, should only be called once.
        
        

       // PutWater({50,20}, &shape);
    }
    static void MakeFireParticle(b2Vec2 pos)
    {
        b2ParticleDef def;
        float horizontal_speed = 6;
        def.lifetime = ((float)(rand() % 4)) / 4 * 4 + 6;
        def.velocity = {((float)(rand() % 25)) / 25 * 8 - horizontal_speed/2, ((float)(rand() % 100)) / 100 * 5 + 5};
        def.position = pos;
        def.group = pfireGroup;
        def.userData = (void*)particleFireTag;
        def.flags = b2ParticleFlag::b2_particleContactFilterParticle;
        sfireSystem->CreateParticle(def);
        /*b2ParticleGroupDef groupdef;
        groupdef.position = pos;
        b2Vec2 aaa[1] = {{0,0}};
        groupdef.positionData = aaa;
        groupdef.particleCount = 1;
        groupdef.group = pfireGroup;
        
        groupdef.lifetime = ((float)(rand() % 1000)) / 1000 * 5 + 5;
        groupdef.linearVelocity = {((float)(rand() % 25)) / 25 * 5 - 2.5f, ((float)(rand() % 100)) / 100 * 5 + 5};
*/
        //sfireSystem->CreateParticleGroup(groupdef);
        
    }
    static void Update()
    {
                world->Step(1.f/60.0f, 6, 2, 1);
        if(Input::MouseDown(SAPP_MOUSEBUTTON_RIGHT))
        {
            float mx, my;
            Input::MousePos(&mx, &my);
            MakeFireParticle(Camera::ScreenToBox2D({mx,my}));
        }
    if(Input::MouseClick(SAPP_MOUSEBUTTON_MIDDLE))
      {
        float aaa[20] = {5};
        for(int i = 0; i < 20; i++) aaa[i] = -5;

        float mx, my;
        Input::MousePos(&mx, &my);
        static float start_x;
        start_x = Camera::ScreenToBox2D({mx,0}).x;
        //terrain.WriteHeightMapOffset(aaa, 20, Camera::ScreenToBox2D({mx,0}).x);

        static float pit_length;
        pit_length = terrain->GetDistance(20);
        
        static int pit_width_verts = 20;
        
        pit_width_verts = terrain->GetVertsForDistance(pit_length);

        const int pit_depth = 10;

       
        auto func = [](int i,float f) -> float
        {
            return sin( ((f) / pit_length) * M_PI ) * -pit_depth;  
        };
        terrain->WriteHeightmapProceduralOffset(func, pit_width_verts, start_x - pit_length/2);
        terrain->RegenerateChunks();
        
        b2CircleShape shape;
        shape.m_radius = pit_depth/2;
        PutWater({start_x,terrain->GetHeightAt(start_x) + shape.m_radius*1.25f }, &shape);
      }
        IterateEntities([](auto pe) {pe->Update();});

        auto colors = sfireSystem->GetColorBuffer();
        for(int i = 0; i < sfireSystem->GetParticleCount(); i++)
        {
            float time = sfireSystem->GetParticleLifetime(i);
            int index = time/fireLifetime * (float)fire_gradient_width ;
            if(index < 0) index = 0;
            if(index > fire_gradient_width) index = fire_gradient_width - 1;
            colors[i] = fireColorGradient[index];
        }


    }

    static void Draw()
    {
        float mx, my;
        Input::MousePos(&mx, &my);
        Camera::MouseMovement(mx/sapp_widthf(),my/sapp_heightf(), Input::MouseScroll() );
        IterateEntities([](auto pe) {pe->Draw();});
        world->DrawDebugData();
    }

    static void Leave()
    {
        delete terrain;
        delete world;
        delete debugDraw;
        delete contactListener;
        delete particleFilter;
    }
} ;