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
#include "stdlib.h"

#include "BZZRE/subsystems/audio.hpp"
#include "soloud_wavstream.h"
#include "scene_win.hpp"
#include "soloud_wav.h"
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

BZZRE::Image background_sky("sky.jpg");

static struct 
{
	int Animals;
	int AnimalsDied;
	int AnimalsSaved;
	int BuildingsLeft;
	int BuildingPartCount[512];
	int NumAnimals;
} GameVar;
static b2ParticleColor fireColorGradient[fire_gradient_width];

BZZRE::Image img("logotip.qoi");
SoLoud::WavStream screams;
SoLoud::handle screamHandle = 0; 
SoLoud::WavStream fireNoise;
SoLoud::handle fireNoiseHandle = 0; 
void UpdateFireColor()
{
	if(!BZZRE::Audio::GetSoloud()->isValidVoiceHandle(fireNoiseHandle) ||
	!BZZRE::Audio::GetSoloud()->isValidVoiceHandle(screamHandle))
	{
		fireNoise.load("assets/party.ogg");
		BZZRE::Audio::GetSoloud()->setLooping(fireNoiseHandle = BZZRE::Audio::GetSoloud()->play(fireNoise), true);

		;
		printf("%s\n", BZZRE::Audio::GetSoloud()->getErrorString(screams.load("assets/happycrowdnoises.ogg")));
		BZZRE::Audio::GetSoloud()->setLooping(screamHandle = BZZRE::Audio::GetSoloud()->play(screams), true);
	}
	BZZRE::Audio::GetSoloud()->setVolume(fireNoiseHandle, sfireSystem->GetParticleCount() / 800. );
	BZZRE::Audio::GetSoloud()->setVolume(screamHandle, sfireSystem->GetParticleCount() / 800. );
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
static SoLoud::Wav building_explosions[4];
void RegisterBuildingDestruction()
{
    GameVar.BuildingsLeft--;
    printf("BUILDING GONE\n");
	float pitch = rand()/65535. * 0.5f + 0.85f;
	auto handle = BZZRE::Audio::GetSoloud()->play(building_explosions[rand()%4]);
	BZZRE::Audio::GetSoloud()->setRelativePlaySpeed(handle,pitch);
}
static SoLoud::Wav deerDeaths[4];
void KillAnimal(Animal* animal)
{
	GameVar.AnimalsDied++;
	float pitch = rand()/65535. * 1.5f + 0.85f;
	auto handle = BZZRE::Audio::GetSoloud()->play(deerDeaths[rand()%4], 0.5f);
	BZZRE::Audio::GetSoloud()->setRelativePlaySpeed(handle,pitch);
	animal->Kill(true);
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
		if(allocate){name = new type(__VA_ARGS__);}else {delete name;}

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
	if(allocate)
	{
		world->SetDebugDraw(&debugDraw);
		world->SetContactFilter(&particleFilter);
		world->SetContactListener(&contactListener);

		debugDraw.SetFlags(b2Draw::e_shapeBit | b2Draw::e_particleBit);
	}
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

 	void PutPit(float x, float width, float depth, bool water)
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

		//sSmokeSystem->CreateParticleGroup(def);
	}
}


void
SaveAnimal(Animal* animal)
{
    GameVar.AnimalsSaved++;
	animal->Kill(true);
}

BZZRE::Image BuildingSprites[] =
{
	BZZRE::Image("skycrapper.jpg"),
	BZZRE::Image("propaganda1.jpg"),
	BZZRE::Image("propaganda2.jpg"),
	BZZRE::Image("propaganda3.jpg"),
};

