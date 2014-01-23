#pragma once

#include "Precompiled.hpp"
#include "Component.hpp"

//
// Render Component
//

class Render : public Component
{
public:
    Render();
    ~Render();

    // Sets the color.
    void SetColor(const glm::vec4& color)
    {
        m_color = color;
    }

    // Gets the color.
    const glm::vec4& GetColor() const
    {
        return m_color;
    }

private:
    // Entity color.
    glm::vec4 m_color;
};
