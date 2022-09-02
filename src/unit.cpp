#include "unit.hpp"
#include "stdio.h"

Unit::Unit() { type = Constants::PC_PLAYERUNIT; }

Unit::~Unit()
{
	printf("DEL UNIT\n");
	if(!destroyed)
		Kill(true);
}

void
Unit::SetPosition(b2Vec2 pos)
{
	body->SetTransform(pos, 0);
	targetpos = pos;
}

void
Unit::Follow()
{
	auto thing = targetpos;
	auto dir = thing - body->GetPosition();
	auto distance = dir.Length();
	dir.Normalize();
	auto a = distance;
	auto b = m_maxspeed;
	dir *= ((b > a) ? a : b) * 0.1f;
	dir += body->GetPosition();
	body->SetTransform(dir, 0);
	body->SetLinearVelocity({ 0, 0 });
}

void
Unit::BaseUnitUpdate()
{
	if(lock)
	{
        Follow();
		if(BZZRE::Input::MouseUp(SAPP_MOUSEBUTTON_LEFT))
		{
			lock = false;
		}
        return;
	}
	if(BZZRE::Input::MouseClick(SAPP_MOUSEBUTTON_LEFT))
	{
		hmm_v2 pos;
		BZZRE::Input::MousePos(&pos.X, &pos.Y);
		auto worldpos = Camera::ScreenToBox2D(pos);
		auto fixture = body->GetFixtureList()[0]; // a unit always has only 1 fixture
		auto testpoint = fixture.TestPoint(worldpos);
		if(Unit::current != this && testpoint)
		{
            if(Unit::current)
            {
                Unit::current->targetpos = Unit::current->body->GetPosition();
            }
            Unit::current = this;
			lock = true;
			return;
		}

		if(Unit::current == this && testpoint)
		{
			if(ability_cooldown < 0)
			{
				Ability();
				ability_cooldown = 0.25f;
			}
			lock = true;
			return;
		}
	}
	else
    {
        ability_cooldown -= 1. / 60.;
    }
	if(Unit::current == this && BZZRE::Input::MouseDown(SAPP_MOUSEBUTTON_LEFT))
	{
		hmm_v2 pos;
		BZZRE::Input::MousePos(&pos.X, &pos.Y);
		targetpos = Camera::ScreenToBox2D(pos);
	}
	Follow();
}