static float RebootTime;
static int current_BuildingID = 0;
static b2Vec2 current_Building_offset;
#define COMMAND_BEGIN(cmdname, argc) if (strcmp(cmdname,command) == 0) { if(argc != arrlen(args)) { printf("COMMAND \"%s\" ON LINE %i. EXPECTED %i. GOT %i.\n",command,linenum, argc, arrlen(args)); abort(); }
#define COMMAND_END() return; }
static void InterpretComand(const char* command, float* args, int linenum)
{

	COMMAND_BEGIN("terrain", 5)
		float length = args[0];
		float depth = args[1];
		int verts = args[2];
		int verts_per_chunk = args[3];	
		int seed = args[4];
		
		terrain = new Terrain();
		terrain->Initialize(world, 0, 0, length, depth, verts, verts_per_chunk);

		terrain->Generate(world, [](int, float x) { return (float)(((int)(x / 32)) * 1); });
		
		auto asz = new AnimalSafeZone();
		asz->Instantiate(world);
		asz->body->SetTransform({ 0, 0 }, 0);

		asz = new AnimalSafeZone();
		asz->Instantiate(world);
		asz->body->SetTransform({ length, 0 }, 0);
		

		Camera::screen_margin_x = 0.35f;
		Camera::screen_margin_y = 0.35f;
		Camera::speed = 1;
		Camera::ppm = 10-4;
        Camera::bounds = {0,length,-depth,50};
	COMMAND_END()

	COMMAND_BEGIN("pit", 4)
		float x = args[0];
		float width = args[1];
		float depth = args[2];
		float b = args[3];
		PutPit(x,width,depth,b);
	COMMAND_END()

	COMMAND_BEGIN("regen", 0)
		terrain->RegenerateChunks();
	COMMAND_END()

	COMMAND_BEGIN("b_begin", 1)
	current_BuildingID = GameVar.BuildingsLeft;
	GameVar.BuildingsLeft++;
	float x = args[0];
	current_Building_offset = { x, terrain->GetHeightAt(x) + 1 };
	COMMAND_END()

	COMMAND_BEGIN("b_part", 6)
		assert(current_BuildingID != -1);
		int sprite = args[0];
		float x = args[1];
		float y = args[2];
		float w = args[3];
		float h = args[4];
		float r = args[5];

		BuildingPartDef def;
		def.sprite.image = *BuildingSprites[sprite].Get();
		def.buildingCountPtr = &GameVar.BuildingPartCount[current_BuildingID];
		def.size = {w,h};
		def.sprite.color = {255,255,255,255};
		def.transform.Set(b2Vec2(x,y) + current_Building_offset,r);

		BuildingPart* part = new BuildingPart(def);
		part->Instantiate(world);
	COMMAND_END()

	COMMAND_BEGIN("b_end", 0)
	current_BuildingID = -1;
	COMMAND_END()

	COMMAND_BEGIN("animal", 1)
		GameVar.Animals++;
		float x = args[0];
		auto e = new Animal();
		e->Instantiate(world);
		b2Vec2 pos = { x, terrain->GetHeightAt(x) + 1 };
		e->body->SetTransform(pos, 0);
	COMMAND_END()
	
	COMMAND_BEGIN("sun", 2)
		Sun* e = (new Sun());
		e->Instantiate(world);
		e->SetPosition({ args[0], args[1] });
		e->targetpos = {args[0], args[1]};
		Camera::x = args[0];
		Camera::y = args[1];
	COMMAND_END()
}

void ParseCommand(const char* line, int linenum, int* terminate)
{
	char* command = nullptr;
	char* currptr = (char*)(void*)line;
	float* args = nullptr;


	while (!isspace(*currptr))
	{
		arrpush(command, *currptr);
		currptr++;
	}
	arrpush(command, 0);
	bool terminateReadArgs = 0;
	while(*currptr)
	{
		while(*currptr && !isdigit(*currptr)) currptr++;
		if(*currptr) arrpush(args, (float)atof(currptr));
		while(*currptr && !isspace(*currptr)) currptr++;
	}

	assert(*currptr == 0);	

	printf("%s", command);
	for(int i = 0; i < arrlen(args); i++)
	{
		printf(" %f", args[i]);
	}
	printf("\n");
	if(strcmp("end", command) == 0) *terminate = true;
	else InterpretComand(command, args, linenum);
	
	arrfree(command);
	arrfree(args);
}
static bool InitStream = false;
static SoLoud::WavStream music_rampage;

struct SceneLevel
{
	static inline const char* LevelPath = NULL;
	static inline bool LevelEditor = false;
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
	
	static void CreateParticleSystems()
	{
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
	}

	static void LoadFireGradient()
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

