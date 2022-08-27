#include "unit.hpp"
#include "stdio.h"
void Unit::Follow()
{
    auto thing = targetpos;
    auto dir = thing - body->GetPosition();
    auto distance = dir.Length();
    dir.Normalize();
    auto a = distance;
    auto b = m_maxspeed;
    dir *= ((b>a)?a:b) * 0.1f;
    dir += body->GetPosition();
    body->SetTransform(dir, 0);
}

void Unit::BaseUnitUpdate()
{
    static bool lock = false;
    if(lock) 
    {
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
        auto fixture = body->GetFixtureList()[0]; //a unit always has only 1 fixture
        if(fixture.TestPoint(worldpos))
        {
            Unit::current = this;
             lock = true;
        }
       
        return;
    }

    if(Unit::current == this && BZZRE::Input::MouseDown(SAPP_MOUSEBUTTON_LEFT))
    {
        hmm_v2 pos;
        BZZRE::Input::MousePos(&pos.X, &pos.Y);
        targetpos = Camera::ScreenToBox2D(pos);
    }
    Follow();
}