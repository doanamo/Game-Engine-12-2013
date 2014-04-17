#include "Precompiled.hpp"
#include "InterfaceSystem.hpp"

#include "MainContext.hpp"
#include "GameContext.hpp"
#include "EntitySystem.hpp"

#include "Graphics/TextRenderer.hpp"

InterfaceSystem::InterfaceSystem() :
    m_initialized(false)
{
}

InterfaceSystem::~InterfaceSystem()
{
    Cleanup();
}

bool InterfaceSystem::Initialize()
{
    Cleanup();

    m_initialized = true;

    return true;
}

void InterfaceSystem::Cleanup()
{
    m_screenSpace.Cleanup();

    m_initialized = false;
}

void InterfaceSystem::Update(float timeDelta)
{
    if(!m_initialized)
        return;

    // Get window size.
    float windowWidth = Console::windowWidth;
    float windowHeight = Console::windowHeight;

    // Setup screen space.
    m_screenSpace.SetSourceSize(windowWidth, windowHeight);
    m_screenSpace.SetTargetAspect(16.0f / 9.0f);
}

void InterfaceSystem::Draw()
{
    if(!m_initialized)
        return;

    // Draw debug game info.
    {
        std::stringstream text;
        text << "Entities: " << Game::EntitySystem().GetEntityCount();

        TextDrawInfo info;
        info.font = &Main::DefaultFont();
        info.size = 22;
        info.bodyColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        info.glowColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        info.glowRange = glm::vec2(0.2f, 0.5f);
        info.position.x = 10.0f;
        info.position.y = m_screenSpace.GetTargetSize().y - 5.0f;

        Main::TextRenderer().Draw(info, m_screenSpace.GetTransform(), text.str().c_str());
    }
}