	static void
	Enter()
	{
		if(!InitStream)
		{
			building_explosions[0].load("assets/boom1.ogg");
			building_explosions[1].load("assets/boom2.ogg");
			building_explosions[2].load("assets/boom3.ogg");
			building_explosions[3].load("assets/boom4.ogg");
			
			deerDeaths[0].load("assets/happydeernoise1.ogg");
			deerDeaths[1].load("assets/happydeernoise2.ogg");
			deerDeaths[2].load("assets/happydeernoise3.ogg");
			deerDeaths[3].load("assets/happydeernoise4.ogg");
			
			InitStream = true;
			music_rampage.load("assets/deercost.ogg");
			auto a =BZZRE::Audio::GetSoloud()->play(music_rampage, 0.75f);
			BZZRE::Audio::GetSoloud()->setLooping(a, true);
		}
		memset(&GameVar, 0, sizeof(GameVar));
		AllocateWorld(true);
		CreateParticleSystems();
		LoadFireGradient();
		// PutWater({50,20}, &shape);
		

{
		memset(LEVELDATA, 0, sizeof(LEVELDATA));
		FILE* filePointer = fopen(LevelPath, "r");;
		fseek(filePointer, 0, SEEK_END);
		long fsize = ftell(filePointer);
		rewind(filePointer);
		fread(LEVELDATA, fsize, 1, filePointer);
		rewind(filePointer);		

		int bufferLength = 255;
		char buffer[bufferLength]; /* not ISO 90 compatible */
		int linenum = 0;

		while(fgets(buffer, bufferLength, filePointer)) {
			auto castedbuf = (const char*)((void*)buffer);
			linenum++;
			int terminate = 0;
			ParseCommand(castedbuf, linenum, &terminate);
			if(terminate)
			{
				break;
			}
		}


		fclose(filePointer);
}
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


		terrain = new Terrain();
		terrain->Initialize(world, 0, 0, 100, 128, 128, 32);
		terrain->Generate(world, [](int, float x) { return (float)(((int)(x / 32)) * 1); });
		*/
	}
static inline char LEVELDATA[1<<16];

	static void
	Update()
	{


		world->Step(1.f / 60.0f, 6, 2, 1);
		IterateEntities([](auto pe) { pe->Update(); });
		UpdateFireColor();
		
		if(LevelEditor)
		{
			ImGui::Begin("EDITOR");
			if(ImGui::Button("Reload"))
			{
				StateManagement::SwitchState<SceneLevel>();
				auto fp = fopen(LevelPath, "w");
				fputs(LEVELDATA, fp);
				fclose(fp);
			} else {
				ImGui::InputTextMultiline("LEVEL DATA", LEVELDATA, sizeof(LEVELDATA), {400,400});
			}
			ImGui::End();
		}else{
			if(GameVar.Animals == (GameVar.AnimalsDied + GameVar.AnimalsSaved))
			{
				StateManagement::SwitchState<SceneWin>();
			}
		}
	}
	static inline int aaaaaa = 0;
	static inline sg_image* iii;
	static void
	Draw()
	{
		if(!iii)
		{
			iii = background_sky.Get();
		}
		BZZRE::Graphics::SpriteDrawParams sdp;
        sdp.color = {255,128,64,255};
        sdp.xywh = {0,0,sapp_widthf(), sapp_heightf()};
        sdp.image = *iii;
		AddSprite(sdp);

		float mx, my;
		Input::MousePos(&mx, &my);
		if(!ImGui::GetIO().WantCaptureMouse)
			Camera::MouseMovement(mx / sapp_widthf(), my / sapp_heightf(), Input::MouseScroll());
		world->DrawDebugData();
		IterateEntities([](auto pe) { pe->Draw(); });
		
		const char* buildingsLeftFmt = "Buildings left: %i";
		const char* animalsSaved = "Deers killed: %i/%i. (Got away: %i)";
		char tmpstr[1024];
		

		auto a = ImGui::GetFont();
		a->Scale = 1.5f;
	
		auto list = ImGui::GetForegroundDrawList();
		
		ImGui::PushFont(a);
		int num = sprintf_s(tmpstr, sizeof(tmpstr), buildingsLeftFmt, GameVar.BuildingsLeft);
		list->AddText({0,0}, 0xFF00FF00, tmpstr, tmpstr+num);

		num = sprintf_s(tmpstr, sizeof(tmpstr), animalsSaved, GameVar.AnimalsDied, GameVar.Animals, GameVar.AnimalsSaved);
		list->AddText({0,14 * 1.5f}, 0xFF00FF00, tmpstr, tmpstr+num);
		ImGui::PopFont();
		a->Scale = 1;
	}

	static void
	Leave()
	{
		AllocateWorld(false);
	}
};