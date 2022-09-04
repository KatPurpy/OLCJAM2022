#include "BZZRE/subsystems/audio.hpp"
#include "soloud.h"
#include "soloud_wavstream.h"
SoLoud::WavStream ending;
BZZRE::Image ending_image("ending_v2.jpg");
struct SceneWin
{
    static void Enter()
    {
        ending.load("assets/rickroll.ogg");
        BZZRE::Audio::GetSoloud()->stopAll();
        BZZRE::Audio::GetSoloud()->play(ending, 2);
    }

    static void Update()
    {

    }

    static void Draw()
    {
        BZZRE::Graphics::SpriteDrawParams sdp;
        sdp.color = {255,255,255,255};
        sdp.xywh = {0,0,sapp_widthf(), sapp_heightf()};
        sdp.image = *ending_image.Get();
        AddSprite(sdp);
    }

    static void Leave()
    {

    }
};