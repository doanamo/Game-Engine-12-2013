#include "Precompiled.hpp"
#include "LuaMath.hpp"
#include "LuaEngine.hpp"

namespace
{
    template<typename Type>
    bool Equals(const Type& left, const Type& right)
    {
        return left == right;
    }

    template<typename Type>
    Type Normalized(const Type& vector)
    {
        return glm::normalize(vector);
    }

    template<typename Type>
    void Normalize(Type& vector)
    {
        glm::normalize(vector);
    }
}

bool BindLuaMath(LuaEngine& lua)
{
    if(!lua.IsValid())
        return false;

    // Bind definitions.
    Lua::getGlobalNamespace(lua.GetState())
        .beginClass<glm::vec2>("Vec2")
            .addConstructor<void(*)(float, float)>()
            .addData("x", &glm::vec2::x)
            .addData("y", &glm::vec2::y)
            .addSelfFunction("Equals", &Equals<glm::vec2>)
            .addSelfFunction("Normalized", &Normalized<glm::vec2>)
            .addSelfFunction("Normalize", &Normalize<glm::vec2>)
        .endClass();

    Lua::getGlobalNamespace(lua.GetState())
        .beginClass<glm::vec3>("Vec3")
            .addConstructor<void(*)(float, float, float)>()
            .addData("x", &glm::vec3::x)
            .addData("y", &glm::vec3::y)
            .addData("z", &glm::vec3::z)
            .addData("r", &glm::vec3::r)
            .addData("g", &glm::vec3::g)
            .addData("b", &glm::vec3::b)
            .addSelfFunction("Equals", &Equals<glm::vec3>)
            .addSelfFunction("Normalized", &Normalized<glm::vec3>)
            .addSelfFunction("Normalize", &Normalize<glm::vec3>)
        .endClass();

    Lua::getGlobalNamespace(lua.GetState())
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
            .addSelfFunction("Equals", &Equals<glm::vec4>)
        .endClass();

    return true;
}
