#include "Precompiled.hpp"
#include "Button.hpp"
#include "Root.hpp"
using namespace Interface;

#include "MainGlobal.hpp"

Button::Button() :
    m_text(),
    m_position(0.0f, 0.0f),
    m_size(0.0f, 0.0f),
    m_enabled(true),
    m_hovered(false),
    m_pressed(false)
{
}

Button::~Button()
{
    Cleanup();
}

void Button::Cleanup()
{
    Element::Cleanup();

    m_text.clear();
    m_position = glm::vec2(0.0f, 0.0f);
    m_size = glm::vec2(0.0f, 0.0f);

    m_enabled = true;
    m_hovered = false;
    m_pressed = false;

    m_eventAction.Cleanup();
    m_eventPressed.Cleanup();
    m_eventHovered.Cleanup();

    m_eventActionLua.Cleanup();
    m_eventPressedLua.Cleanup();
    m_eventHoveredLua.Cleanup();
}

bool Button::Process(const SDL_Event& event)
{
    if(!m_enabled)
        return false;

    switch(event.type)
    {
    case SDL_MOUSEMOTION:
        {
            // Get cursor position in interface space.
            glm::vec2 cursorPosition = GetRoot()->GetCursorPosition();

            // Checks if cursor is above the button.
            bool hovered =
                cursorPosition.x >= m_position.x && cursorPosition.x < m_position.x + m_size.x &&
                cursorPosition.y >= m_position.y && cursorPosition.y < m_position.y + m_size.y;

            if(m_hovered != hovered)
            {
                m_eventHovered.Dispatch(EventHovered(hovered));
                m_eventHoveredLua.Invoke(hovered);
                m_hovered = hovered;
            }
        }
        break;

    case SDL_MOUSEBUTTONDOWN:
        if(event.button.button == SDL_BUTTON_LEFT)
        {
            // Set the pressed state if cursor is above the button.
            if(m_hovered)
            {
                m_eventPressed.Dispatch(EventPressed(true));
                m_eventPressedLua.Invoke(true);
                m_pressed = true;
            }
        }
        return m_hovered;

    case SDL_MOUSEBUTTONUP:
        if(event.button.button == SDL_BUTTON_LEFT)
        {
            if(m_pressed)
            {
                if(m_hovered)
                {
                    m_eventAction.Dispatch(EventAction());
                    m_eventActionLua.Invoke();
                }

                m_eventPressed.Dispatch(EventPressed(false));
                m_eventPressedLua.Invoke(false);
                m_pressed = false;
            }
        }
        return m_hovered;
    }

    return false;
}

void Button::Update(float timeDelta)
{
}

void Button::OnEventAction(const ReceiverSignature<EventAction>& receiver)
{
    m_eventAction.Subscribe(receiver);
}

void Button::OnEventHovered(const ReceiverSignature<EventHovered>& receiver)
{
    m_eventHovered.Subscribe(receiver);
}

void Button::OnEventPressed(const ReceiverSignature<EventPressed>& receiver)
{
    m_eventPressed.Subscribe(receiver);
}

void Button::OnEventActionLua(Lua::LuaRef function, Lua::LuaRef instance)
{
    m_eventActionLua.Bind(function, instance);
}

void Button::OnEventHoveredLua(Lua::LuaRef function, Lua::LuaRef instance)
{
    m_eventHoveredLua.Bind(function, instance);
}

void Button::OnEventPressedLua(Lua::LuaRef function, Lua::LuaRef instance)
{
    m_eventPressedLua.Bind(function, instance);
}

void Button::Enable()
{
    m_enabled = true;
}

void Button::Disable()
{
    m_enabled = false;

    // Clear related flags.
    m_hovered = false;
    m_pressed = false;
}

void Button::SetEnabled(bool enabled)
{
    if(enabled)
    {
        Enable();
    }
    else
    {
        Disable();
    }
}

void Button::SetText(std::string text)
{
    m_text = text;
}

void Button::SetPosition(const glm::vec2& position)
{
    m_position = position;
}

void Button::SetSize(const glm::vec2& size)
{
    m_size = size;
}

const std::string& Button::GetText() const
{
    return m_text;
}

const glm::vec2& Button::GetPosition() const
{
    return m_position;
}

const glm::vec2& Button::GetSize() const
{
    return m_size;
}

bool Button::IsEnabled() const
{
    return m_enabled;
}

bool Button::IsHovered() const
{
    return m_hovered;
}

bool Button::IsPressed() const
{
    return m_pressed;
}
