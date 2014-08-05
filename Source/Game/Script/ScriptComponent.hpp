#pragma once

#include "Precompiled.hpp"
#include "Script.hpp"

#include "MainGlobal.hpp"
#include "Game/Component/Component.hpp"

//
// Script Lua Component
//

class ScriptLuaComponent : public Component
{
public:
    // Type declarations.
    typedef std::vector<Lua::LuaRef> ScriptList;

public:
    ScriptLuaComponent();
    ~ScriptLuaComponent();

    void AddScript(Lua::LuaRef script);

    template<typename... Arguments>
    void Call(std::string name, Arguments... arguments);

private:
    ScriptList m_scripts;
};

template<typename... Arguments>
void ScriptLuaComponent::Call(std::string name, Arguments... arguments)
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

//
// Script Component
//

class ScriptComponent : public Component
{
public:
    // Friend declarations.
    friend class ScriptSystem;

    // Type declarations.
    typedef std::shared_ptr<Script> ScriptPtr;
    typedef std::vector<ScriptPtr> ScriptList;

public:
    ScriptComponent();
    ~ScriptComponent();

    void AddScript(ScriptPtr script);

    // Script calls.
    void OnCreate(EntityHandle self);
    void OnUpdate(EntityHandle self, float timeDelta);
    void OnDamage(EntityHandle self, int value, bool alive);
    void OnHeal(EntityHandle self, int value);
    void OnCollision(const CollisionObject& self, const CollisionObject& other);
    
private:
    ScriptList m_scripts;
    bool       m_touched;
};
