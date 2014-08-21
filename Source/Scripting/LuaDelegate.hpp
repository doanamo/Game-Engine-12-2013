#pragma once

#include "Precompiled.hpp"

class LuaDelegate
{
public:
    LuaDelegate() :
        m_instance(),
        m_function()
    {
    }

    void Cleanup()
    {
        m_instance = Lua::LuaRef();
        m_function = Lua::LuaRef();
    }

    void Bind(Lua::LuaRef& function, Lua::LuaRef& instance)
    {
        m_instance = instance;
        m_function = function;
    }

    template<typename... Arguments>
    Lua::LuaRef Invoke(Arguments... arguments)
    {
        if(m_function.state() != nullptr && m_function.isFunction())
        {
            if(m_instance.state() != nullptr && m_instance.isTable())
            {
                // Call as a method.
                return m_function(m_instance, arguments...);
            }
            else
            {
                // Call as a function.
                return m_function(arguments...);
            }
        }
        else
        {
            return Lua::LuaRef();
        }
    }

private:
    Lua::LuaRef m_instance;
    Lua::LuaRef m_function;
};
