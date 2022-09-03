#include "physicsentity.hpp"
#include "overlapcheck.hpp"

void SaveAnimal(Animal*);

class AnimalSafeZone : public PhysicsEntity
{
	public:
	AnimalSafeZone()
	{
		canBeSetOnFire = false;
	}
	private:
	b2Body*
	CreateBody(b2World* world)
	{
		b2PolygonShape shape;
		shape.SetAsBox(2, 100);

		b2BodyDef def;
		def.type = b2_staticBody;

		auto body = world->CreateBody(&def);

		b2FixtureDef fdef;
		fdef.isSensor = true;
		fdef.shape = &shape;
		fdef.filter.categoryBits = Constants::PC_ANIMALSENSOR;
		fdef.filter.maskBits = Constants::ANIMALSENSOR_COLLIDESWITH;
		body->CreateFixture(&fdef);
		return body;
	}
	void Draw(){}
	void
	OnCollisionEnter(PhysicsEntity* otherBody)
	{
		printf("GOT TYPE %i\n", otherBody->type);
		if(otherBody->type & Constants::PC_ANIMAL)
		{
			SaveAnimal((Animal*)otherBody);
		}
	}
};