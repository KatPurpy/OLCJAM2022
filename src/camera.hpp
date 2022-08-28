#include "HandmadeMath.h"
#include "Box2D.h"
namespace Camera
{
    extern float ppm, x,y,w,h, screen_margin_x, screen_margin_y, speed;  
    hmm_v2 Box2DToScreen(b2Vec2);
    b2Vec2 ScreenToBox2D(hmm_v2);
    void MouseMovement(float mx, float my);
};