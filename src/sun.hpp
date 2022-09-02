#pragma once
#include "unit.hpp"

class Sun : public Unit
{
  protected:
	const int cloudCost = 27;
	int cloudMaterial{};
	void Ability();

  public:
	Sun();
	b2BodyDef BodyDef();
	void Update();
	void Draw();
	b2Body* CreateBody(b2World*);
	void OnCollisionEnter(b2Body*);
	void OnParticleColisionEnter(b2ParticleSystem* particleSystem, b2ParticleBodyContact* particleBodyContact);
};