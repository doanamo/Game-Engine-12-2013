#include "Precompiled.hpp"
#include "InterfaceSystem.hpp"

#include "MainContext.hpp"
#include "Graphics/TextRenderer.hpp"
#include "Game/GameContext.hpp"
#include "Game/GameState.hpp"
#include "Game/Entity/EntitySystem.hpp"
#include "Game/Identity/IdentitySystem.hpp"
#include "Game/Transform/TransformComponent.hpp"
#include "Game/Health/HealthComponent.hpp"
#include "Game/Render/RenderSystem.hpp"

namespace
{
    // Interface space size.
    const float InterfaceWidth = 1024;
    const float InterfaceHeight = 576;

    // Interface parameters.
    const bool InterfaceSpaceFloatingText = false;
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

    // Bind the event receiver.
    m_receiverHealthChange.Bind<InterfaceSystem, &InterfaceSystem::OnHealthChangeEvent>(this);

    // Success!
    m_initialized = true;

    return true;
}

void InterfaceSystem::Cleanup()
{
    m_initialized = false;

    // Screen space.
    m_screenSpace.Cleanup();

    // Player health bar.
    m_playerHealthBar.Cleanup();
    
    // Floating text elements.
    ClearContainer(m_floatingTextList);

    // Event receivers.
    m_receiverHealthChange.Cleanup();
}

void InterfaceSystem::Update(float timeDelta)
{
    if(!m_initialized)
        return;

    // Update the health bar element.
    EntityHandle playerEntity = GameState::IdentitySystem().GetEntityByName("Player");
    HealthComponent* playerHealth = GameState::HealthComponents().Lookup(playerEntity);

    if(playerHealth != nullptr)
    {
        m_playerHealthBar.SetMaximumValue((float)playerHealth->GetMaximumHealth());
        m_playerHealthBar.SetCurrentValue((float)playerHealth->GetCurrentHealth());
    }

    m_playerHealthBar.Update(timeDelta);

    // Update lifetimes of floating text elements.
    for(auto it = m_floatingTextList.begin(); it != m_floatingTextList.end(); ++it)
    {
        FloatingText& element = *it;

        if(element.lifetime < 0.0f)
        {
            // Set initial value.
            element.lifetime = 0.0f;
        }
        else
        {
            // Increment lifetime.
            element.lifetime += timeDelta;
        }
    }

    // Remove floating text elements that reached their lifetimes.
    for(auto it = m_floatingTextList.begin(); it != m_floatingTextList.end(); /* inloop */)
    {
        FloatingText& element = *it;

        assert(element.interface != nullptr);

        if(element.lifetime > element.interface->GetLifetime())
        {
            it = m_floatingTextList.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void InterfaceSystem::Draw()
{
    if(!m_initialized)
        return;

    // Set screen space source size.
    float windowWidth = Console::windowWidth;
    float windowHeight = Console::windowHeight;

    m_screenSpace.SetSourceSize(windowWidth, windowHeight);

    //
    // Ingame Interface
    //

    // Set scaled screen space target.
    m_screenSpace.SetTargetSize(InterfaceWidth, InterfaceHeight);

    // Draw the health bar.
    m_playerHealthBar.Draw(glm::vec2(200.0f, 10.0f), m_screenSpace.GetTransform());

    //
    // Floating Text
    //

    // Set window screen space target.
    if(InterfaceSpaceFloatingText)
    {
        m_screenSpace.SetTargetSize(windowWidth, windowHeight);
    }

    // Draw floating text elements.
    for(auto it = m_floatingTextList.begin(); it != m_floatingTextList.end(); ++it)
    {
        const FloatingText& element = *it;

        assert(element.lifetime >= 0.0f);

        // Draw text.
        TextDrawInfo info;
        info.font = &Main::DefaultFont();
        info.align = TextDrawAlign::Centered;
        info.size = 22 * element.interface->CalculateScale(element.lifetime);
        info.position = element.origin + element.interface->CalculateOffset(element.lifetime);
        info.bodyColor = glm::vec4(element.interface->CalculateColor(element.lifetime), element.interface->CalculateTransparency(element.lifetime));
        info.glowColor = glm::vec4(0.0f, 0.0f, 0.0f, element.interface->CalculateTransparency(element.lifetime));
        info.glowRange = glm::vec2(0.2f, 0.5f);
        
        Main::TextRenderer().Draw(info, m_screenSpace.GetTransform(), element.text.c_str());
    }

    //
    // Debug Info
    //

    // Set aspect screen space target.
    m_screenSpace.SetTargetAspect(16.0f / 9.0f);

    // Draw debug game info.
    {
        std::stringstream text;
        text << "Entities: " << GameState::EntitySystem().GetEntityCount();

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

void InterfaceSystem::AddFloatingText(std::string text, const glm::vec2& position, const FloatingTextInterface* interface)
{
    if(!m_initialized)
        return;

    if(text.empty())
        return;

    if(interface == nullptr)
        return;

    // Create a floating text element.
    FloatingText element;
    element.text = text;
    element.origin = position;
    element.interface = interface;
    element.lifetime = -1.0f;

    // Transform element position to interface space.
    if(InterfaceSpaceFloatingText)
    {
        // Get the source transform along with current viewport.
        const ScreenSpace& gameScreenSpace = GameState::RenderSystem().GetScreenSpace();
        glm::ivec4 viewport(0, 0, Console::windowWidth, Console::windowHeight);

        // Project position from the world to window space.
        glm::vec3 windowPosition = glm::project(glm::vec3(element.origin, 0.0f), gameScreenSpace.GetView(), gameScreenSpace.GetProjection(), viewport);

        // Set the new floating text origin.
        element.origin = glm::vec2(windowPosition.x, windowPosition.y);
    }

    // Add floating text element to the list.
    m_floatingTextList.push_back(element);
}

ReceiverSignature<HealthChangeEvent> InterfaceSystem::GetHealthChangeReceiver()
{
    return m_receiverHealthChange;
}

void InterfaceSystem::OnHealthChangeEvent(const HealthChangeEvent& event)
{
    // Handle health change event for floating text display.
    TransformComponent* transform = GameState::TransformComponents().Lookup(event.entity);
    if(transform == nullptr) return;

    if(event.value < 0)
    {
        // Add floating text for damage.
        std::string text = std::to_string(event.value * -1);
        AddFloatingText(text, transform->GetPosition(), &FloatingDamageNumber::GetInstance());
    }
    else
    if(event.value > 0)
    {
        // Add floating text for healing.
        std::string text = std::to_string(event.value);
        AddFloatingText(text, transform->GetPosition(), &FloatingHealNumber::GetInstance());
    }
}
