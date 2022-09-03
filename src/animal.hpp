#pragma once
#include "physicsentity.hpp"
#include "BZZRE/resources/image.hpp"

template <typename T>
int
sgn(T val)
{
	return (T(0) < val) - (val < T(0));
}

class Animal : public PhysicsEntity
{
  private:
	bool scared{};
	float direction;
	float speed;
	float jumptimer{};
	void Jump();
	sg_image image;

  public:
	Animal();
	void GetScared(float scare_x);
	b2Body* CreateBody(b2World* world);
	void Update();
	void Draw();
	void Kill(bool);
};