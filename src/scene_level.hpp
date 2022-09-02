#pragma once
#include "stdio.h"
#include "BZZRE/subsystems/input.hpp"
#include "constants.hpp"

#include "sun.hpp"
#include "cloud.hpp"
#include "terrain.hpp"
#include "animal.hpp"
#include "Box2D.h"
#include "imguidebugdraw.cpp"
#include "time.h"
#include "BZZRE/resources/image.hpp"
#include "gradient_fire.h"
#include "leveldata.hpp"
#include "animalsafezone.hpp"
#include "building_part.hpp"

using namespace BZZRE;

class _contactListener : public b2ContactListener
{
	bool
	isParticle(void* data)
	{
		return 0 != data && data < Constants::maxParticleTag;
	}
	void
	BeginContact(b2Contact* contact)
	{
		if(contact->IsTouching())
		{
			auto userDataA = (PhysicsEntity*)contact->GetFixtureA()->GetBody()->GetUserData();
			auto userDataB = (PhysicsEntity*)contact->GetFixtureB()->GetBody()->GetUserData();

			if(userDataA && userDataB && !isParticle(userDataA) && !isParticle(userDataB))
			{
				printf("%p %p\n", userDataA, userDataB);
				userDataA->OnCollisionEnter(userDataB);
				userDataB->OnCollisionEnter(userDataA);
			}
		}
		// B2_NOT_USED(contact);
	}

	/// Called when a fixture and particle start touching if the
	/// b2_fixtureContactFilterParticle flag is set on the particle.
	void
	BeginContact(b2ParticleSystem* particleSystem, b2ParticleBodyContact* particleBodyContact)
	{
		auto bodyData = (PhysicsEntity*)particleBodyContact->body->GetUserData();
		if(bodyData)
		{
			bodyData->OnParticleColisionEnter(particleSystem, particleBodyContact);
		}
	}

	/// Called when a fixture and particle stop touching if the
	/// b2_fixtureContactFilterParticle flag is set on the particle.
	void
	EndContact(b2Fixture* fixture, b2ParticleSystem* particleSystem, int32 index)
	{
		B2_NOT_USED(fixture);
		B2_NOT_USED(particleSystem);
		B2_NOT_USED(index);
	}
};

class _presolver : public b2ContactFilter
{
  public:
	bool
	ShouldCollide(b2ParticleSystem* particleSystem, int32 particleIndexA, int32 particleIndexB)
	{
		auto buffer = particleSystem->GetUserDataBuffer();
		auto userdataA = buffer[particleIndexA];
		auto userdataB = buffer[particleIndexB];
		return !(userdataA == Constants::particleFireTag && userdataB == Constants::particleFireTag);
	}
	virtual bool
	ShouldCollide(b2Fixture* fixture, b2ParticleSystem* particleSystem, int32 particleIndex)
	{
		PhysicsEntity* ent = (PhysicsEntity*)fixture->GetBody()->GetUserData();
		if(ent)
		{
			if(ent->type == Constants::PC_CLOUD)
			{
				if(particleSystem->GetUserDataBuffer()[particleIndex] == Constants::particleWaterTag)
				{
					return false;
				}
			}

			if(particleSystem->GetUserDataBuffer()[particleIndex] == Constants::particleFireTag
			   && (ent->type & Constants::PC_BURNING))
			{
				return false;
			}
		}
		return true;
	}
};

static b2World* world;
static Terrain* terrain;

static FooDraw debugDraw;
static _presolver particleFilter;
static _contactListener contactListener;

static b2ParticleSystem* sWaterSystem;
static b2ParticleGroup* pwaterGroup;

static b2ParticleSystem* sfireSystem;
static b2ParticleGroup* pfireGroup;

static b2ParticleSystem* sSmokeSystem;
static b2ParticleGroup* pSmokeGroup;

static int GameVar_Animals;
static int GameVar_AnimalsSaved;
static int GameVar_BuildingsLeft;
static int GameVar_BuildingPartCount[Constants::MaxBuildingParts];

static b2ParticleColor fireColorGradient[fire_gradient_width];

BZZRE::Image img("logotip.qoi");
void UpdateFireColor()
{
	auto colors = sfireSystem->GetColorBuffer();
	for(int i = 0; i < sfireSystem->GetParticleCount(); i++)
	{
		float time = sfireSystem->GetParticleLifetime(i);
		int index = time / Constants::fireLifetime * (float)fire_gradient_width;
		if(index < 0)
			index = 0;
		if(index > fire_gradient_width)
			index = fire_gradient_width - 1;
		colors[i] = fireColorGradient[index];
	}
}

void RegisterBuildingDestruction()
{
    GameVar_BuildingsLeft--;
    printf("BUILDING GONE\n");
}

