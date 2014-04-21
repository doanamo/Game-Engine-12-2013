#include "Precompiled.hpp"
#include "FloatingText.hpp"

//
// Floating Damage Number
//

const float FloatingDamageNumber::Lifetime = 1.0f;

const FloatingDamageNumber& FloatingDamageNumber::GetInstance()
{
    static FloatingDamageNumber instance;
    return instance;
}

glm::vec2 FloatingDamageNumber::CalculateOffset(float lifetime) const
{
    return glm::vec2(0.0f, 0.0f);
}

float FloatingDamageNumber::CalculateScale(float lifetime) const
{
    return 1.0f;
}

glm::vec3 FloatingDamageNumber::CalculateColor(float lifetime) const
{
    return glm::vec3(1.0f, 0.0f, 0.0f);
}

float FloatingDamageNumber::CalculateTransparency(float lifetime) const
{
    return (Lifetime - lifetime) / Lifetime;
}

float FloatingDamageNumber::GetLifetime() const
{
    return Lifetime;
}

//
// Floating Heal Number
//

const float FloatingHealNumber::Lifetime = 2.0f;

const FloatingHealNumber& FloatingHealNumber::GetInstance()
{
    static FloatingHealNumber instance;
    return instance;
}

glm::vec2 FloatingHealNumber::CalculateOffset(float lifetime) const
{
    return glm::vec2(0.0f, 0.0f);
}

float FloatingHealNumber::CalculateScale(float lifetime) const
{
    return 1.0f;
}

glm::vec3 FloatingHealNumber::CalculateColor(float lifetime) const
{
    return glm::vec3(0.0f, 1.0f, 0.0f);
}

float FloatingHealNumber::CalculateTransparency(float lifetime) const
{
    return (Lifetime - lifetime) / Lifetime;
}

float FloatingHealNumber::GetLifetime() const
{
    return Lifetime;
}
