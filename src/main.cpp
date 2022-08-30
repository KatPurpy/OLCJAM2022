#include "stdio.h"
#include "BZZRE/subsystems/init.hpp"
#include "BZZRE/resources/spritesheet.hpp"
#include "BZZRE/subsystems/shaders.hpp"
#include "BZZRE/subsystems/graphics/spritedrawlist.tcc"
#include "BZZRE/subsystems/graphics/pipelinebuilder.hpp"
#include "BZZRE/libs.h"
#include "shader_default2D.h"
#include "camera.hpp"
#include "statemanagement.hpp"
#include "scene_level.hpp"
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
	
    StateManagement::SwitchState<SceneLevel>();
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
    StateManagement::Update();
}
char IMGNAME[256] = {0};
void draw()
{
    sg_pass_action act{0};
    sg_begin_default_pass(&act, sapp_width(), sapp_height());
    params.mvp = HMM_Orthographic(0.f, sapp_widthf(), sapp_heightf(), 0, 0.01f, 100.f);

    StateManagement::Draw();

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