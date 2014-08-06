#include "Precompiled.hpp"
#include "LuaMath.hpp"
#include "LuaEngine.hpp"

bool BindLuaMath(LuaEngine& lua)
{
    if(!lua.IsValid())
        return false;

    // Bind definitions.
    Lua::getGlobalNamespace(lua.GetState())
        .beginNamespace("Private")
            .beginClass<glm::vec2>("glmvec2")
            .endClass()
        .endNamespace()
        .deriveClass<LuaVector<glm::vec2>, glm::vec2>("Vec2")
            .addConstructor<void(*)(float, float)>()
            .addData("x", &LuaVector<glm::vec2>::x)
            .addData("y", &LuaVector<glm::vec2>::y)
            .addFunction("Equals", &LuaVector<glm::vec2>::Equals)
            .addFunction("Normalized", &LuaVector<glm::vec2>::Normalized)
            .addFunction("Normalize", &LuaVector<glm::vec2>::Normalize)
        .endClass();

    Lua::getGlobalNamespace(lua.GetState())
        .beginNamespace("Private")
            .beginClass<glm::vec3>("glmvec3")
            .endClass()
        .endNamespace()
        .deriveClass<LuaVector<glm::vec3>, glm::vec3>("Vec3")
            .addConstructor<void(*)(float, float, float)>()
            .addData("x", &LuaVector<glm::vec3>::x)
            .addData("y", &LuaVector<glm::vec3>::y)
            .addData("z", &LuaVector<glm::vec3>::z)
            .addData("r", &LuaVector<glm::vec3>::r)
            .addData("g", &LuaVector<glm::vec3>::g)
            .addData("b", &LuaVector<glm::vec3>::b)
            .addFunction("Equals", &LuaVector<glm::vec3>::Equals)
        .endClass();

    Lua::getGlobalNamespace(lua.GetState())
        .beginNamespace("Private")
            .beginClass<glm::vec4>("glmvec4")
            .endClass()
        .endNamespace()
        .deriveClass<LuaVector<glm::vec4>, glm::vec4>("Vec4")
            .addConstructor<void(*)(float, float, float, float)>()
            .addData("x", &LuaVector<glm::vec4>::x)
            .addData("y", &LuaVector<glm::vec4>::y)
            .addData("z", &LuaVector<glm::vec4>::z)
            .addData("w", &LuaVector<glm::vec4>::w)
            .addData("r", &LuaVector<glm::vec4>::r)
            .addData("g", &LuaVector<glm::vec4>::g)
            .addData("b", &LuaVector<glm::vec4>::b)
            .addData("a", &LuaVector<glm::vec4>::a)
            .addFunction("Equals", &LuaVector<glm::vec4>::Equals)
        .endClass();

    return true;
}
