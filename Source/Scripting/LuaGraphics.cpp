#include "Precompiled.hpp"
#include "LuaGraphics.hpp"
#include "LuaEngine.hpp"

#include "MainGlobal.hpp"
#include "Graphics/ScreenSpace.hpp"
#include "Graphics/CoreRenderer.hpp"

bool BindLuaGraphics(LuaEngine& lua)
{
    if(!lua.IsValid())
        return false;

    // Bind definitions.
    Lua::getGlobalNamespace(lua.GetState())
        .beginClass<ScreenSpace>("ScreenSpace")
            .addConstructor<void(*)(void)>()
            .addFunction("SetSourceSize", &ScreenSpace::SetSourceSize)
            .addFunction("GetSourceSize", &ScreenSpace::GetSourceSize)
            .addFunction("SetTargetSize", &ScreenSpace::SetSourceSize)
            .addFunction("GetTargetSize", &ScreenSpace::GetSourceSize)
            .addFunction("SetTargetAspect", &ScreenSpace::SetTargetAspect)
            .addFunction("GetRectangle", &ScreenSpace::GetRectangle)
            .addFunction("GetOffset", &ScreenSpace::GetOffset)
        .endClass();

    Lua::getGlobalNamespace(lua.GetState())
        .beginClass<CoreRenderer>("CoreRenderer")
            .addFunction("Clear", &CoreRenderer::Clear)
            .addFunction("SetClearColor", &CoreRenderer::SetClearColor)
            .addFunction("SetClearDepth", &CoreRenderer::SetClearDepth)
        .endClass();

    // Define constants.
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
