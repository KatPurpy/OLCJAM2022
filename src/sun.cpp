#include "sun.hpp"
#include "stdio.h"
#include "cloud.hpp"
#include "overlapcheck.hpp"
#include "camera.hpp"
#include "BZZRE/resources/image.hpp"
#include "imgui.h"
Sun::Sun() { m_maxspeed = 20; }

void WaterToSmoke(b2Vec2 pos, float radius);

void
Sun::Update()
{
	BaseUnitUpdate();
	Unit::current = this;
	if(cloudMaterial >= cloudCost)
	{
		cloudMaterial -= cloudCost;
		Cloud* c = new Cloud();
		c->Instantiate(body->GetWorld());
		c->body->SetTransform(body->GetPosition(), 0);
		c->targetpos = body->GetPosition();
		c->targetpos += { 0, 10 };
	}

		const float radius = 4;
        auto pos = body->GetPosition();
		//WaterToSmoke(pos, radius);
        b2CircleShape shape;
	    shape.m_radius = radius;
	    
        OverlapCheck<PhysicsEntity*, (Constants::PhysicsCategory)UINT16_MAX, false> check;
        auto ret = check.OverlapCircle(body->GetWorld(), pos, radius);
        for(int i = 0; i < arrlen(ret); i++)
        {
            ret[i]->burnData.SetOnFireImmediate(ret[i]);
        }
}

BZZRE::Image base("sun_base.qoi");
BZZRE::Image face("sun_face.qoi");

static sg_image* base_img = NULL;
static sg_image* face_img = NULL;
int time = 0;
void
Sun::Draw()
{
	if(!base_img) base_img = base.Get();
	if(!face_img) face_img = face.Get();

	BZZRE::Graphics::SpriteDrawParams sdp;
	sdp.color = {255,255,255,255};
	
	auto pos = Camera::Box2DToScreen(body->GetPosition());
	sdp.xywh = {pos.X, pos.Y, 6, 6};
	sdp.xywh.ZW *= Camera::ppm;
	sdp.origin = {128,128};

	sdp.r = time++ / 90.;
	sdp.image = *base_img;
	AddSprite(sdp);

	sdp.r = 0;
	sdp.image = *face_img;
	AddSprite(sdp);


}


void
Sun::Ability()
{

	{

	}
}

b2Body*
Sun::CreateBody(b2World* world)
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
void
Sun::OnCollisionEnter(b2Body*)
{
}

void
Sun::OnParticleColisionEnter(b2ParticleSystem* particleSystem, b2ParticleBodyContact* particleBodyContact)
{
	void* tag = particleSystem->GetUserDataBuffer()[particleBodyContact->index];

	if(tag == Constants::particleSmokeTag)
	{
		particleSystem->SetParticleFlags(particleBodyContact->index, b2ParticleFlag::b2_zombieParticle);
		cloudMaterial++;
	}

	// TODO: MAKE SMOKE
}