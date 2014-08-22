#include "Precompiled.hpp"
#include "LuaBindings.hpp"
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
                .addFunction("AddChild", &Interface::Element::AddChild)
                .addFunction("RemoveChild", &Interface::Element::RemoveChild)
                .addFunction("RemoveChildren", &Interface::Element::RemoveChildren)
            .endClass()
        .endNamespace();

    Lua::getGlobalNamespace(lua.GetState())
        .beginNamespace("Interface")
            .deriveClass<Interface::Root, Interface::Element>("Root")
                .addConstructor<void(*)(void)>()
                .addFunction("SetScreenSpace", &Interface::Root::SetScreenSpace)
                .addFunction("Process", &Interface::Root::Process)
            .endClass()
        .endNamespace();

    Lua::getGlobalNamespace(lua.GetState())
        .beginNamespace("Interface")
            .deriveClass<Interface::Button, Interface::Element>("Button")
                .addConstructor<void(*)(void)>()
                .addFunction("Enable", &Interface::Button::Enable)
                .addFunction("Disable", &Interface::Button::Disable)
                .addFunction("SetEnabled", &Interface::Button::SetEnabled)
                .addFunction("SetText", &Interface::Button::SetText)
                .addFunction("SetPosition", &Interface::Button::SetPosition)
                .addFunction("SetSize", &Interface::Button::SetSize)
                .addFunction("GetText", &Interface::Button::GetText)
                .addFunction("GetPosition", &Interface::Button::GetPosition)
                .addFunction("GetSize", &Interface::Button::GetSize)
                .addFunction("IsEnabled", &Interface::Button::IsEnabled)
                .addFunction("IsHovered", &Interface::Button::IsHovered)
                .addFunction("IsPressed", &Interface::Button::IsPressed)
                .addFunction("OnEventAction", &Interface::Button::OnEventAction)
                .addFunction("OnEventHovered", &Interface::Button::OnEventHovered)
                .addFunction("OnEventPressed", &Interface::Button::OnEventPressed)
            .endClass()
        .endNamespace();

    return true;
}
