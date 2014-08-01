#include "Precompiled.hpp"
#include "InterfaceSystem.hpp"

#include "MainGlobal.hpp"
#include "Common/Services.hpp"
#include "Graphics/TextRenderer.hpp"
#include "Game/Event/EventSystem.hpp"
#include "Game/Entity/EntitySystem.hpp"
#include "Game/Identity/IdentitySystem.hpp"
#include "Game/Component/ComponentSystem.hpp"
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
    m_initialized(false),
    m_eventSystem(nullptr),
    m_entitySystem(nullptr),
    m_identitySystem(nullptr),
    m_componentSystem(nullptr),
    m_renderSystem(nullptr)
{
}

InterfaceSystem::~InterfaceSystem()
{
    Cleanup();
}

void InterfaceSystem::Cleanup()
{
    m_initialized = false;

    // Game systems.
    m_eventSystem = nullptr;
    m_entitySystem = nullptr;
    m_identitySystem = nullptr;
    m_componentSystem = nullptr;
    m_renderSystem = nullptr;

    // Screen space.
    m_screenSpace.Cleanup();

    // Player health bar.
    m_playerHealthBar.Cleanup();
    
    // Floating text elements.
    ClearContainer(m_floatingTextList);

    // Event receivers.
    m_receiverEntityDamaged.Cleanup();
    m_receiverEntityHealed.Cleanup();
}

bool InterfaceSystem::Initialize(const Services& services)
{
    Cleanup();

    // Setup scope guard.
    SCOPE_GUARD_IF(!m_initialized, Cleanup());

    // Validate arguments.
    m_eventSystem = services.Get<EventSystem>();
    if(m_eventSystem == nullptr) return false;

    m_entitySystem = services.Get<EntitySystem>();
    if(m_entitySystem == nullptr) return false;

    m_identitySystem = services.Get<IdentitySystem>();
    if(m_identitySystem == nullptr) return false;

    m_componentSystem = services.Get<ComponentSystem>();
    if(m_componentSystem == nullptr) return false;

    m_renderSystem = services.Get<RenderSystem>();
    if(m_renderSystem == nullptr) return false;

    // Bind event receivers.
    m_receiverEntityDamaged.Bind<InterfaceSystem, &InterfaceSystem::OnEntityDamagedEvent>(this);
    m_receiverEntityHealed.Bind<InterfaceSystem, &InterfaceSystem::OnEntityHealedEvent>(this);

    // Subscribe event receivers.
    m_eventSystem->Subscribe(m_receiverEntityDamaged);
    m_eventSystem->Subscribe(m_receiverEntityHealed);

    // Declare required components.
    m_componentSystem->Declare<TransformComponent>();
    m_componentSystem->Declare<HealthComponent>();

    // Initialize the health bar.
    m_playerHealthBar.SetDrawingRectangle(glm::vec4(0.0f, 0.0f, 624.0f, 15.0f));
    m_playerHealthBar.SetForegroundColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    m_playerHealthBar.SetBackgroundColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    m_playerHealthBar.SetDecayColor(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));

    // Success!
    return m_initialized = true;
}

void InterfaceSystem::Update(float timeDelta)
{
    assert(m_initialized);

    // Update the health bar element.
    EntityHandle playerEntity = m_identitySystem->GetEntityByName("Player");
    HealthComponent* playerHealth = m_componentSystem->Lookup<HealthComponent>(playerEntity);

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
    assert(m_initialized);

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
        info.font = &Main::GetDefaultFont();
        info.align = TextDrawAlign::Centered;
        info.size = 22 * element.interface->CalculateScale(element.lifetime);
        info.position = element.origin + element.interface->CalculateOffset(element.lifetime);
        info.bodyColor = glm::vec4(element.interface->CalculateColor(element.lifetime), element.interface->CalculateTransparency(element.lifetime));
        info.glowColor = glm::vec4(0.0f, 0.0f, 0.0f, element.interface->CalculateTransparency(element.lifetime));
        info.glowRange = glm::vec2(0.2f, 0.5f);
        
        Main::GetTextRenderer().Draw(info, element.text.c_str(), m_screenSpace.GetTransform());
    }

    //
    // Debug Info
    //

    // Set aspect screen space target.
    m_screenSpace.SetTargetAspect(16.0f / 9.0f);

    // Draw debug game info.
    {
        std::stringstream text;
        text << "Entities: " << m_entitySystem->GetEntityCount();

        TextDrawInfo info;
        info.font = &Main::GetDefaultFont();
        info.size = 22;
        info.bodyColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        info.glowColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        info.glowRange = glm::vec2(0.2f, 0.5f);
        info.position.x = 10.0f;
        info.position.y = m_screenSpace.GetTargetSize().y - 5.0f;

        Main::GetTextRenderer().Draw(info, text.str().c_str(), m_screenSpace.GetTransform());
    }
}

void InterfaceSystem::AddFloatingText(std::string text, const glm::vec2& position, const FloatingTextInterface* interface)
{
    assert(m_initialized);

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
        const ScreenSpace& gameScreenSpace = m_renderSystem->GetScreenSpace();
        glm::ivec4 viewport(0, 0, Console::windowWidth, Console::windowHeight);

        // Project position from the world to window space.
        glm::vec3 windowPosition = glm::project(glm::vec3(element.origin, 0.0f), gameScreenSpace.GetView(), gameScreenSpace.GetProjection(), viewport);

        // Set the new floating text origin.
        element.origin = glm::vec2(windowPosition.x, windowPosition.y);
    }

    // Add floating text element to the list.
    m_floatingTextList.push_back(element);
}

void InterfaceSystem::OnEntityDamagedEvent(const GameEvent::EntityDamaged& event)
{
    assert(m_initialized);

    // Get the transform component.
    TransformComponent* transform = m_componentSystem->Lookup<TransformComponent>(event.entity);
    if(transform == nullptr) return;

    // Add a floating text element.
    AddFloatingText(std::to_string(event.value), transform->GetPosition(), &FloatingDamageNumber::GetInstance());
}

void InterfaceSystem::OnEntityHealedEvent(const GameEvent::EntityHealed& event)
{
    assert(m_initialized);

    // Get the transform component.
    TransformComponent* transform = m_componentSystem->Lookup<TransformComponent>(event.entity);
    if(transform == nullptr) return;

    // Add a floating text element.
    AddFloatingText(std::to_string(event.value), transform->GetPosition(), &FloatingHealNumber::GetInstance());
}

