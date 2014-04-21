#pragma once

#include "Precompiled.hpp"

//
// Floating Text Interface
//

class FloatingTextInterface
{
public:
    virtual glm::vec2 CalculateOffset(float lifetime) const = 0;
    virtual float CalculateScale(float lifetime) const = 0;
    virtual glm::vec3 CalculateColor(float lifetime) const = 0;
    virtual float CalculateTransparency(float lifetime) const = 0;
    virtual float GetLifetime() const = 0;
};

//
// Floating Damage Number
//

class FloatingDamageNumber : public FloatingTextInterface
{
public:
    static const FloatingDamageNumber& GetInstance();

    glm::vec2 CalculateOffset(float lifetime) const;
    float CalculateScale(float lifetime) const;
    glm::vec3 CalculateColor(float lifetime) const;
    float CalculateTransparency(float lifetime) const;
    float GetLifetime() const;

private:
    static const float Lifetime;  
};

//
// Floating Heal Number
//

class FloatingHealNumber : public FloatingTextInterface
{
public:
    static const FloatingHealNumber& GetInstance();

    glm::vec2 CalculateOffset(float lifetime) const;
    float CalculateScale(float lifetime) const;
    glm::vec3 CalculateColor(float lifetime) const;
    float CalculateTransparency(float lifetime) const;
    float GetLifetime() const;

private:
    static const float Lifetime;  
};
