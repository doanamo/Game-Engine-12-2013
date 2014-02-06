#pragma once

#include "Precompiled.hpp"
#include "Component.hpp"

//
// Collision Component
//

class CollisionComponent : public Component
{
public:
    CollisionComponent();
    ~CollisionComponent();

    void SetBoundingBox(const glm::vec4& boundingBox)
    {
        m_boundingBox = boundingBox;
    }

    const glm::vec4& GetBoundingBox() const
    {
        return m_boundingBox;
    }

    void Enable()
    {
        m_enabled = true;
    }

    void Disable()
    {
        m_enabled = false;
    }

    bool IsEnabled() const
    {
        return m_enabled;
    }

private:
    // Collision bounding box.
    glm::vec4 m_boundingBox;
    
    // Collision enabled.
    bool m_enabled;
};