void
MakeFireParticle(b2Vec2 pos)
{
	b2ParticleDef def;
	float horizontal_speed = 6;
	def.lifetime = ((float)(rand() % 4)) / 4 * 4 + 6;
	def.velocity = { ((float)(rand() % 25)) / 25 * 8 - horizontal_speed / 2, ((float)(rand() % 100)) / 100 * 5 + 5 };
	def.position = pos;
	def.group = pfireGroup;
	def.userData = (void*)Constants::particleFireTag;
	def.flags = b2ParticleFlag::b2_particleContactFilterParticle | b2ParticleFlag::b2_fixtureContactFilterParticle
				| b2ParticleFlag::b2_fixtureContactListenerParticle;
	sfireSystem->CreateParticle(def);
}

#define allocThing(type, name, ...)                                                                                    \
	if(allocate)                                                                                                       \
		name = new type(__VA_ARGS__);                                                                                  \
	else                                                                                                               \
		delete name;
static void
AllocateWorld(bool allocate)
{
	if(!allocate)
	{
		b2Body* node = world->GetBodyList();
		while(node)
		{
			b2Body* b = node;
			node = node->GetNext();
			PhysicsEntity* myActor = (PhysicsEntity*)b->GetUserData();
			if(myActor)
			{
				delete myActor;
			}
		}
	}
	allocThing(b2World, world, { 0, -10 });
	
	allocThing(Terrain, terrain);

}
#undef allocThing

void
PutWater(b2Vec2 position, b2Shape* shape, float stride = 0)
{
	b2ParticleSystemDef psystemdef;
	b2ParticleGroupDef pgroupdef;
	pgroupdef.position = position;
	pgroupdef.shape = shape;
	pgroupdef.group = pwaterGroup;
	pgroupdef.userData = Constants::particleWaterTag;
	pgroupdef.color = { 0, 0, 255, 255 };
	pgroupdef.stride = stride;
	pgroupdef.flags
		= b2ParticleFlag::b2_fixtureContactListenerParticle | b2ParticleFlag::b2_fixtureContactFilterParticle;
	sWaterSystem->CreateParticleGroup(pgroupdef);
}

void
WaterToSmoke(b2Vec2 pos, float radius)
{
	b2CircleShape shape;
	shape.m_radius = radius;
	auto aaa = b2Transform(pos, b2Rot(0));
	int num = sWaterSystem->DestroyParticlesInShape(shape, aaa);

	if(num)
	{
		b2ParticleGroupDef def;
		def.userData = Constants::particleSmokeTag;
		shape.m_radius = HMM_SquareRootF(sSmokeSystem->GetRadius() * num / 3);
		def.flags = b2ParticleFlag::b2_fixtureContactListenerParticle;
		def.shape = &shape;
		def.position = pos;
		def.color = { 255, 255, 255, 64 };
		def.group = pSmokeGroup;
		def.linearVelocity = { 0, 5 };

		sSmokeSystem->CreateParticleGroup(def);
	}
}

static int NumAnimals;
void
SaveAnimal(Animal* animal)
{
    GameVar_AnimalsSaved++;
	animal->Kill(true);
}

static float RebootTime;

struct SceneLevel
{
	static void
	IterateEntities(void (*iterate)(PhysicsEntity*))
	{
        {
            b2Body* node = world->GetBodyList();
            while(node)
            {
                b2Body* b = node;
                node = node->GetNext();
                PhysicsEntity* myActor = (PhysicsEntity*)b->GetUserData();
                if(myActor)
                {
                    {
                        iterate(myActor);
                    }
                }
            }
        }
        
        {
            b2Body* node = world->GetBodyList();
            while(node)
            {
                b2Body* b = node;
                node = node->GetNext();
                PhysicsEntity* myActor = (PhysicsEntity*)b->GetUserData();
                if(myActor)
                {
                    if(myActor->dead)
                    {
                        delete myActor;
                    }
                }
            }
        }
	}

