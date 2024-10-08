#include "animal.hpp"
#include "stdio.h"
#include "camera.hpp"

const int count = 4;
BZZRE::Image images[count] = 
{
	BZZRE::Image("deer1.qoi"),
	BZZRE::Image("deer2.qoi"),
	BZZRE::Image("deer3.qoi"),
	BZZRE::Image("deer4.qoi"),
};
hmm_v2 deerSize = {2.5f,2.f};

Animal::Animal()
{
	type = Constants::PC_ANIMAL;
	canBeSetOnFire = true;
    burnData.particleThreshold = 1;
	image = *images[rand() % count].Get();
	switch (rand()%2)
	{
		case 0: direction = 1; break;
		case 1: direction = -1; break;
	}
}

void
Animal::GetScared(float scare_x)
{
	direction = sgn(body->GetPosition().x - scare_x);
	;
	body->ApplyLinearImpulse({ direction * 5 * body->GetMass(), 6 * body->GetMass() }, body->GetPosition(), true);
	scared = true;
}

b2Body*
Animal::CreateBody(b2World* world)
{
	b2PolygonShape shape;
	shape.SetAsBox(deerSize.Width, deerSize.Height);

	b2BodyDef bdef;
	bdef.type = b2_dynamicBody;
	bdef.fixedRotation = !true;
	auto body = world->CreateBody(&bdef);

	b2FixtureDef def;
	def.restitution = 0.35f;
	def.density = 1.2f;
	def.friction = 0.5f;
	def.shape = &shape;
	def.filter.categoryBits = Constants::PC_ANIMAL;
	def.filter.maskBits = Constants::ANIMAL_COLLIDESWITH;
	body->CreateFixture(&def);
	return body;
}
#include "overlapcheck.hpp"

void
Animal::Jump()
{
	float jumpforce = (rand() % 6) / 6. * 2;
	jumptimer = (rand() % 12) / 12. * 1 + 1;
	body->ApplyLinearImpulse({ direction * 4 * jumpforce * body->GetMass(), 8 * jumpforce * body->GetMass() },
							 body->GetPosition(), true);
}

void
Animal::Update()
{
	b2Vec2 f = { direction * 10, 0 };

	if(scared)
	{
		jumptimer -= 1.f / 60.f;
		//  printf("jumptimer %f\n", jumptimer);
		if(jumptimer < 0)
		{
			Jump();
		}
		OverlapCheck<void*, Constants::PC_GROUND, true> check;
		void** discard = check.OverlapCircle(body->GetWorld(), body->GetWorldCenter(), 2);
		if(arrlen(discard) != 0)
		{
			body->ApplyForceToCenter(f * body->GetMass(), true);
		}
	} else
    {
        if(burnData.onFire)
        {
            GetScared(rand()/RAND_MAX - 0.5f);
        }
    }
	PhysicsEntity::Update();
    
}



void
Animal::Draw()
{
	BZZRE::Graphics::SpriteDrawParams spr;
	spr.image = image;
	auto pos = Camera::Box2DToScreen(body->GetPosition());
	auto inf = sg_query_image_info(spr.image);
	spr.color = {255,255,255,255};
	spr.xywh = {pos.X, pos.Y,
	direction * deerSize.Width * 2, deerSize.Height * 2};
	spr.xywh.ZW *= Camera::ppm;
	spr.r = -body->GetAngle();
	spr.origin = {inf.width/2.f,inf.height/2.f};
	//spr.origin = {boxSize.Width * Camera::ppm, boxSize.Height * Camera::ppm };
	//spr.origin *= Camera::ppm;
	AddSprite(spr);
}
void KillAnimal(Animal* animal);
void
Animal::Kill(bool silent)
{
	dead = true;
	if(silent)
		return;
	KillAnimal(this);
}