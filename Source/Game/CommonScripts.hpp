#pragma once

#include "Precompiled.hpp"
#include "Script.hpp"

// Forward declarations.
class TransformComponent;
class RenderComponent;

//
// Constant Velocity
//

class ConstantVelocityScript : public Script
{
public:
    ConstantVelocityScript(const glm::vec2& velocity);

    void OnUpdate(EntityHandle self, float timeDelta);

private:
    TransformComponent* GetTransformComponent(EntityHandle self);

private:
    // Cached components.
    TransformComponent* m_transform;

    // Constant velocity.
    glm::vec2 m_velocity;
};

//
// Destroy On Death
//

class DestroyOnDeathScript : public Script
{
public:
    void OnDamage(EntityHandle self, int value, bool alive);
};

//
// Flash On Damage
//

class FlashOnDamageScript : public Script
{
public:
    FlashOnDamageScript();

    void OnUpdate(EntityHandle self, float timeDelta);
    void OnDamage(EntityHandle self, int value, bool alive);

private:
    RenderComponent* GetRenderComponent(EntityHandle self);

private:
    // Cached components.
    RenderComponent* m_render;

    // Blink timer.
    float m_timer;
};
