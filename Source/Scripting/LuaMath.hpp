#pragma once

#include "Precompiled.hpp"

// Forward declarations.
class LuaState;

//
// Lua Math
//

bool BindLuaMath(LuaState& state);

//
// Vector Wrapper
//

template<typename Type>
struct LuaVector : public Type
{
    template<typename... Arguments>
    LuaVector(Arguments... arguments) :
        Type(arguments...)
    {
    }

    operator Type&()
    {
        return *this;
    }

    operator const Type&()
    {
        return *this;
    }

    bool Equals(const Type& right) const
    {
        return *this == right;
    }

    LuaVector<Type> Normalized() const
    {
        return glm::normalize((const Type&)*this);
    }

    void Normalize()
    {
        *this = glm::normalize((const Type&)*this);
    }
};
