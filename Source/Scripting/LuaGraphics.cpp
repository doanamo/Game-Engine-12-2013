#include "Precompiled.hpp"
#include "LuaGraphics.hpp"
#include "LuaEngine.hpp"

#include "MainGlobal.hpp"
#include "Graphics/ScreenSpace.hpp"
#include "Graphics/Font.hpp"
#include "Graphics/CoreRenderer.hpp"
#include "Graphics/TextRenderer.hpp"

namespace
{
    // Proxy functions.
    void SetTextDrawAlign(TextDrawInfo& info, uint32_t align)
    {
        info.align = (TextDrawAlign::Type)align;
    }

    uint32_t GetTextDrawAlign(const TextDrawInfo& info)
    {
        return (uint32_t)info.align;
    }
}

bool BindLuaGraphics(LuaEngine& lua)
{
    if(!lua.IsValid())
        return false;

    // Bind definitions.
    Lua::getGlobalNamespace(lua.GetState())
        .beginNamespace("Graphics")
            .beginClass<ScreenSpace>("ScreenSpace")
                .addConstructor<void(*)(void)>()
                .addFunction("SetSourceSize", &ScreenSpace::SetSourceSize)
                .addFunction("GetSourceSize", &ScreenSpace::GetSourceSize)
                .addFunction("SetTargetSize", &ScreenSpace::SetSourceSize)
                .addFunction("GetTargetSize", &ScreenSpace::GetSourceSize)
                .addFunction("SetTargetAspect", &ScreenSpace::SetTargetAspect)
                .addFunction("GetRectangle", &ScreenSpace::GetRectangle)
                .addFunction("GetOffset", &ScreenSpace::GetOffset)
            .endClass()
        .endNamespace();

    Lua::getGlobalNamespace(lua.GetState())
        .beginNamespace("Graphics")
            .beginClass<Font>("Font")
            .endClass()
            .addFunction("GetDefaultFont", &Main::GetDefaultFont)
        .endNamespace();

    Lua::getGlobalNamespace(lua.GetState())
        .beginNamespace("Graphics")
            .beginClass<TextDrawInfo>("TextDrawInfo")
                .addConstructor<void(*)(void)>()
                .addData("font", &TextDrawInfo::font)
                .addData("size", &TextDrawInfo::size)
                .addPropertyProxy("align", &GetTextDrawAlign, &SetTextDrawAlign)
                .addData("bodyColor", &TextDrawInfo::bodyColor)
                .addData("outlineColor", &TextDrawInfo::outlineColor)
                .addData("glowColor", &TextDrawInfo::glowColor)
                .addData("outlineRange", &TextDrawInfo::outlineRange)
                .addData("glowRange", &TextDrawInfo::glowRange)
                .addData("position", &TextDrawInfo::position)
                .addData("area", &TextDrawInfo::area)
                .addData("cursorIndex", &TextDrawInfo::cursorIndex)
                .addData("debug", &TextDrawInfo::debug)
            .endClass()
        .endNamespace();

    Lua::getGlobalNamespace(lua.GetState())
        .beginNamespace("Graphics")
            .beginClass<TextDrawMetrics>("TextDrawMetrics")
                .addConstructor<void(*)(void)>()
                .addData("textArea", &TextDrawMetrics::textArea)
                .addData("boundingBox", &TextDrawMetrics::boundingBox)
                .addData("lines", &TextDrawMetrics::lines)
            .endClass()
        .endNamespace();

    Lua::getGlobalNamespace(lua.GetState())
        .beginNamespace("Graphics")
            .beginClass<CoreRenderer>("CoreRenderer")
                .addFunction("Clear", &CoreRenderer::Clear)
                .addFunction("SetClearColor", &CoreRenderer::SetClearColor)
                .addFunction("SetClearDepth", &CoreRenderer::SetClearDepth)
            .endClass()
        .endNamespace();

    // Define constants.
    Lua::LuaRef textDrawAlign(lua.GetState());
    textDrawAlign = Lua::newTable(lua.GetState());
    textDrawAlign["Default"] = (uint32_t)TextDrawAlign::Default;
    textDrawAlign["TopLeft"] = (uint32_t)TextDrawAlign::TopLeft;
    textDrawAlign["TopRight"] = (uint32_t)TextDrawAlign::TopRight;
    textDrawAlign["BottomLeft"] = (uint32_t)TextDrawAlign::BottomLeft;
    textDrawAlign["BottomRight"] = (uint32_t)TextDrawAlign::BottomRight;
    textDrawAlign["Centered"] = (uint32_t)TextDrawAlign::Centered;

    textDrawAlign.push(lua.GetState());
    lua_setglobal(lua.GetState(), "TextDrawAlign");

    Lua::LuaRef clearFlags(lua.GetState());
    clearFlags = Lua::newTable(lua.GetState());
    clearFlags["None"] = (uint32_t)ClearFlags::None;
    clearFlags["Color"] = (uint32_t)ClearFlags::Color;
    clearFlags["Depth"] = (uint32_t)ClearFlags::Depth;

    clearFlags.push(lua.GetState());
    lua_setglobal(lua.GetState(), "ClearFlags");

    // Pass global references.
    Lua::push(lua.GetState(), Main::GetCoreRenderer());
    lua_setglobal(lua.GetState(), "CoreRenderer");

    return true;
}
