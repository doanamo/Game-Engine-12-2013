#include "Precompiled.hpp"
#include "InterfaceRoot.hpp"

#include "MainContext.hpp"
#include "Graphics/ScreenSpace.hpp"

InterfaceRoot::InterfaceRoot() :
    InterfaceElement(this),
    m_screenSpace(nullptr),
    m_windowCursorPosition(0, 0),
    m_interfaceCursorPosition(0.0f, 0.0f),
    m_rebuildCursorPosition(true)
{
}

InterfaceRoot::~InterfaceRoot()
{
    Cleanup();
}

void InterfaceRoot::Cleanup()
{
    InterfaceElement::Cleanup();

    m_screenSpace = nullptr;
}

bool InterfaceRoot::Process(const SDL_Event& event)
{
    // Intercept mouse motion event.
    if(event.type == SDL_MOUSEMOTION)
    {
        m_windowCursorPosition.x = event.motion.x;
        m_windowCursorPosition.y = event.motion.y;
        m_rebuildCursorPosition = true;
    }

    // Continue processing for childs.
    return InterfaceElement::Process(event);
}

void InterfaceRoot::SetScreenSpace(ScreenSpace* screenSpace)
{
    m_screenSpace = screenSpace;
}

const glm::vec2& InterfaceRoot::GetCursorPosition() const
{
    CalculateCursorPosition();
    return m_interfaceCursorPosition;
}

void InterfaceRoot::CalculateCursorPosition() const
{
    if(m_rebuildCursorPosition)
    {
        // Set the initial position.
        m_interfaceCursorPosition.x = (float)m_windowCursorPosition.x;
        m_interfaceCursorPosition.y = (float)m_windowCursorPosition.y;

        // Transform to interface's screen space.
        if(m_screenSpace != nullptr)
        {
            // Flip cursor y-axis.
            m_interfaceCursorPosition.y = Console::windowHeight.GetFloat() - m_interfaceCursorPosition.y - 1.0f;

            // Unproject cursor position to game space.
            glm::vec4 viewport(0.0f, 0.0f, Console::windowWidth, Console::windowHeight);
            glm::vec3 transformedPosition = glm::unProject(glm::vec3(m_interfaceCursorPosition, 0.0f), m_screenSpace->GetView(), m_screenSpace->GetProjection(), viewport);

            // Save the transformed position.
            m_interfaceCursorPosition.x = transformedPosition.x;
            m_interfaceCursorPosition.y = transformedPosition.y;
        }

        // Done!
        m_rebuildCursorPosition = true;
    }
}
