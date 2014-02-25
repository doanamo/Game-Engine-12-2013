#pragma once

#include "Precompiled.hpp"
#include "Script.hpp"

// Forward declarations.
class RenderComponent;

//
// Blink On Damage
//

class BlinkOnDamageScript : public Script
{
public:
    BlinkOnDamageScript();

    void OnUpdate(EntityHandle self, float timeDelta);
    void OnDamage(EntityHandle self, int value, bool alive);

    RenderComponent* GetRenderComponent(EntityHandle self);

private:
    // Render component.
    RenderComponent* m_render;

    // Blink timer.
    float m_timer;
};
