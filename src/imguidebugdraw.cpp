#include "imgui.h"
#include "box2d/box2d.h"
#include "box2d/b2_draw.h"
#include "camera.hpp"

class FooDraw : public b2Draw
  {
    ImU32 b2CtoImU32( const b2Color& color)
    {
        uint32_t packedR = uint32_t(color.r * 255);
        uint32_t packedG = uint32_t(color.g * 255) << 8; // shift bits over 8 places
        uint32_t packedB = uint32_t(color.b * 255) << 16; // shift bits over 16 places
        uint32_t packedA = uint32_t(color.a * 255) << 24; // shift bits over 24 places
        return packedR + packedG + packedB + packedA;
    }
    ImVec2 b2WorldToScreen(b2Vec2 vec)
    {
        hmm_v2 world = Camera::Box2DToScreen(vec);
        return {world.X, world.Y};
    }
  public:
    void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) 
    {
        auto drawlist = ImGui::GetBackgroundDrawList();
        ImVec2 vectors[vertexCount];
        for(int i = 0; i < vertexCount; i++)
        {
            vectors[i] = b2WorldToScreen(vertices[i]);
        }
        drawlist->AddPolyline(vectors, vertexCount, b2CtoImU32(color), ImDrawFlags_None, 2);
    }
    void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) 
    {
        auto drawlist = ImGui::GetBackgroundDrawList();
        ImVec2 vectors[vertexCount];
        for(int i = 0; i < vertexCount; i++)
        {
            vectors[i] = b2WorldToScreen(vertices[i]);
        }
        drawlist->AddConvexPolyFilled(vectors, vertexCount, b2CtoImU32(color));
    }
    void DrawCircle(const b2Vec2& center, float radius, const b2Color& color) 
    {
        auto drawlist = ImGui::GetBackgroundDrawList();
        ImVec2 c = b2WorldToScreen(center);
        drawlist->AddCircle(c, radius, b2CtoImU32(color));
    }
    void DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) 
    {
        auto drawlist = ImGui::GetBackgroundDrawList();
        ImVec2 c = b2WorldToScreen(center);
        drawlist->AddCircleFilled(c, radius * Camera::ppm, b2CtoImU32(color));
    }
    void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) 
    {
        auto drawlist = ImGui::GetBackgroundDrawList();

        drawlist->AddLine(b2WorldToScreen(p1),b2WorldToScreen(p2), b2CtoImU32(color));
    }
    void DrawTransform(const b2Transform& xf) {}

	void DrawPoint(const b2Vec2& p, float size, const b2Color& color)
    {
        auto drawlist = ImGui::GetBackgroundDrawList();
        ImVec2 c = b2WorldToScreen(p);
        drawlist->AddCircleFilled(c, size * Camera::ppm, b2CtoImU32(color));
    }
  };