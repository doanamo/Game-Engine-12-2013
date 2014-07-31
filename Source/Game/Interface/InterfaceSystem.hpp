#pragma once

#include "Precompiled.hpp"
#include "ValueBar.hpp"
#include "FloatingText.hpp"

#include "Common/Receiver.hpp"
#include "Graphics/ScreenSpace.hpp"
#include "Game/Event/EventDefinitions.hpp"

// Forward declarations
class EventSystem;
class EntitySystem;
class IdentitySystem;
class ComponentSystem;
class RenderSystem;

//
// Interface System
//

class InterfaceSystem
{
public:
    InterfaceSystem();
    ~InterfaceSystem();

    bool Initialize(EventSystem* eventSystem, EntitySystem* entitySystem, IdentitySystem* identitySystem, ComponentSystem* componentSystem, RenderSystem* renderSystem);
    void Cleanup();

    void Update(float timeDelta);
    void Draw();

    void AddFloatingText(std::string text, const glm::vec2& position, const FloatingTextInterface* interface);

public:
    void OnEntityDamagedEvent(const GameEvent::EntityDamaged& event);
    void OnEntityHealedEvent(const GameEvent::EntityHealed& event);

private:
    // System state.
    bool m_initialized;

    // Game systems;
    EventSystem*     m_eventSystem;
    EntitySystem*    m_entitySystem;
    IdentitySystem*  m_identitySystem;
    ComponentSystem* m_componentSystem;
    RenderSystem*    m_renderSystem;

    // Screen space.
    ScreenSpace m_screenSpace;

    // Player health bar.
    ValueBar m_playerHealthBar;
    
    // Floating text elements.
    struct FloatingText
    {
        std::string text;
        glm::vec2 origin;
        const FloatingTextInterface* interface;
        float lifetime;
    };

    std::vector<FloatingText> m_floatingTextList;

    // Event receivers.
    Receiver<GameEvent::EntityDamaged> m_receiverEntityDamaged;
    Receiver<GameEvent::EntityHealed> m_receiverEntityHealed;
};
