#include "Precompiled.hpp"
#include "LuaMath.hpp"
#include "LuaState.hpp"

bool BindLuaMath(LuaState& state)
{
    if(!state.IsValid())
        return false;

    // Bind definitions.
    Lua::getGlobalNamespace(state.GetState())
        .beginClass<glm::vec2>("Vec2")
            .addConstructor<void(*)(float, float)>()
            .addData("x", &glm::vec2::x)
            .addData("y", &glm::vec2::y)
        .endClass();

    Lua::getGlobalNamespace(state.GetState())
        .beginClass<glm::vec3>("Vec3")
            .addConstructor<void(*)(float, float, float)>()
            .addData("x", &glm::vec3::x)
            .addData("y", &glm::vec3::y)
            .addData("z", &glm::vec3::z)
            .addData("r", &glm::vec3::r)
            .addData("g", &glm::vec3::g)
            .addData("b", &glm::vec3::b)
        .endClass();

    Lua::getGlobalNamespace(state.GetState())
        .beginClass<glm::vec4>("Vec4")
            .addConstructor<void(*)(float, float, float, float)>()
            .addData("x", &glm::vec4::x)
            .addData("y", &glm::vec4::y)
            .addData("z", &glm::vec4::z)
            .addData("w", &glm::vec4::w)
            .addData("r", &glm::vec4::r)
            .addData("g", &glm::vec4::g)
            .addData("b", &glm::vec4::b)
            .addData("a", &glm::vec4::a)
        .endClass();

    return true;
}