	static void
	Enter()
	{
		AllocateWorld(true);

		{
			char* data = fire_gradient_data;
			for(int i = 0; i < fire_gradient_width; i++)
			{
				uint8_t pixel[3];
				HEADER_PIXEL(data, pixel);
				int index = fire_gradient_width - i - 1;
				assert(index >= 0);
				fireColorGradient[index] = b2ParticleColor(pixel[0], pixel[1], pixel[2], (index >> 2) << 2);
			}
		}
		// Define the gravity vector.
		b2Vec2 gravity(0.0f, -10.0f);

		// Construct a world object, which will hold and simulate the rigid bodies.
		world = new b2World(gravity);
		world->SetDebugDraw(&debugDraw);
		world->SetContactFilter(&particleFilter);
		world->SetContactListener(&contactListener);

		debugDraw.SetFlags(b2Draw::e_shapeBit | b2Draw::e_particleBit);

		Camera::x = 0;
		Camera::y = 0;
		Camera::screen_margin_x = 0.35f;
		Camera::screen_margin_y = 0.35f;
		Camera::speed = 1;
		Camera::ppm = 10;
        Camera::bounds = {0,128,0,50};
        //TODO: ADD REAL TERRAIN RENDERING!!!
        //TODO: WIN CONDITION
        //TODO: LOSE CONDITION
        //TODO: ADD SPRITES FOR SUN, CLOUD, ANIMALS
        //TODO: LEVEL GENERATION
        //TODO: ADD MAIN MENU
        //TODO: ADD DAY NIGHT VISUAL CYCLE
        //TODO: ADD BLOOM
		
		/*PhysicsEntity* e = (new Sun());
		e->Instantiate(world);
		((Sun*)e)->SetPosition({ 20, 20 });
		e = new Animal();
		e->Instantiate(world);
		e->body->SetTransform({ 60, 10 }, 0);
*/

		/*cloud.Instantiate(world);*/
		/*terrain = new Terrain();
		terrain->Initialize(world, 0, 0, 100, 128, 128, 32);
		terrain->Generate(world, [](int, float x) { return (float)(((int)(x / 32)) * 1); });
*/

		b2ParticleSystemDef psystemdef;
		psystemdef.radius = 0.75f;
		sWaterSystem = world->CreateParticleSystem(&psystemdef);

		psystemdef.gravityScale = -0;
		psystemdef.colorMixingStrength = 0.5f;
		psystemdef.maxCount = 2048;
		sfireSystem = world->CreateParticleSystem(&psystemdef);
		psystemdef.gravityScale = -0.01f;
		sSmokeSystem = world->CreateParticleSystem(&psystemdef);

		b2ParticleGroupDef groupdef;
		groupdef.groupFlags = b2ParticleGroupFlag::b2_particleGroupCanBeEmpty;

		pwaterGroup = sWaterSystem->CreateParticleGroup(groupdef);

		pfireGroup = sfireSystem->CreateParticleGroup(groupdef);

		pSmokeGroup = sSmokeSystem->CreateParticleGroup(groupdef);
		b2CircleShape shape;
		shape.m_radius = 5;

		

		/*auto asz = new AnimalSafeZone();
		asz->Instantiate(world);
		asz->body->SetTransform({ 0, 0 }, 0);

		asz = new AnimalSafeZone();
		asz->Instantiate(world);
		asz->body->SetTransform({ 128, 0 }, 0);
*/
/*{

        BuildingPartDef bpdef;
        bpdef.sprite.color = {255,255,255,255};
        bpdef.sprite.image = *img.Get();
        bpdef.size = {5,5};
        bpdef.transform = b2Transform({20,20}, b2Rot());
        GameVar_BuildingPartCount[0] = 0;
        bpdef.buildingCountPtr = &GameVar_BuildingPartCount[0];
        static BuildingPart* part;
        part = new BuildingPart(bpdef);
        part->Instantiate(world);
        printf("PARTS FOR BUILDING 0 %i\n",GameVar_BuildingPartCount[0]);
}*/
		// PutWater({50,20}, &shape);
	}
	static void
	PutPit(float x, float width, float depth, bool water)
	{
		static float start_x;
		static float pit_length;
		static int pit_width_verts;
		static float pit_depth;
		start_x = x;
		pit_length = terrain->GetDistance(width);
		pit_width_verts = terrain->GetVertsForDistance(pit_length);
		pit_depth = depth;

		auto func = [](int i, float f) -> float { return sin(((f) / pit_length) * M_PI) * -pit_depth; };
		terrain->WriteHeightmapProceduralOffset(func, pit_width_verts, start_x - pit_length / 2);

		if(water)
		{
			b2CircleShape shape;
			shape.m_radius = pit_depth / 2;
			PutWater({ start_x, terrain->GetHeightAt(start_x) + shape.m_radius * 1.25f }, &shape);
		}
		terrain->RegenerateChunks();
	}
	static void
	Update()
	{

		if(Input::IsKeyDown(SAPP_KEYCODE_R))
		{
			StateManagement::SwitchState<SceneLevel>();
		}
		world->Step(1.f / 60.0f, 6, 2, 1);
		IterateEntities([](auto pe) { pe->Update(); });
		UpdateFireColor();
	}

	static void
	Draw()
	{
		float mx, my;
		Input::MousePos(&mx, &my);
		Camera::MouseMovement(mx / sapp_widthf(), my / sapp_heightf(), Input::MouseScroll());
		IterateEntities([](auto pe) { pe->Draw(); });
		world->DrawDebugData();
	}

	static void
	Leave()
	{
		AllocateWorld(false);
	}
};