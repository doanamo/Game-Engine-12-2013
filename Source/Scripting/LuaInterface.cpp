#include "Precompiled.hpp"
#include "LuaInterface.hpp"
#include "LuaEngine.hpp"

#include "Interface/Element.hpp"
#include "Interface/Root.hpp"
#include "Interface/Button.hpp"

bool BindLuaInterface(LuaEngine& lua)
{
    if(!lua.IsValid())
        return false;

    // Bind definitions.
    Lua::getGlobalNamespace(lua.GetState())
        .beginNamespace("Interface")
            .beginClass<Interface::Element>("Element")
            .endClass()
        .endNamespace();

    Lua::getGlobalNamespace(lua.GetState())
        .beginNamespace("Interface")
            .deriveClass<Interface::Root, Interface::Element>("Root")
                .addConstructor<void(*)(void)>()
            .endClass()
        .endNamespace();

    Lua::getGlobalNamespace(lua.GetState())
        .beginNamespace("Interface")
            .deriveClass<Interface::Button, Interface::Element>("Button")
                .addConstructor<void(*)(void)>()
            .endClass()
        .endNamespace();

    return true;
}
