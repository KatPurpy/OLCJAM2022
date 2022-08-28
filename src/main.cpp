#include "stdio.h"
#include "BZZRE/subsystems/init.hpp"
#include "BZZRE/resources/spritesheet.hpp"
#include "BZZRE/subsystems/shaders.hpp"
#include "BZZRE/subsystems/graphics/spritedrawlist.tcc"
#include "BZZRE/subsystems/graphics/pipelinebuilder.hpp"
#include "BZZRE/libs.h"
#include "shader_default2D.h"
#include "Box2D.h"
#include "imguidebugdraw.cpp"
#include "camera.hpp"
#include "BZZRE/subsystems/input.hpp"
#include "sun.hpp"
#include "cloud.hpp"
#include "terrain.hpp"

using namespace BZZRE;


typedef struct
{
	float x, y, z;			  //	4	*	3	=	12
	unsigned char r, g, b, a; //	4
	unsigned short u, v;	  //	2*2	=	4
							  //	12	+	4	+	4	=	20
} vert;
Graphics::SpriteDrawList<vert, uint16_t>* drawList;
sg_pipeline spritePipeline;
sg_bindings bind;

SimpleSpriteShader_vs_params_t params;
BZZRE::Graphics::UniformParams uparams = { { SG_RANGE(params) } };
BZZRE::SpriteSheet* sheet;

b2World* world;

Sun sun;
Cloud cloud;
Terrain terrain;

void init()
{
    Assets::AddDirectory("assets");
    Shaders::Add("simpleimg", SimpleSpriteShader_default2D_shader_desc);

	drawList = new Graphics::SpriteDrawList<vert, uint16_t>(4 * 100, 6 * 100, 4 * 2048, 6 * 2048);
	sg_shader shd = Shaders::Get("simpleimg");

	auto pipdesc = Graphics::PipelineBuilder(SG_INDEXTYPE_UINT16, shd)
					   .Blend(0, SG_BLENDFACTOR_SRC_ALPHA, SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA)
					   .Attribute(ATTR_SimpleSpriteShader_vs_pos, SG_VERTEXFORMAT_FLOAT3)
					   .Attribute(ATTR_SimpleSpriteShader_vs_color0, SG_VERTEXFORMAT_UBYTE4N)
					   .Attribute(ATTR_SimpleSpriteShader_vs_texcoord0, SG_VERTEXFORMAT_USHORT2N)
					   .Done();

	spritePipeline = sg_make_pipeline(pipdesc);

    sheet = (new BZZRE::SpriteSheet("assets/test.txt"))->Get();
	
    // Define the gravity vector.
	b2Vec2 gravity(0.0f, -10.0f);

	// Construct a world object, which will hold and simulate the rigid bodies.
	world = new b2World(gravity);
    FooDraw* draw = new FooDraw();
    world->SetDebugDraw(draw);
    draw->SetFlags(b2Draw::e_shapeBit);

    Camera::x = -150;
    Camera::y = -150;
    Camera::screen_margin_x = 0.35f;
    Camera::screen_margin_y = 0.35f;
    Camera::speed = 6;
    Camera::ppm = 8;

    sun.Instantiate(world);
    cloud.Instantiate(world);
    terrain.Initialize(world, 45, 0, 128, 128, 32);
    terrain.Generate(world, [](int,float x){return (float)(((int)(x/32)) * 1);});

}

void update()
{
    	simgui_frame_desc_t d = {
            .width = sapp_width(),
            .height = sapp_height(),
            .delta_time = sapp_frame_duration(),
            .dpi_scale = sapp_dpi_scale()
        };
	simgui_new_frame(&d);
    sun.Update();
    cloud.Update();
      if(Input::MouseClick(SAPP_MOUSEBUTTON_MIDDLE))
      {
            float aaa[20] = {5};
        for(int i = 0; i < 20; i++) aaa[i] = -5;

        float mx, my;
        Input::MousePos(&mx, &my);

        terrain.WriteHeightMapOffset(aaa, 20, Camera::ScreenToBox2D({mx,0}).x);
        terrain.RegenerateChunks();
      }
    world->Step(1.f/60.0f, 6, 2);
   // ImGui::Text("it finally works");
}
char IMGNAME[256] = {0};
void draw()
{
    sg_pass_action act{0};
    sg_begin_default_pass(&act, sapp_width(), sapp_height());
    params.mvp = HMM_Orthographic(0.f, sapp_widthf(), sapp_heightf(), 0, 0.01f, 100.f);

    float mx, my;
    Input::MousePos(&mx, &my);
    Camera::MouseMovement(mx/sapp_widthf(),my/sapp_heightf());

  

    world->DrawDebugData();

    simgui_render();
    sg_end_pass();
}

int main()
{
    BZZRE::Base::initargs args{0};
    args.windowtitle = "TOP SECRET";
    args.windowWidth = 600;
    args.windowHeight = 480;
    args.init = init;
    args.update = update;
    args.draw = draw;
    BZZRE::Base::Run(args);
}