#include "camera.hpp"
#include "sokol_app.h"
#include "BZZRE/subsystems/graphics/spritedrawparams.h"
namespace Camera
{
	float ppm, x, y, w, h, screen_margin_x, screen_margin_y, speed;
    hmm_v4 bounds;
}

hmm_v2
Camera::Box2DToScreen(b2Vec2 vec)
{
    w = sapp_widthf();
    h = sapp_heightf();
	return { (-Camera::x + vec.x) * Camera::ppm + w/2, (Camera::y + -vec.y) * Camera::ppm + h/2 };
}
#include "stdio.h"
b2Vec2
Camera::ScreenToBox2D(hmm_v2 vec)
{
    w = sapp_widthf();
    h = sapp_heightf();
	b2Vec2 out = { Camera::x  + (vec.X - w/2) / Camera::ppm, Camera::y + (-vec.Y + h/2) / Camera::ppm };
	return out;
}

void DrawParamToScreen(BZZRE::Graphics::SpriteDrawParams& params)
{
    params.xywh *= Camera::ppm;
}

void
Camera::MouseMovement(float mx, float my, float ms)
{
	float margin_x = screen_margin_x;
	float margin_y = screen_margin_y;
	float camspeed = speed;

	if(ms != 0)
	{
		ppm += ms;
		if(ppm > 16)
			ppm = 16;
		if(ppm < 4)
			ppm = 4;
	}
    float oldX = Camera::x;
    float oldY = Camera::y;
	if(mx < margin_x)
	{
		float factor = (1 - mx / margin_x);
		Camera::x -= factor * camspeed;
	}

	float marg_right = (1 - margin_x);
	if(mx > marg_right)
	{
		float factor = ((mx - marg_right) / margin_x);
		Camera::x += (factor)*camspeed;
	}

	if(my < margin_y)
	{
		float factor = (1 - my / margin_y);
		Camera::y += factor * camspeed;
	}

	float marg_bottom = (1 - margin_y);
	if(my > marg_bottom)
	{
		float factor = ((my - marg_bottom) / margin_y);
		Camera::y -= (factor)*camspeed;
	}
    if(Camera::x < Camera::bounds[0] || Camera::x > Camera::bounds[1]) Camera::x = oldX;
    if(Camera::y < Camera::bounds[2] || Camera::y > Camera::bounds[3]) Camera::y = oldY;
}