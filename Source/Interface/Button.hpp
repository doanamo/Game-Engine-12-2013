#pragma once

#include "Precompiled.hpp"
#include "Element.hpp"

#include "Scripting/LuaDelegate.hpp"

//
// Interface
//

namespace Interface
{
    //
    // Button
    //

    class Button : public Element
    {
    public:
        Button();
        virtual ~Button();

        void Cleanup();

        virtual bool Process(const SDL_Event& event);
        virtual void Update(float timeDelta);
        //virtual void Draw() = 0;

        void Enable();
        void Disable();

        void SetEnabled(bool enabled);
        void SetText(std::string text);
        void SetPosition(const glm::vec2& position);
        void SetSize(const glm::vec2& size);

        const std::string& GetText() const;
        const glm::vec2& GetPosition() const;
        const glm::vec2& GetSize() const;

        bool IsEnabled() const;
        bool IsHovered() const;
        bool IsPressed() const;

    public:
        // Arguments: None
        void OnEventAction(Lua::LuaRef function, Lua::LuaRef instance);

        // Arguments:
        //  hovered - true if cursor is entered area over the button, false if left.
        void OnEventHovered(Lua::LuaRef function, Lua::LuaRef instance);

        // Arguments:
        //  pressed - true if button has been pressed, false if released.
        void OnEventPressed(Lua::LuaRef function, Lua::LuaRef instance);

    private:
        // Button state.
        std::string m_text;
        glm::vec2 m_position;
        glm::vec2 m_size;

        bool m_enabled;
        bool m_hovered;
        bool m_pressed;

        // Script delegates.
        LuaDelegate m_eventAction;
        LuaDelegate m_eventPressed;
        LuaDelegate m_eventHovered;
    };
} // namespace Interface
