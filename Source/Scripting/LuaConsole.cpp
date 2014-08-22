#include "Precompiled.hpp"
#include "LuaConsole.hpp"
#include "LuaEngine.hpp"

#include "MainGlobal.hpp"
#include "Console/ConsoleSystem.hpp"
#include "Console/ConsoleVariable.hpp"

namespace
{
    void Execute(const char* input)
    {
        Main::GetConsoleSystem().Execute(input);
    }

    ConsoleVariable* GetVariable(lua_State* lua)
    {
        // Get the argument string.
        if(!lua_isstring(lua, 1))
            return nullptr;

        const char* definition = lua_tostring(lua, 1);

        // Get the console variable.
        return Main::GetConsoleSystem().FindDefinition<ConsoleVariable>(definition);
    }

    int SetString(lua_State* lua)
    {
        // Get the console variable.
        ConsoleVariable* variable = GetVariable(lua);

        if(variable == nullptr)
            return 0;

        // Get the second argument.
        if(!lua_isstring(lua, 2))
            return 0;

        const char* value = lua_tostring(lua, 2);

        // Set variable value.
        variable->SetString(value);

        // Return nothing.
        return 0;
    }

    int SetBool(lua_State* lua)
    {
        // Get the console variable.
        ConsoleVariable* variable = GetVariable(lua);

        if(variable == nullptr)
            return 0;

        // Get the second argument.
        if(!lua_isboolean(lua, 2))
            return 0;

        bool value = lua_toboolean(lua, 2) ? true : false;

        // Set variable value.
        variable->SetBool(value);

        // Return nothing.
        return 0;
    }

    int SetInteger(lua_State* lua)
    {
        // Get the console variable.
        ConsoleVariable* variable = GetVariable(lua);

        if(variable == nullptr)
            return 0;

        // Get the second argument.
        if(!lua_isnumber(lua, 2))
            return 0;

        lua_Number value = lua_tonumber(lua, 2);

        // Set variable value.
        variable->SetInteger((int)value);

        // Return nothing.
        return 0;
    }

    int SetFloat(lua_State* lua)
    {
        // Get the console variable.
        ConsoleVariable* variable = GetVariable(lua);

        if(variable == nullptr)
            return 0;

        // Get the second argument.
        if(!lua_isnumber(lua, 2))
            return 0;

        lua_Number value = lua_tonumber(lua, 2);

        // Set variable value.
        variable->SetFloat((float)value);

        // Return nothing.
        return 0;
    }

    int GetString(lua_State* lua)
    {
        // Get the console variable.
        ConsoleVariable* variable = GetVariable(lua);

        if(variable == nullptr)
        {
            lua_pushnil(lua);
            return 1;
        }

        // Return the value.
        lua_pushstring(lua, variable->GetString().c_str());
        return 1;
    }

    int GetBool(lua_State* lua)
    {
        // Get the console variable.
        ConsoleVariable* variable = GetVariable(lua);

        if(variable == nullptr)
        {
            lua_pushnil(lua);
            return 1;
        }

        // Return the value.
        lua_pushboolean(lua, variable->GetBool());
        return 1;
    }

    int GetInteger(lua_State* lua)
    {
        // Get the console variable.
        ConsoleVariable* variable = GetVariable(lua);

        if(variable == nullptr)
        {
            lua_pushnil(lua);
            return 1;
        }

        // Return the value.
        lua_pushnumber(lua, variable->GetInteger());
        return 1;
    }

    int GetFloat(lua_State* lua)
    {
        // Get the console variable.
        ConsoleVariable* variable = GetVariable(lua);

        if(variable == nullptr)
        {
            lua_pushnil(lua);
            return 1;
        }

        // Return the value.
        lua_pushnumber(lua, variable->GetFloat());
        return 1;
    }
}

bool BindLuaConsole(LuaEngine& lua)
{
    if(!lua.IsValid())
        return false;

    // Bind definitions.
    Lua::getGlobalNamespace(lua.GetState())
        .beginNamespace("Console")
            .addFunction("Execute", &Execute)
            .addCFunction("SetString", &SetString)
            .addCFunction("SetBool", &SetBool)
            .addCFunction("SetInteger", &SetInteger)
            .addCFunction("SetFloat", &SetFloat)
            .addCFunction("GetString", &GetString)
            .addCFunction("GetBool", &GetBool)
            .addCFunction("GetInteger", &GetInteger)
            .addCFunction("GetFloat", &GetFloat)
        .endNamespace();

    return true;
}
