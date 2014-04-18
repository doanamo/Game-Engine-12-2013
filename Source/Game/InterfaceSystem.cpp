#include "Precompiled.hpp"
#include "InterfaceSystem.hpp"

#include "MainContext.hpp"
#include "GameContext.hpp"
#include "EntitySystem.hpp"
#include "IdentitySystem.hpp"

#include "HealthComponent.hpp"

#include "Graphics/TextRenderer.hpp"

namespace
{
    // Interface space size.
    float interfaceWidth = 1024;
    float interfaceHeight = 576;
}

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

    // Initialize the health bar.
    m_playerHealthBar.SetDrawingRectangle(glm::vec4(0.0f, 0.0f, 624.0f, 15.0f));
    m_playerHealthBar.SetForegroundColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    m_playerHealthBar.SetBackgroundColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    m_playerHealthBar.SetDecayColor(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));

    m_initialized = true;

    return true;
}

void InterfaceSystem::Cleanup()
{
    m_screenSpace.Cleanup();

    m_playerHealthBar.Cleanup();

    m_initialized = false;
}

void InterfaceSystem::Update(float timeDelta)
{
    if(!m_initialized)
        return;

    // Update the health bar.
    EntityHandle playerEntity = Game::IdentitySystem().GetEntityByName("Player");
    HealthComponent* playerHealth = Game::HealthComponents().Lookup(playerEntity);

    if(playerHealth != nullptr)
    {
        m_playerHealthBar.SetMaximumValue((float)playerHealth->GetMaximumHealth());
        m_playerHealthBar.SetCurrentValue((float)playerHealth->GetCurrentHealth());
    }

    m_playerHealthBar.Update(timeDelta);
}

void InterfaceSystem::Draw()
{
    if(!m_initialized)
        return;

    // Set screen space source size.
    float windowWidth = Console::windowWidth;
    float windowHeight = Console::windowHeight;

    m_screenSpace.SetSourceSize(windowWidth, windowHeight);

    // Set unscaled screen space target.
    m_screenSpace.SetTargetSize(interfaceWidth, interfaceHeight);

    // Draw the health bar.
    m_playerHealthBar.Draw(glm::vec2(200.0f, 10.0f), m_screenSpace.GetTransform());

    // Set scaled screen space target.
    m_screenSpace.SetTargetAspect(16.0f / 9.0f);

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
