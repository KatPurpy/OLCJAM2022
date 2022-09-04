#include "imgui.h"
BZZRE::Image main_menu_img("mainmenu.jpg");
static const char* leveled_str = "Level editor";
static const char* credits = "Credits";
struct SceneMainMenu
{
    static void Enter()
    {

    }

//https://github.com/ocornut/imgui/discussions/3862
static bool ButtonCenteredOnLine(const char* label, float alignment = 0.5f)
{
    ImGuiStyle& style = ImGui::GetStyle();

    float size = ImGui::CalcTextSize(label).x + style.FramePadding.x * 2.0f;
    float avail = ImGui::GetContentRegionAvail().x;

    float off = (avail - size) * alignment;
    if (off > 0.0f)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

    return ImGui::Button(label);
}

    static void Update()
    {
        ImVec2 middle = {sapp_widthf()/2, sapp_heightf()/2};
        ImVec2 size = {110,160};
        ImVec2 pos = {middle.x - size.x/2, middle.y - size.y/2};
        ImGui::SetNextWindowPos(pos);
        ImGui::SetNextWindowSize(size, ImGuiCond_Always);
        ImGui::Begin("PAIN", (bool*)nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize );
        if(ButtonCenteredOnLine("Play"))
        {
            SceneLevel::LevelPath = "assets/levels/level3.txt";
            SceneLevel::LevelEditor = false;
            StateManagement::SwitchState<SceneLevel>();
        }
        if(ButtonCenteredOnLine(leveled_str))
        {
            leveled_str = "nope";

//            SceneLevel::LevelPath = "devmap.txt";
  //        SceneLevel::LevelEditor = "assets/levels/level3.txt";
    //        StateManagement::SwitchState<SceneLevel>();
        }
        if(ButtonCenteredOnLine(credits))
        {
            credits = "Can u read?";
        }
        if(ButtonCenteredOnLine("Exit"))
        {
            sapp_request_quit();
        }
        ImGui::End();
    }
    static void Draw()
    {
        BZZRE::Graphics::SpriteDrawParams sdp;
        sdp.color = {255,255,255,255};
        sdp.xywh = {0,0,sapp_widthf(), sapp_heightf()};
        sdp.image = *main_menu_img.Get();
        AddSprite(sdp);
    }

    static void Leave()
    {

    }
};