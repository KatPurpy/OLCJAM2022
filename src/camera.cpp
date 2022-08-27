#include "camera.hpp"
#include "sokol_app.h"
namespace Camera
{
    float ppm, x,y,w,h, screen_margin_x, screen_margin_y, speed;  
} 

hmm_v2 Camera::Box2DToScreen(b2Vec2 vec) {
    return {-Camera::x + vec.x * Camera::ppm, -Camera::y + -vec.y * Camera::ppm};
}

b2Vec2 Camera::ScreenToBox2D(hmm_v2 vec) 
{
    return {(Camera::x + vec.X) / Camera::ppm, (-Camera::y -vec.Y) / Camera::ppm};
}

void Camera::MouseMovement(float mx, float my)
{
    float margin_x = screen_margin_x;
    float margin_y = screen_margin_y;
    float camspeed = speed;

   
    if(mx < margin_x)
    {
        float factor = (1 - mx/margin_x);
        Camera::x -= factor * camspeed;
    }

    float marg_right = (1 - margin_x);
    if(mx > marg_right)
    {
        float factor = ((mx-marg_right)/margin_x);
        Camera::x += (factor) * camspeed;
    }

    if(my < margin_y)
    {
        float factor = (1 - my/margin_y);
        Camera::y -= factor * camspeed;
    }

    float marg_bottom = (1 - margin_y);
    if(my > marg_bottom)
    {
        float factor = ((my-marg_bottom)/margin_y);
        Camera::y += (factor) * camspeed;
    }
}