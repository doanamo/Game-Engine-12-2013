#pragma once

#include "Precompiled.hpp"

#include "MainGlobal.hpp"
#include "Game/Component/Component.hpp"

//
// Script Component
//

class ScriptComponent : public Component
{
public:
    // Type declarations.
    typedef std::vector<Lua::LuaRef> ScriptList;

public:
    ScriptComponent();
    ~ScriptComponent();

    void AddScript(Lua::LuaRef script);

    template<typename... Arguments>
    void Call(std::string name, Arguments... arguments);

private:
    ScriptList m_scripts;
};

template<typename... Arguments>
void ScriptComponent::Call(std::string name, Arguments... arguments)
{
    // Execute every script in order.
    for(Lua::LuaRef& script : m_scripts)
    {
        // Get the function reference.
        Lua::LuaRef function = script[name];

        // Call the function.
        if(function.isFunction())
        {
            try
            {
                function(arguments...);
            }
            catch(Lua::LuaException& exception)
            {
                // Get the exception error text.
                std::string error = exception.what();

                // Remove base path to working directory.
                std::size_t position = error.find(Main::GetWorkingDir());

                if(position != std::string::npos)
                {
                    error.erase(position, Main::GetWorkingDir().size());
                }

                // Print the error.
                Log() << "Lua error - " << error << ".";
            }
        }
    }
}
