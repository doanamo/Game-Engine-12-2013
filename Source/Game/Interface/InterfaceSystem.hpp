#pragma once

#include "Precompiled.hpp"
#include "ValueBar.hpp"
#include "FloatingText.hpp"

#include "Common/Receiver.hpp"
#include "Graphics/ScreenSpace.hpp"
#include "Game/GameEvents.hpp"

//
// Interface System
//

class InterfaceSystem
{
public:
    InterfaceSystem();
    ~InterfaceSystem();

    bool Initialize();
    void Cleanup();

    void Update(float timeDelta);
    void Draw();

    void AddFloatingText(std::string text, const glm::vec2& position, const FloatingTextInterface* interface);

public:
    ReceiverSignature<GameEvent::EntityDamaged> GetEntityDamagedReceiver();
    ReceiverSignature<GameEvent::EntityHealed> GetEntityHealedReceiver();

    void OnEntityDamagedEvent(const GameEvent::EntityDamaged& event);
    void OnEntityHealedEvent(const GameEvent::EntityHealed& event);

private:
    bool m_initialized;

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
