#pragma once
#include "physicsentity.hpp"

template <typename T> int sgn(T val) {
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

    public:
        Animal();
        void GetScared(float scare_x);
        b2Body* CreateBody(b2World* world);
        void Update();
        void Draw();
        void OnCollisionEnter(b2Body* otherBody);
        void OnParticleColisionEnter(b2ParticleSystem* particleSystem,
                               b2ParticleBodyContact* particleBodyContact) ;
        void Destroy(bool);
};