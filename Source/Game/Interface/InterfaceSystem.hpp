#pragma once

#include "Precompiled.hpp"
#include "ValueBar.hpp"
#include "FloatingText.hpp"

#include "Graphics/ScreenSpace.hpp"

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
};
